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

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="pDevice">デバイス</param>
    /// <param name="pPool">ディスクリプタプール</param>
    /// <param name="filename">ファイル名</param>
    /// <param name="batch">更新バッチ、テクスチャの更新に必要なデータを格納</param>
    /// <returns>true:初期化成功 / false:初期化失敗</returns>
    bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, const wchar_t* filename, DirectX::ResourceUploadBatch& batch);

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="pDevice">デバイス</param>
    /// <param name="pPool">ディスクリプタプール</param>
    /// <param name="pDesc">構成設定</param>
    /// <param name="isCube">キューブマップの場合[true]を指定</param>
    /// <returns>ture:初期化成功 / false:初期化失敗</returns>
    bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, const D3D12_RESOURCE_DESC* pDesc, bool isCube);

    // <summary>
    // 終了処理
    // </summary>
    void Term();

    /// <summary>
    /// CPUディスクリプタハンドルを取得
    /// </summary>
    /// <returns>CPUディスクリプタハンドルが返却される</returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

    /// <summary>
    /// GPUディスクリプタハンドルを取得
    /// </summary>
    /// <returns>GPUディスクリプタハンドルが返却される</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

private:
    // private variables
    ComPtr<ID3D12Resource>  m_pTex;
    DescriptorHandle* m_pHandle;
    DescriptorPool* m_pPool;


    // private methods

    /// <summary>
    /// シェーダリソースビューの設定を求める
    /// </summary>
    /// <param name="isCube">キューブマップかどうか？</param>
    /// <returns>シェーダリソースビューの設定が返却される</returns>
    D3D12_SHADER_RESOURCE_VIEW_DESC GetViewDesc(bool isCube);
};