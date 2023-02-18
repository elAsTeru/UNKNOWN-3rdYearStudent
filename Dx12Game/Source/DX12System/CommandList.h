#pragma once

#include <d3d12.h>
#include "ComPtr.h"
#include <cstdint>
#include <vector>

class CommandList
{
    CommandList(const CommandList&) = delete;
    void operator = (const CommandList&) = delete;
public:
	CommandList();
	~CommandList();

    //Public Methods

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="pDevice">デバイス</param>
	/// <param name="type">コマンドリストタイプ</param>
	/// <param name="count">アロケータ数、ダブルバッファ化する場合は2に設定</param>
	/// <returns>true:初期化成功 / false:初期化失敗</returns>
	bool Init(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, uint32_t count);

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// リセット処理を行ったコマンドリストを取得
    /// </summary>
    /// <returns>リセット処理を行ったコマンドリストが返却される</returns>
    ID3D12GraphicsCommandList* Reset();

private:
    //Private Variables
    ComPtr<ID3D12GraphicsCommandList>           m_pCmdList;         //!< コマンドリストです.
    std::vector<ComPtr<ID3D12CommandAllocator>> m_pAllocators;      //!< コマンドアロケータです.
    uint32_t                                    m_Index;            //!< アロケータ番号です.
};