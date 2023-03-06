#include "EfkMgr.h"
#include "Logger.h"
#include "EnumIterator.h"
#include "Resource/EfkType.h"
#include "Tool/JsonLoader.h"
#include "Helper/StringHelper.h"

#include "Timer.h"

// Link Effekseer
#if defined(DEBUG) || (_DEBUG)
#pragma comment(lib,"Effekseerd.lib")
#pragma comment(lib,"EffekseerRendererDX12d.lib")
#pragma comment(lib,"LLGId.lib")
# else
#pragma comment(lib,"Effekseer.lib")
#pragma comment(lib,"EffekseerRendererDX12.lib")
#pragma comment(lib,"LLGI.lib")
#endif

static const int MaxEffectDisplay = 500;		// 一度に画面上に存在できるエフェクト数
static const int MaxParticleDisplay = 65536;	// 一度に画面上に存在できるパーティクル数


namespace Effect
{
	EfkMgr* EfkMgr::singleton = nullptr;

	EfkMgr::EfkMgr() :
		efkRenderer(nullptr),
		efkMgr(nullptr),
		efkMemPool(nullptr),
		efkCmdList(nullptr),
		effect(),
		loopPlayData()
	{
		singleton = nullptr;
	}

	EfkMgr::~EfkMgr()
	{
	}

	void EfkMgr::OnInit(ID3D12Device* _Dev, ID3D12CommandQueue* _Que)
	{
		if (!singleton)
		{
			singleton = new EfkMgr;
			singleton->device = _Dev;
			singleton->cmdQue = _Que;
			Init();
		}
	}

	void EfkMgr::OnTerm()
	{
		if (singleton)
		{
			delete singleton;
			singleton = nullptr;
		}
	}

	void EfkMgr::Init()
	{
		DXGI_FORMAT bbFormats[] = { DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM };
		singleton->efkRenderer = EffekseerRendererDX12::Create(
			singleton->device,		// デバイス
			singleton->cmdQue,		// コマンドキュー
			2,						// バックバッファーの数
			bbFormats,				// レンダーターゲットフォーマット
			1,						// レンダーターゲット数
			DXGI_FORMAT_D32_FLOAT,	// デプスはなし
			false,					// 反対デプスはなし
			MaxParticleDisplay		// 最大パーティクルの数
		);

		singleton->efkMgr = Effekseer::Manager::Create(MaxEffectDisplay);  // 最大インスタンス数
		// エフェクシア設定
		SetUpEfkMgr();
		// DX12用の初期化
		InitDX12Renderer();


		auto data = System::Tool::LoadJson("Resource/JsonData/EfkData.json");

		// エフェクト読み込み
		typedef EnumIterator < Res::EfkType, Res::EfkType::Begin, Res::EfkType::End > typeIte;

		for (auto ite = typeIte(); ite != typeIte(Res::EfkType::End); ++ite)
		{
			const std::string tempId = std::to_string(ite.GetId());
			const std::wstring path = Sys::Helper::ToWstr(data[tempId.c_str()].GetString());

			for (auto ite2 = typeIte(); ite2 != typeIte(Res::EfkType::End); ++ite2)
			{
				const std::string tempId2 = std::to_string(ite2.GetId());
				const std::wstring path2 = Sys::Helper::ToWstr(data[tempId2.c_str()].GetString());

				if (path == path2 && ite != ite2)
				{
					// 2重ロード
					ELOG("Error : Mesh Double Reading");
				}
			}
			LoadEffect(*ite, path.c_str());
		}
	}

	int EfkMgr::PlayEffect(const Res::EfkType _Type, const DirectX::XMFLOAT3& _Pos, bool _IsLoop, const DirectX::XMFLOAT3 _Angle)
	{
		if (_IsLoop)
		{
			LoopPlayData data;
			data.type = _Type;
			data.handle = singleton->efkMgr->Play(singleton->effect[_Type], _Pos.x, _Pos.y, _Pos.z);
			singleton->loopPlayData.emplace_back(data);
			// 角度の設定
			singleton->efkMgr->SetRotation(data.handle, _Angle.x, _Angle.y, _Angle.z);

			//  速度をデルタタイムに合わせる
			singleton->efkMgr->SetSpeed(data.handle, Sys::Timer::GetDeltaTime() * 100);

			return data.handle;
		}
		else
		{
			int temp = singleton->efkMgr->Play(singleton->effect[_Type], _Pos.x, _Pos.y, _Pos.z);
			// 角度の変更
			singleton->efkMgr->SetRotation(temp, _Angle.x, _Angle.y, _Angle.z);

			//  速度をデルタタイムに合わせる
			singleton->efkMgr->SetSpeed(temp, Sys::Timer::GetHitStopTime() * 100);

			return temp;
		}
	}

	void EfkMgr::StopEffect(int _Handle)
	{
		// 指定されたエフェクトを停止
		singleton->efkMgr->StopEffect(_Handle);
		// ループ再生中のエフェクトデータから削除
		for (auto itr = singleton->loopPlayData.begin(); itr != singleton->loopPlayData.end(); ++itr)
		{
			if (itr->handle == _Handle)
			{
				singleton->loopPlayData.erase(itr);
				return;
			}
		}
	}

	void EfkMgr::Draw(ID3D12GraphicsCommandList* _GCList)
	{
		// エフェクト描画
		singleton->efkMgr->Update();		// マネージャの更新(更新時間)
		singleton->efkMemPool->NewFrame();	// 描画するレンダーターゲットを選択
		// 描画処理の開始
		EffekseerRendererDX12::BeginCommandList(singleton->efkCmdList, _GCList);
		// 描画前処理
		singleton->efkRenderer->BeginRendering();
		// エフェクト描画
		singleton->efkMgr->Draw();
		// 描画後処理
		singleton->efkRenderer->EndRendering();
		// 描画処理終了
		EffekseerRendererDX12::EndCommandList(singleton->efkCmdList);
	}

	int EfkMgr::UpdateLoopPlayEffect(const DirectX::XMFLOAT3& _Pos, const int _Handle, const float _DeltaTime, const DirectX::XMFLOAT3& _Angle)
	{
		//  ハンドルによって指定されたエフェクトがループ再生中か検索
		for (auto itr = singleton->loopPlayData.begin(); itr != singleton->loopPlayData.end(); ++itr)
		{
			//  ループ再生中且つエフェクトの再生が終わっていたら、最初から再生しなおす。
			if (itr->handle == _Handle && !singleton->efkMgr->Exists(_Handle))
			{
				//  再生
				itr->handle = singleton->efkMgr->Play(singleton->effect[itr->type], _Pos.x, _Pos.y, _Pos.z);
				//  角度の変更
				singleton->efkMgr->SetRotation(itr->handle, _Angle.x, _Angle.y, _Angle.z);

				return itr->handle;
			}
		}
		//  位置の変更
		singleton->efkMgr->SetLocation(_Handle, _Pos.x, _Pos.y, _Pos.z);
		//  角度の変更
		singleton->efkMgr->SetRotation(_Handle, _Angle.x, _Angle.y, _Angle.z);
		//  速度をデルタタイムに合わせる
		singleton->efkMgr->SetSpeed(_Handle, _DeltaTime);
		return _Handle;
	}

	void EfkMgr::SetPlaySpeed(const int _Handle, const float _PlaySpeed)
	{
		singleton->efkMgr->SetSpeed(_Handle, _PlaySpeed);
	}

	void EfkMgr::SyncronizeEffekseerCamera(DirectX::XMMATRIX _ViewMat, DirectX::XMMATRIX _ProjMat)
	{
		Effekseer::Matrix44 fkViewMat; // Effekseer用ビュー行列
		Effekseer::Matrix44 fkProjMat; // Effekseer用プロジェクション行列

		//  Effekseer用とデバイス上のそれぞれの行列を同期
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				fkViewMat.Values[i][j] = _ViewMat.r[i].m128_f32[j];
				fkProjMat.Values[i][j] = _ProjMat.r[i].m128_f32[j];
			}
		}

		// それぞれの行列に従ってカメラを設定
		singleton->efkRenderer->SetCameraMatrix(fkViewMat);
		singleton->efkRenderer->SetProjectionMatrix(fkProjMat);
	}

	void EfkMgr::SetUpEfkMgr()
	{
		// 座標系を左手系に(クライアント側に合わせる)
		singleton->efkMgr->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
		// 描画用インスタンスから描画機能を設定
		singleton->efkMgr->SetSpriteRenderer(singleton->efkRenderer->CreateSpriteRenderer());
		singleton->efkMgr->SetRibbonRenderer(singleton->efkRenderer->CreateRibbonRenderer());
		singleton->efkMgr->SetRingRenderer(singleton->efkRenderer->CreateRingRenderer());
		singleton->efkMgr->SetTrackRenderer(singleton->efkRenderer->CreateTrackRenderer());
		singleton->efkMgr->SetModelRenderer(singleton->efkRenderer->CreateModelRenderer());
		// 描画用インスタンスからテクスチャの読み込み機能を設定(独自拡張も可能)
		singleton->efkMgr->SetTextureLoader(singleton->efkRenderer->CreateTextureLoader());
		singleton->efkMgr->SetModelLoader(singleton->efkRenderer->CreateModelLoader());
		singleton->efkMgr->SetMaterialLoader(singleton->efkRenderer->CreateMaterialLoader());
	}

	void EfkMgr::InitDX12Renderer()
	{
		// DX12特有の処理
		singleton->efkMemPool = EffekseerRenderer::CreateSingleFrameMemoryPool(singleton->efkRenderer->GetGraphicsDevice());
		singleton->efkCmdList = EffekseerRenderer::CreateCommandList(singleton->efkRenderer->GetGraphicsDevice(), singleton->efkMemPool);
		singleton->efkRenderer->SetCommandList(singleton->efkCmdList);
	}

	void EfkMgr::LoadEffect(const Res::EfkType _Type, const wchar_t* _Path)
	{
		const float Magnification = 1.0f;

		singleton->effect[_Type] = Effekseer::Effect::Create(
			singleton->efkMgr,
			(const EFK_CHAR*)_Path,
			Magnification,
			(const EFK_CHAR*)L"Resource/Effekseer/Texture"
		);

		// エラーチェック
		if (singleton->effect[_Type] == nullptr)
		{
			ELOG("Error : Missing Efk Load");
		}
	}
}