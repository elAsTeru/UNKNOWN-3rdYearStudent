//-----------------------------------------------------------------------------
// File : Model.h
// Desc : Model Module.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "ResMesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh
{
	Mesh(const Mesh&) = delete;					// アクセス禁止
	void operator = (const Mesh&) = delete;     // アクセス禁止
public:
	Mesh();
	virtual ~Mesh();


    //Public Methods

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="pDevice">デバイス</param>
    /// <param name="resource">リソースメッシュ</param>
    /// <returns>true:初期化成功 / false:初期化失敗</returns>
    bool Init(ID3D12Device* pDevice, const ResMesh& resource);

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="pCmdList">コマンドリスト</param>
    void Draw(ID3D12GraphicsCommandList* pCmdList) const;

    /// <summary>
    /// マテリアルIDを取得
    /// </summary>
    /// <returns>マテリアルIDが返却される</returns>
    uint32_t GetMaterialId() const;

private:
    // private variables.
    VertexBuffer    m_VB;               //!< 頂点バッファ
    IndexBuffer     m_IB;               //!< インデックスバッファ
    uint32_t        m_MaterialId;       //!< マテリアルID
    uint32_t        m_IndexCount;       //!< インデックス数
};