#include "EffekseerMgr.h"
#include "Dx12Wrapper.h"
#include "EfkData.h"
#include "Logger.h"

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
	EffekseerMgr* EffekseerMgr::singleton = nullptr;

	EffekseerMgr::EffekseerMgr() :
		efkRenderer(nullptr),
		efkMgr(nullptr),
		efkMemPool(nullptr),
		efkCmdList(nullptr),
		effect(),
		loopPlayData()
	{
		singleton = nullptr;
	}

	EffekseerMgr::~EffekseerMgr()
	{
	}

	void EffekseerMgr::OnInit()
	{
		if (!singleton)
		{
			singleton = new EffekseerMgr;
			Init();
		}
	}

	void EffekseerMgr::OnTerm()
	{
		if (singleton)
		{
			delete singleton;
			singleton = nullptr;
		}
	}

	void EffekseerMgr::Init()
	{
		DXGI_FORMAT bbFormats[] = { DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM };
		singleton->efkRenderer = EffekseerRendererDX12::Create(
			MyDX::Dx12Wrapper::GetDevice(),		// DirectX12のデバイス
			MyDX::Dx12Wrapper::GetQue(),        // DirectX12のコマンドキュー
			2,                                  // バックバッファーの数
			bbFormats,                          // レンダーターゲットフォーマット
			1,                                  // レンダーターゲット数
			DXGI_FORMAT_D32_FLOAT,				// デプスはなし
			false,                              // 反対デプスはなし
			MaxParticleDisplay);              // 最大パーティクルの数

		singleton->efkMgr = Effekseer::Manager::Create(MaxEffectDisplay);  // 最大インスタンス数
		// エフェクシア設定
		SetUpEfkMgr();
		// DX12用の初期化
		InitDX12Renderer();
		// エフェクト読み込み
					// 全てのメッシュデータをロード
		for (auto type = MyRes::EfkType::Begin; type != MyRes::EfkType::End; ++type)
		{
			// ?同じものを読み込んでいないか比較する
			for (auto comparison = MyRes::EfkType::Begin; comparison != MyRes::EfkType::End; ++comparison)
			{
				if ((type != comparison) && (MyRes::EfkData::pathMap[type] == MyRes::EfkData::pathMap[comparison]))
				{
					ELOG("Error : Efk File Double Load");
				}
			}
			LoadEffect(type);
		}
	}

	int EffekseerMgr::PlayEffect(const MyRes::EfkType _Type, const DirectX::XMFLOAT3& _Pos, bool _IsLoop, const DirectX::XMFLOAT3 _Angle)
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
			singleton->efkMgr->SetSpeed(data.handle, MySys::Timer::GetDeltaTime() * 100);

			return data.handle;
		}
		else
		{
			int temp = singleton->efkMgr->Play(singleton->effect[_Type], _Pos.x, _Pos.y, _Pos.z);
			// 角度の変更
			singleton->efkMgr->SetRotation(temp, _Angle.x, _Angle.y, _Angle.z);

			//  速度をデルタタイムに合わせる
			singleton->efkMgr->SetSpeed(temp, MySys::Timer::GetHitStopTime() * 100);

			return temp;
		}
	}

	void EffekseerMgr::StopEffect(int _Handle)
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

	void EffekseerMgr::Draw()
	{
		// エフェクト描画
		singleton->efkMgr->Update();		// マネージャの更新(更新時間)
		singleton->efkMemPool->NewFrame();	// 描画するレンダーターゲットを選択
		// 描画処理の開始
		EffekseerRendererDX12::BeginCommandList(singleton->efkCmdList, MyDX::Dx12Wrapper::GetCmdList());
		// 描画前処理
		singleton->efkRenderer->BeginRendering();
		// エフェクト描画
		singleton->efkMgr->Draw();
		// 描画後処理
		singleton->efkRenderer->EndRendering();
		// 描画処理終了
		EffekseerRendererDX12::EndCommandList(singleton->efkCmdList);
	}

	int EffekseerMgr::UpdateLoopPlayEffect(const DirectX::XMFLOAT3& _Pos, const int _Handle, const float _DeltaTime, const DirectX::XMFLOAT3& _Angle)
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

	void EffekseerMgr::SetPlaySpeed(const int _Handle, const float _PlaySpeed)
	{
		singleton->efkMgr->SetSpeed(_Handle, _PlaySpeed);
	}

	void EffekseerMgr::SyncronizeEffekseerCamera()
	{
		// Effekseer用ビュー行列
		Effekseer::Matrix44 fkViewMat;
		// Effekseer用プロジェクション行列
		Effekseer::Matrix44 fkProjMat;

		// デバイスのビュー行列とプロジェクション行列をコピー
		DirectX::XMMATRIX view = MyDX::Dx12Wrapper::GetViewMatrix();
		DirectX::XMMATRIX proj = MyDX::Dx12Wrapper::GetProjMatrix();

		//  Effekseer用とデバイス上のそれぞれの行列を同期
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				fkViewMat.Values[i][j] = view.r[i].m128_f32[j];
				fkProjMat.Values[i][j] = proj.r[i].m128_f32[j];
			}
		}

		// それぞれの行列に従ってカメラを設定
		singleton->efkRenderer->SetCameraMatrix(fkViewMat);
		singleton->efkRenderer->SetProjectionMatrix(fkProjMat);
	}

	void EffekseerMgr::SetUpEfkMgr()
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

	void EffekseerMgr::InitDX12Renderer()
	{
		// DX12特有の処理
		singleton->efkMemPool = EffekseerRenderer::CreateSingleFrameMemoryPool(singleton->efkRenderer->GetGraphicsDevice());
		singleton->efkCmdList = EffekseerRenderer::CreateCommandList(singleton->efkRenderer->GetGraphicsDevice(), singleton->efkMemPool);
		singleton->efkRenderer->SetCommandList(singleton->efkCmdList);
	}

	void EffekseerMgr::LoadEffect(const MyRes::EfkType _Type)
	{
		const float Magnification = 1.0f;

		singleton->effect[_Type] = Effekseer::Effect::Create(
			singleton->efkMgr,
			(const EFK_CHAR*)MyRes::EfkData::pathMap[_Type],
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