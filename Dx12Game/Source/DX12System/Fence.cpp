



#include "Fence.h"

Fence::Fence():
	m_pFence(nullptr),
	m_Event(nullptr),
	m_Counter(0)
{}

Fence::~Fence() { Term(); }

bool Fence::Init(ID3D12Device* pDevice)
{
    if (pDevice == nullptr) { return false; }

    // イベントを生成.
    m_Event = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    if (m_Event == nullptr) { return false; }

    // フェンスを生成.
    auto hr = pDevice->CreateFence
    (
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(m_pFence.GetAddressOf())
    );
    if (FAILED(hr)) { return false; }

    // カウンタ設定.
    m_Counter = 1;

    // 正常終了.
    return true;
}

void Fence::Term()
{
    // ハンドルを閉じる.
    if (m_Event != nullptr)
    {
        CloseHandle(m_Event);
        m_Event = nullptr;
    }

    // フェンスオブジェクトを破棄.
    m_pFence.Reset();

    // カウンターリセット.
    m_Counter = 0;
}

void Fence::Wait(ID3D12CommandQueue* pQueue, UINT timeout)
{
    if (pQueue == nullptr) { return; }

    const auto fenceValue = m_Counter;

    // シグナル処理.
    auto hr = pQueue->Signal(m_pFence.Get(), fenceValue);
    if (FAILED(hr)) { return; }

    // カウンターを増やす.
    m_Counter++;

    // 次のフレームの描画準備がまだであれば待機する.
    if (m_pFence->GetCompletedValue() < fenceValue)
    {
        // 完了時にイベントを設定.
        auto hr = m_pFence->SetEventOnCompletion(fenceValue, m_Event);
        if (FAILED(hr)) { return; }

        // 待機処理.
        if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_Event, timeout, FALSE)) { return; }
    }
}

void Fence::Sync(ID3D12CommandQueue* pQueue)
{
    if (pQueue == nullptr) { return; }

    // シグナル処理.
    auto hr = pQueue->Signal(m_pFence.Get(), m_Counter);
    if (FAILED(hr)) { return; }

    // 完了時にイベントを設定.
    hr = m_pFence->SetEventOnCompletion(m_Counter, m_Event);
    if (FAILED(hr)) { return; }

    // 待機処理.
    if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_Event, INFINITE, FALSE)) { return; }

    // カウンターを増やす.
    m_Counter++;
}