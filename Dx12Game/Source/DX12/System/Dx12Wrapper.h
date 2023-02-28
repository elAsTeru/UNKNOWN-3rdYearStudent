#pragma once

#include <d3d12.h>
#include <array>
#include <unordered_map>
#include <DirectXTK12/SimpleMath.h>

#include "ComPtr.h"
#include "DescriptorPool.h"
#include "ColorTarget.h"
#include "DepthTarget.h"
#include "CommandList.h"
#include "Fence.h"
#include "ConstantBuffer.h"
#include "MaterialType.h"
#include "RootSignature.h"
#include "MeshType.h"

#include "Resource/Mesh.h"
#include "Resource/Texture.h"
#include "Resource/Material.h"

using namespace DirectX::SimpleMath;

namespace MyDX
{
	// D3D12Wrapper class
	class Dx12Wrapper
	{
		Dx12Wrapper();
		~Dx12Wrapper();
		static Dx12Wrapper* singleton;

		//// CBV(定数バッファビュー)
		// 定数バッファをシェーダに送るために使用する
		//// SRV
		// テクスチャとして使う場合
		//// UAV(案オーダードアクセスビュー)
		// コンピュートシェーダなどで、データの読み/書きを行うバッファとして使用する場合
		//// Sampler
		// テクスチャサンプリングに使用する
		//// RTV
		// レンダリング前の一時的な中間バッファにレンダリングできるようにする
		//// DSV(デプスステンシルビュー)
		// 深度とステンシルの情報を保持するための形式とバッファ
		enum POOL_TYPE
		{
			POOL_TYPE_RES = 0,     // CBV / SRV / UAV
			POOL_TYPE_SMP = 1,     // Sampler
			POOL_TYPE_RTV = 2,     // RTV
			POOL_TYPE_DSV = 3,     // DSV
			POOL_COUNT = 4,
		};

	public:
		struct ObjectData
		{
			DirectX::XMMATRIX matrix;
			Res::MeshType mesh;
			Res::MaterialType matType;
			float alpha = 1;
		};

		struct LineData
		{
			std::vector<DirectX::XMFLOAT3> vertices;	// 線の頂点データ
			VertexBuffer lineVB;						// 専用頂点バッファ
		};

		struct RectData
		{
			DirectX::XMMATRIX	matrix;					// 座標系
			DirectX::XMFLOAT4	color;					// 色
		};

		static bool OnInit(HWND _hWnd, uint32_t _width, uint32_t _height);
		static void OnTerm();
		static void RenderBefore();
		static void RenderBegin();
		static void Render();
		static void RenderEnd();
		static Mesh* InitMesh(std::vector<ResMesh>& _ResMesh, Mesh* _MeshData);

		// 仮置き
		static void SetPlayerPos(const DirectX::XMFLOAT3& _Pos) { singleton->playerPos = _Pos; }

		/// <summary>
		/// メッシュを描画する
		/// </summary>
		/// <param name="_Data">座標系 / メッシュ番号 / マテリアル番号</param>
		static void DrawBasicMesh(const ObjectData& _MeshData);
		/// <summary>
		/// 線による四角を描画する
		/// </summary>
		/// <param name="_RectData">座標系 / マテリアル番号</param>
		static void DrawRect(const DirectX::XMMATRIX& _Matrix, const DirectX::XMFLOAT4& _Color);
		static void Draw2DUI(const ObjectData& _2DUIData);

		static ID3D12Resource* MakeTexture(int _Color);

		// カメラ設定
		static void SetCameraPos(const Vector3& _EyePos) { singleton->eyePos = _EyePos; }
		static void SetCameraTargetPos(const Vector3& _TargetPos) { singleton->targetPos = _TargetPos; }
		static void SetCameraUpward(const Vector3& _Upward) { singleton->upward = _Upward; }

		static const Vector3& GetCameraPos() { return singleton->eyePos; }

		static ID3D12Device* GetDevice() { return singleton->device.Get(); }
		static ID3D12CommandQueue* GetQue() { return singleton->queue.Get(); }
		static ID3D12GraphicsCommandList* GetCmdList() { return singleton->graphicsCmdList; }
		static DirectX::XMMATRIX GetViewMatrix() { return singleton->view; }
		static DirectX::XMMATRIX GetProjMatrix() { return singleton->proj; }
	private:
		//// 描画データ
		std::vector<ObjectData> drawBasicMeshData;	// 標準描画メッシュデータ
		std::vector<ObjectData> draw2DUIData;		// 2D表示のUIデータ
		std::vector<RectData> drawRectData;
		
		DirectX::XMFLOAT3 playerPos{};
		LineData lineData[4];						// 線データ

		// カメラ設定用
		Vector3 eyePos;
		Vector3 targetPos;
		Vector3 upward;

		enum class DrawType : unsigned char
		{
			Begin = 0,

			Basic = Begin,	// 通常ポリ
			Line,			// 通常ライン用
			Ui,				// UI用

			End
		};

		//Private Variable
		static const uint32_t FrameCount = 2;	// フレームバッファ数

		HWND		hWnd;		// ウィンドウハンドル
		uint32_t	width;		// ウィンドウ幅
		uint32_t	height;		// ウィンドウ高

		ComPtr<ID3D12Device>		device;								//デバイス
		ComPtr<ID3D12CommandQueue>	queue;								//コマンドキュー
		ComPtr<IDXGISwapChain3>		swapChain;							//スワップチェイン
		ColorTarget					colorTarget[FrameCount];			//カラーターゲット
		DepthTarget					depthTarget;						//深度ターゲット
		DescriptorPool*				pool[POOL_COUNT];					//ディスクリプタプール
		CommandList					cmdList;							//コマンドリスト
		ID3D12GraphicsCommandList*	graphicsCmdList;					// グラフィックスコマンドリスト
		Fence						fence;								//フェンス
		uint32_t					frameIndex;							//フレーム番号
		D3D12_VIEWPORT				viewport;							//ビューポート
		D3D12_RECT					scissor;							//シザー矩形

		Matrix						view;
		Matrix						proj;

		// 旧定数バッファ←現在
		std::unordered_map<DrawType, std::vector<ConstantBuffer*>>transform;	// 変換行列
		// 新定数バッファ
		//ConstantBuffer			meshCB[FrameCount * MAX_MESH_CB];	// メッシュ用バッファ
		//ConstantBuffer			transformCB[MAX_MESH_CB];			// 変換行列用バッファ

		std::unordered_map<Res::MaterialType, Material> materials;		// マテリアル

		std::unordered_map<DrawType, ComPtr<ID3D12PipelineState>> pso;	// パイプラインステート
		
		ComPtr<ID3D12RootSignature>	BasicRS;	//ルートシグネチャ
		ComPtr<ID3D12RootSignature> Basic2DRS;	// 2D用ルートシグネチャ

		//// MultiPass
		// ペラポリ用
		std::array<ComPtr<ID3D12Resource>,2>	peraReses;			// ペラポリゴンに張り付けるためのリソース
		ComPtr<ID3D12DescriptorHeap>			peraRTVHeap;
		ComPtr<ID3D12DescriptorHeap>			peraSRVHeap;
		ComPtr<ID3D12RootSignature>				peraRS;				// ペラ用ルートシグネチャ
		ComPtr<ID3D12PipelineState>				peraPSO1;			// ペラ用パイプラインステート
		ComPtr<ID3D12PipelineState>				peraPSO2;
		void CreatePeraResource();
		void PreDrawToPera1();
		void PostDrawToPera1();
		void CreatePeraPipeline();
		ComPtr<ID3D12Resource> peraVB;
		D3D12_VERTEX_BUFFER_VIEW peraVBV;
		void CreatePeraVertex();

		// ぼかし
		ComPtr<ID3D12Resource> bokehParamResource;
		void CreateBokeParamResourcec();
		void DrawHorizontalBokeh();

		// ブルーム
		std::array<ComPtr<ID3D12Resource>, 2> bloomBuff;
		ComPtr<ID3D12PipelineState> blurPSO;
		void CreateBloomBuffer();
		void DrawShrinkTextureForBlur();

		// Private Method
		static bool InitD3D();
		static bool InitRender();
		static void TermD3D();
		static void Term();
		static void Present(uint32_t interval);
		void LoadShader(LPCWSTR _Path, ComPtr<ID3DBlob>& _Blob);
		void Barrier(ID3D12Resource* p, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

		// 新、記入（整理する際に必要なものだけ残していく）
	public:
		static void SetDirLightDir(const Vector3 _Dir);
	private:
		// Private Variable
		ConstantBuffer* dirLight;	// ディレクショナルライト(向きだけ所有)
	};
}