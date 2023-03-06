#include "DXTK12Font.h"
#include "Fence.h"
#include "FileUtil.h"
#include "Logger.h"

#pragma comment(lib,"DirectXTK12.lib")

namespace MyDX
{
	DXTK12Font* DXTK12Font::singleton = nullptr;

	DXTK12Font::DXTK12Font()
	{
		singleton = nullptr;
	}
	DXTK12Font::~DXTK12Font()
	{
		delete memory;
		memory = nullptr;
		delete spriteFont;
		spriteFont = nullptr;
		delete spriteBatch;
		spriteBatch = nullptr;
	}

	void DXTK12Font::Commit(ID3D12CommandQueue* _CmdQue)
	{
		singleton->memory->Commit(_CmdQue); 
	}

	void DXTK12Font::DrawFont(const FontData _FontData)
	{
		singleton->drawFontData.emplace_back(_FontData);
	}

	void DXTK12Font::RenderingFonts(ID3D12GraphicsCommandList* _GCList)
	{
		_GCList->SetDescriptorHeaps(1, singleton->heapForSpriteFont.GetAddressOf());
		singleton->spriteBatch->Begin(_GCList);
		for (auto font : singleton->drawFontData)
		{
			singleton->spriteFont->DrawString(singleton->spriteBatch, font.str.c_str(), font.pos, font.color, font.rotation, font.origin, font.scale);
		}
		singleton->spriteBatch->End();
		singleton->drawFontData.clear();
	}

	void DXTK12Font::OnInit(ID3D12Device* _Device, ID3D12CommandQueue* _CmdQue, Fence& _Fence, D3D12_VIEWPORT& _Viewport)
	{
		if (!singleton)
		{
			singleton = new DXTK12Font;
			singleton->device = _Device;
			// Graphics Memory オブジェクトの初期化
			singleton->memory = new DirectX::GraphicsMemory(_Device);
			// Sprite Batch Memory オブジェクトの初期化
			DirectX::ResourceUploadBatch resUploadBatch(_Device);
			resUploadBatch.Begin();
			DirectX::RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
			DirectX::SpriteBatchPipelineStateDescription pd(rtState);
			singleton->spriteBatch = new DirectX::SpriteBatch(_Device, resUploadBatch, pd);

			std::wstring filePath = L"Resource/Font/Meiryo_72.spritefont";

			// Sprite Font オブジェクトの初期化
			singleton->heapForSpriteFont = CreateDescHeapForSpriteFont();
			singleton->spriteFont = new DirectX::SpriteFont
			(
				_Device,
				resUploadBatch,
				filePath.c_str(),
				singleton->heapForSpriteFont->GetCPUDescriptorHandleForHeapStart(),
				singleton->heapForSpriteFont->GetGPUDescriptorHandleForHeapStart()
			);

			auto future = resUploadBatch.End(_CmdQue);
			_Fence.Wait(_CmdQue, 1000);
			future.wait();
			singleton->spriteBatch->SetViewport(_Viewport);
		}
	}
	void DXTK12Font::OnTerm()
	{
		if (singleton)
		{
			delete singleton;
			singleton = nullptr;
		}
	}
	ComPtr<ID3D12DescriptorHeap> DXTK12Font::CreateDescHeapForSpriteFont()
	{
		ComPtr<ID3D12DescriptorHeap> ret;
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		singleton->device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));
		return ret;
	}
}