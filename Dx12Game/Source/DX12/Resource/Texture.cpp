#include "Texture.h"
#include <DirectXTK12/DDSTextureLoader.h>
#include "DescriptorPool.h"
#include "Logger.h"

Texture::Texture():
	texture(nullptr),
	descHandle(nullptr),
	descPool(nullptr)
{
}

Texture::~Texture() { Term(); }

bool Texture::Init(ID3D12Device* pDevice, DescriptorPool* pPool, const wchar_t* filename, DirectX::ResourceUploadBatch& batch)
{
    // 引数チェック
    if (pDevice == nullptr || pPool == nullptr || filename == nullptr)
    {
        ELOG("Error : Invalid Argument");
        return false;
    }

    assert(descPool == nullptr);
    assert(descHandle == nullptr);

    // ディスクリプタプールを設定.
    descPool = pPool;
    descPool->AddRef();

    // ディスクリプタハンドルを取得.
    descHandle = pPool->AllocHandle();
    if (descHandle == nullptr)
    {
        return false;
    }

    // ファイルからテクスチャを生成.
    bool isCube = false;
    auto hr = DirectX::CreateDDSTextureFromFile(
        pDevice,
        batch,
        filename,
        texture.GetAddressOf(),
        true,
        0,
        nullptr,
        &isCube);
    if (FAILED(hr))
    {
        ELOG("Error : DirectX::CreateDDSTextureFromFile() Failed. filename = %ls, retcode = 0x%x", filename, hr);
        return false;
    }

    // シェーダリソースビューの設定を求める.
    auto viewDesc = GetViewDesc(isCube);

    // シェーダリソースビューを生成します.
    pDevice->CreateShaderResourceView(texture.Get(), &viewDesc, descHandle->HandleCPU);

    // 正常終了.
    return true;
}

// キューブマップ
bool Texture::Init(ID3D12Device* pDevice, DescriptorPool* pPool, const D3D12_RESOURCE_DESC* pDesc, bool isCube)
{
    if (pDevice == nullptr || pPool == nullptr || pDesc == nullptr)
    {
        return false;
    }

    assert(descPool == nullptr);
    assert(descHandle == nullptr);

    // ディスクリプタプールを設定.
    descPool = pPool;
    descPool->AddRef();

    // ディスクリプタハンドルを取得.
    descHandle = pPool->AllocHandle();
    if (descHandle == nullptr)
    {
        return false;
    }

    D3D12_HEAP_PROPERTIES prop{};
    prop.Type = D3D12_HEAP_TYPE_DEFAULT;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.CreationNodeMask = 0;
    prop.VisibleNodeMask = 0;

    auto hr = pDevice->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        pDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(texture.GetAddressOf())
    );
    if (FAILED(hr))
    {
        ELOG("Error : ID3D12Device::CreateCommittedResource() Failed. retcode = 0x%x", hr);
        return false;
    }

    // シェーダリソースビューの設定を求める.
    auto viewDesc = GetViewDesc(isCube);

    // シェーダリソースビューを生成します.
    pDevice->CreateShaderResourceView(texture.Get(), &viewDesc, descHandle->HandleCPU);

    return true;
}

void Texture::Term()
{
    texture.Reset();

    // ディスクリプタハンドルを解放.
    if (descHandle != nullptr && descPool != nullptr)
    {
        descPool->FreeHandle(descHandle);
        descHandle = nullptr;
    }

    // ディスクリプタプールを解放.
    if (descPool != nullptr)
    {
        descPool->Release();
        descPool = nullptr;
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetHandleCPU() const
{
    if (descHandle != nullptr)
    {
        return descHandle->HandleCPU;
    }

    return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetHandleGPU() const
{
    if (descHandle != nullptr)
    {
        return descHandle->HandleGPU;
    }

    return D3D12_GPU_DESCRIPTOR_HANDLE();
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture::GetViewDesc(bool isCube)
{
    auto desc = texture->GetDesc();
    D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc{};

    viewDesc.Format = desc.Format;
    viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    switch (desc.Dimension)
    {
    case D3D12_RESOURCE_DIMENSION_BUFFER:
    {
        // バッファは対象外とします.
        abort();    // アプリを止める.
    }
    break;

    case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
    {
        if (desc.DepthOrArraySize > 1)
        {
            viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;

            viewDesc.Texture1DArray.MostDetailedMip = 0;
            viewDesc.Texture1DArray.MipLevels = desc.MipLevels;
            viewDesc.Texture1DArray.FirstArraySlice = 0;
            viewDesc.Texture1DArray.ArraySize = desc.DepthOrArraySize;
            viewDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
        }
        else
        {
            viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;

            viewDesc.Texture1D.MostDetailedMip = 0;
            viewDesc.Texture1D.MipLevels = desc.MipLevels;
            viewDesc.Texture1D.ResourceMinLODClamp = 0.0f;
        }
    }
    break;

    case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
    {
        if (isCube)
        {
            if (desc.DepthOrArraySize > 6)
            {
                viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;

                viewDesc.TextureCubeArray.MostDetailedMip = 0;
                viewDesc.TextureCubeArray.MipLevels = desc.MipLevels;
                viewDesc.TextureCubeArray.First2DArrayFace = 0;
                viewDesc.TextureCubeArray.NumCubes = (desc.DepthOrArraySize / 6);
                viewDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
            }
            else
            {
                viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;

                viewDesc.TextureCube.MostDetailedMip = 0;
                viewDesc.TextureCube.MipLevels = desc.MipLevels;
                viewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
            }
        }
        else
        {
            if (desc.DepthOrArraySize > 1)
            {
                if (desc.MipLevels > 1)
                {
                    viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;

                    viewDesc.Texture2DMSArray.FirstArraySlice = 0;
                    viewDesc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
                }
                else
                {
                    viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

                    viewDesc.Texture2DArray.MostDetailedMip = 0;
                    viewDesc.Texture2DArray.MipLevels = desc.MipLevels;
                    viewDesc.Texture2DArray.FirstArraySlice = 0;
                    viewDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
                    viewDesc.Texture2DArray.PlaneSlice = 0;
                    viewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
                }
            }
            else
            {
                if (desc.MipLevels > 1)
                {
                    viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
                }
                else
                {
                    viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

                    viewDesc.Texture2D.MostDetailedMip = 0;
                    viewDesc.Texture2D.MipLevels = desc.MipLevels;
                    viewDesc.Texture2D.PlaneSlice = 0;
                    viewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
                }
            }
        }
    }
    break;

    case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
    {
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;

        viewDesc.Texture3D.MostDetailedMip = 0;
        viewDesc.Texture3D.MipLevels = desc.MipLevels;
        viewDesc.Texture3D.ResourceMinLODClamp = 0.0f;
    }
    break;

    default:
    {
        // 想定外
        abort();    // アプリを止める.
    }
    break;
    }

    return viewDesc;
}