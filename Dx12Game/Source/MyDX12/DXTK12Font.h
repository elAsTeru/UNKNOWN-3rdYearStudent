// ----------------------------------------------------------------------
// DXTK12を利用した画面にフォントを表示するためのシングルトンクラス
// ----------------------------------------------------------------------
#pragma once

#include <DirectXTK12/SpriteFont.h>
#include <DirectXTK12/ResourceUploadBatch.h>
#include "ComPtr.h"
#include "Fence.h"
#include "CommandList.h"

#pragma comment(lib,"DirectXTK12.lib")

namespace MyDX::Render
{
	class DXTK12Font
	{
		DXTK12Font();
		~DXTK12Font();
		static DXTK12Font* singleton;

	public:
		static void OnInit(ID3D12Device* _Device, ID3D12CommandQueue* _CmdQue, Fence& _Fence, D3D12_VIEWPORT& _Viewport);	// 使用開始
		static void OnTerm();																								// 使用終了

		// Public Method
		static ComPtr<ID3D12DescriptorHeap> GetDescHeap() { return singleton->heapForSpriteFont; }
		static void PrintFont(ID3D12GraphicsCommandList* _CmdList, std::wstring _Text, DirectX::XMFLOAT2 _Pos, DirectX::XMVECTOR _Color, float _Rot, DirectX::XMFLOAT2 _Origin, float _Sca);
		static void Commit(ID3D12CommandQueue* _CmdQue);	// 文字のメモリリークを防止する

	private:
		// Private Variable
		DirectX::GraphicsMemory*		memory;				// グラフィックスメモリオブジェクト
		DirectX::SpriteFont*			spriteFont;			// フォント表示用オブジェクト
		DirectX::SpriteBatch*			spriteBatch;		// スプライト表示用オブジェクト
		ComPtr<ID3D12DescriptorHeap>	heapForSpriteFont;
		ID3D12Device*					device;

		// Private Method
		static ComPtr<ID3D12DescriptorHeap>	CreateDescHeapForSpriteFont();
	};
}

// このクラスですること
//	動的配列を作成して、文字列と座標をスタックしていき
//	描画を行うとまとめて表示する