#pragma once

#include <d3d12.h>
#include "ComPtr.h"

class Fence
{
	Fence(const Fence&) = delete;				// アクセス禁止
	void operator = (const Fence&) = delete;	// アクセス禁止
public:
	Fence();
	~Fence();


    //Public Methods

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="pDevice">デバイス</param>
    /// <returns>ture:初期化成功 / false:初期化失敗</returns>
    bool Init(ID3D12Device* pDevice);

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// シグナル状態になるまで指定された時間待機する
    /// </summary>
    /// <param name="pQueue">コマンドキュー</param>
    /// <param name="timeout">タイムアウト時間(ミリ秒)</param>
    void Wait(ID3D12CommandQueue* pQueue, UINT timeout);

    /// <summary>
    /// シグナル状態になるまでずっと待機する
    /// </summary>
    /// <param name="pQueue">コマンドキュー</param>
    void Sync(ID3D12CommandQueue* pQueue);
private:
    //Private Variables
    ComPtr<ID3D12Fence> m_pFence;           //!< フェンスです.
    HANDLE              m_Event;            //!< イベントです.
    UINT                m_Counter;          //!< 現在のカウンターです.
};