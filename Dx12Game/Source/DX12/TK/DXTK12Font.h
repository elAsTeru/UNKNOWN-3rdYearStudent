#pragma once
// -----------------------------------------------------------------
// DXTK12を利用した画面にフォントを表示するためのシングルトンクラス
// -----------------------------------------------------------------
#include <d3d12.h>
#include <DirectXTK12/SpriteFont.h>
#include <DirectXTK12/ResourceUploadBatch.h>
#include "ComPtr.h"

class Fence;

namespace MyDX
{
	struct FontData
	{
		std::wstring			str{};		// 表示する文字列
		DirectX::XMFLOAT2		pos{};		// 座標
		DirectX::XMVECTOR		color{};	// 色
		float					rotation{};	// 回転
		DirectX::XMFLOAT2		origin{};	// 
		float					scale = 1;	// サイズ
	};

	class DXTK12Font
	{
		DXTK12Font();
		~DXTK12Font();
		static DXTK12Font* singleton;

	public:
		static void OnInit(ID3D12Device* _Device, ID3D12CommandQueue* _CmdQue, Fence& _Fence, D3D12_VIEWPORT& _Viewport);	// 使用開始
		static void OnTerm();																								// 使用終了

		// Public Method
		static void PrintFont(ID3D12GraphicsCommandList* _CmdList, std::wstring _Text, DirectX::XMFLOAT2 _Pos, DirectX::XMVECTOR _Color, float _Rot, DirectX::XMFLOAT2 _Origin, float _Sca);
		static void Commit(ID3D12CommandQueue* _CmdQue);	// 文字のメモリリークを防止する

		/// <summary> 描画文字データを積む </summary>
		static void DrawFont(const FontData _FontData);
		/// <summary> DrawFontで積んだ文字を描画する(※最後に描画を行う必要がある) </summary>
		static void RenderingFonts(ID3D12GraphicsCommandList* _GCList);

	private:
		// Private Variable
		DirectX::GraphicsMemory*		memory;				// グラフィックスメモリオブジェクト
		DirectX::SpriteFont*			spriteFont;			// フォント表示用オブジェクト
		DirectX::SpriteBatch*			spriteBatch;		// スプライト表示用オブジェクト
		ComPtr<ID3D12DescriptorHeap>	heapForSpriteFont;
		ID3D12Device*					device;
		std::vector<FontData>			drawFontData;

		// Private Method
		static ComPtr<ID3D12DescriptorHeap>	CreateDescHeapForSpriteFont();
	};
}

// このクラスですること
//	動的配列を作成して、文字列と座標をスタックしていき
//	描画を行うとまとめて表示する