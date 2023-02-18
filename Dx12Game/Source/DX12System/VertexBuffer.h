#pragma once

#include <d3d12.h>
#include <ComPtr.h>

class VertexBuffer
{
    VertexBuffer(const VertexBuffer&) = delete;         // アクセス禁止
    void operator = (const VertexBuffer&) = delete;     // アクセス禁止
public:
    VertexBuffer();
    ~VertexBuffer();


    //Public Methods

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="pDevice">デバイス</param>
    /// <param name="size">頂点バッファサイズ</param>
    /// <param name="stride">1頂点当たりのサイズ</param>
    /// <param name="pInitData">初期化データ</param>
    /// <returns>true:初期化成功 : false: 初期化失敗</returns>
    bool Init(ID3D12Device* pDevice, size_t size, size_t stride, const void* pInitData = nullptr);
    
    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="pDevice">デバイス</param>
    /// <param name="size">頂点バッファサイズ</param>
    /// <param name="pInitData">初期化データ</param>
    /// <returns>true:初期化成功 / false:初期化失敗</returns>
    template<typename T>
    bool Init(ID3D12Device* pDevice, size_t size, const T* pInitData = nullptr)
    { return Init(pDevice, size, sizeof(T), pInitData ); }

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// メモリマッピング
    /// </summary>
    void* Map();

    /// <summary>
    /// メモリマッピング解除
    /// </summary>
    void Unmap();

    /// <summary>
    /// メモリマッピング
    /// </summary>
    template<typename T>
    T* Map() const
    { return reinterpret_cast<T*>(Map()); }

    /// <summary>
    /// 頂点バッファビュー取得
    /// </summary>
    /// <returns>頂点バッファビューが返却される</returns>
    D3D12_VERTEX_BUFFER_VIEW GetView() const;

private:
    //Private Variables
    ComPtr<ID3D12Resource>      m_pVB;      //!< 頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW    m_View;     //!< 頂点バッファビュー
};
