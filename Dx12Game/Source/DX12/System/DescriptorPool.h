#pragma once

#include <d3d12.h>
#include <atomic>
#include <Pool.h>
#include "ComPtr.h"

class DescriptorHandle
{
public:
    D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;  //!< CPUディスクリプタハンドルです.
    D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;  //!< GPUディスクリプタハンドルです.

    bool HasCPU() const
    {
        return HandleCPU.ptr != 0;
    }

    bool HasGPU() const
    {
        return HandleGPU.ptr != 0;
    }
};

class DescriptorPool
{
    DescriptorPool();
    ~DescriptorPool();

    DescriptorPool(const DescriptorPool&) = delete;     // アクセス禁止.
    void operator = (const DescriptorPool&) = delete;   // アクセス禁止.
public:
    //Public Methods
    
    /// <summary>
    /// 生成処理を行う
    /// </summary>
    /// <param name="pDevice">デバイス</param>
    /// <param name="pDesc">ディスクリプタヒープの構成設定</param>
    /// <param name="ppPool">ディスクリプタプールの格納先</param>
    /// <returns>true:生成処理成功 / false:失敗</returns>
    static bool Create(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* pDesc, DescriptorPool** ppPool);

    /// <summary>
    /// 参照カウントを増やす
    /// </summary>
    void AddRef();

    /// <summary>
    /// 解放処理を行う
    /// </summary>
    void Release();

    /// <summary>
    /// 参照カウントを取得
    /// </summary>
    /// <returns>参照カウントが返却される</returns>
    uint32_t GetCount()const;

    /// <summary>
    /// ディスクリプタハンドルを割り当て
    /// </summary>
    /// <returns>割り当てられたディスクリプタハンドルが返却される</returns>
    DescriptorHandle* AllocHandle();

    /// <summary>
    /// ディスクリプタハンドルを解放
    /// </summary>
    /// <param name="pHandle">解放するハンドルへのポインタ</param>
    void FreeHandle(DescriptorHandle*& pHandle);

    /// <summary>
    /// 利用可能なハンドル数を取得
    /// </summary>
    /// <returns>利用可能なハンドル数が返却される</returns>
    uint32_t GetAvailableHandleCount() const;

    /// <summary>
    /// 割り当て済みのハンドル数を取得
    /// </summary>
    /// <returns> 割り当て済みのハンドル数が返却される</returns>
    uint32_t GetAllocatedHandleCount() const;

    /// <summary>
    /// ハンドル総数を取得
    /// </summary>
    /// <returns>ハンドル総数が返却される</returns>
    uint32_t GetHandleCount() const;

    /// <summary>
    /// ディスクリプタヒープを取得
    /// </summary>
    /// <returns>ディスクリプタヒープが返却される</returns>
    ID3D12DescriptorHeap* const GetHeap() const;

private:
    //Private Variable
    std::atomic<uint32_t>           m_RefCount;         //!< 参照カウント
    Pool<DescriptorHandle>          m_Pool;             //!< ディスクリプタハンドルプール
    ComPtr<ID3D12DescriptorHeap>    m_pHeap;            //!< ディスクリプタヒープ
    uint32_t                        m_DescriptorSize;   //!< ディスクリプタサイズ
};