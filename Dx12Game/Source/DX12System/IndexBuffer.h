#pragma once

#include <d3d12.h>
#include <ComPtr.h>
#include <cstdint>

class IndexBuffer
{
    IndexBuffer(const IndexBuffer&) = delete;
    void operator = (const IndexBuffer&) = delete;
public:
    IndexBuffer();
    ~IndexBuffer();
    

    //Public Methods

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="pDevice">デバイス</param>
    /// <param name="size">インデックスバッファサイズ</param>
    /// <param name="pInitData">初期化データ</param>
    /// <returns>true:初期化成功 / false:初期化失敗</returns>
    bool Init(ID3D12Device* pDevice, size_t size, const uint32_t* pInitData = nullptr);

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// メモリマッピング
    /// </summary>
    uint32_t* Map();

    /// <summary>
    /// メモリマッピング解除
    /// </summary>
    void Unmap();

    /// <summary>
    /// インデックスバッファビューを取得
    /// </summary>
    /// <returns>インデックスバッファビューが返却される</returns>
    D3D12_INDEX_BUFFER_VIEW GetView() const;

private:
    //Private Variables
    ComPtr<ID3D12Resource>      m_pIB;      //!< インデックスバッファです.
    D3D12_INDEX_BUFFER_VIEW     m_View;     //!< インデックスバッファビューです.
};
