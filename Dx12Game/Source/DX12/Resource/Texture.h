#pragma once

#include <d3d12.h>
#include <DirectXTK12/ResourceUploadBatch.h>
#include "ComPtr.h"

// Forward Declarations
class DescriptorHandle;
class DescriptorPool;

class Texture
{
    Texture(const Texture&) = delete;               //アクセス禁止.
    void operator = (const Texture&) = delete;      //アクセス禁止.
public:
    Texture();
    ~Texture();

    //Public Methods

    /// <summary> 初期化処理。初期化成功：true返却 </summary>
    bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, const wchar_t* filename, DirectX::ResourceUploadBatch& batch);

    /// <summary> 初期化処理。初期化成功：true返却 </summary>
    bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, const D3D12_RESOURCE_DESC* pDesc, bool isCube);

    /// <summary> 終了処理 </summary>
    void Term();

    /// <summary> CPUディスクリプタハンドルを取得 </summary>
    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

    /// <summary> GPUディスクリプタハンドルを取得 </summary>
    D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

private:
    // Private Variable

    ComPtr<ID3D12Resource>  texture;
    DescriptorHandle* descHandle;
    DescriptorPool* descPool;

    // Private Method

    /// <summary> シェーダリソースビューの設定を求める </summary>
    /// <param name="isCube">キューブマップかどうか？</param>
    D3D12_SHADER_RESOURCE_VIEW_DESC GetViewDesc(bool isCube);
};