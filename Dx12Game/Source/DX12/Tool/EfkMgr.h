#pragma once

#include <Effekseer/Effekseer.h>
#include <Effekseer/EffekseerRendererDX12.h>
#include <unordered_map>
#include <list>
#include <DirectXMath.h>
#include "Resource/EfkType.h"

namespace Effect
{
	class EfkMgr
	{
		EfkMgr();
		~EfkMgr();
		static EfkMgr* singleton;
	public:
		// Public Method
		static void OnInit(ID3D12Device* _Dev, ID3D12CommandQueue* _Que);
		static void OnTerm();

		static void Init();
		static int PlayEffect(const Res::EfkType _Type, const DirectX::XMFLOAT3& _Pos, bool _IsLoop, const DirectX::XMFLOAT3 _Angle = { 0.0f,0.0f,0.0f });
		static void StopEffect(int _Handle);
		static void Draw(ID3D12GraphicsCommandList* _GCList);
		static int UpdateLoopPlayEffect(const DirectX::XMFLOAT3& _Pos, const int _Handle, const float _DeltaTime, const DirectX::XMFLOAT3& _Angle = { 0.0f,0.0f,0.0f });
		static void SetPlaySpeed(const int _Handle, const float _PlaySpeed);
		/// <summary> カメラとエフェクトの位置を同期 </summary>
		static void SyncronizeEffekseerCamera(DirectX::XMMATRIX _ViewMat, DirectX::XMMATRIX _ProjMat);

	private:
		// Private Struct
		struct LoopPlayData
		{
			Effekseer::Handle handle;
			Res::EfkType type;
		};

		// Private Variable
		ID3D12Device* device;
		ID3D12CommandQueue* cmdQue;
		// 必要項目の基本
		Effekseer::RefPtr <EffekseerRenderer::Renderer>					efkRenderer;	// レンダラ
		Effekseer::RefPtr <Effekseer::Manager>							efkMgr;			// マネジャ
		// DX12・Vulkan・metalなどのコマンドリスト系への対応に必須
		Effekseer::RefPtr <EffekseerRenderer::SingleFrameMemoryPool>	efkMemPool;		// メモリプール
		Effekseer::RefPtr <EffekseerRenderer::CommandList>				efkCmdList;		// コマンドリスト
		// エフェクト再生に必要
		std::unordered_map<Res::EfkType, Effekseer::RefPtr <Effekseer::Effect>> effect;// エフェクト本体
		std::list<LoopPlayData> loopPlayData;


		// Private Method
		static void SetUpEfkMgr();
		static void InitDX12Renderer();
		static void LoadEffect(const Res::EfkType _Type, const wchar_t* _Path);
	};
}