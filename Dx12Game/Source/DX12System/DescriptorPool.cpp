#include "DescriptorPool.h"

DescriptorPool::DescriptorPool() :
	m_RefCount(1),
	m_Pool(),
	m_pHeap(nullptr),
	m_DescriptorSize(0)
{}

DescriptorPool::~DescriptorPool()
{
	m_Pool.Term();
	m_pHeap.Reset();
	m_DescriptorSize = 0;
}

bool DescriptorPool::Create(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* pDesc, DescriptorPool** ppPool)
{
	// 引数チェック.
	if (pDevice == nullptr || pDesc == nullptr || ppPool == nullptr)
	{
		return false;
	}

	// インスタンスを生成します.
	auto instance = new (std::nothrow) DescriptorPool();
	if (instance == nullptr)
	{
		return false;
	}

	// ディスクリプタヒープを生成.
	auto hr = pDevice->CreateDescriptorHeap(
		pDesc,
		IID_PPV_ARGS(instance->m_pHeap.GetAddressOf()));

	// 失敗したら解放処理を行って終了します.
	if (FAILED(hr))
	{
		instance->Release();
		return false;
	}

	// プールを初期化します.
	if (!instance->m_Pool.Init(pDesc->NumDescriptors))
	{
		instance->Release();
		return false;
	}

	// ディスクリプタの加算サイズを取得します.
	instance->m_DescriptorSize =
		pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);

	// インスタンスを設定.
	*ppPool = instance;

	// 正常終了.
	return true;
}

void DescriptorPool::AddRef() { m_RefCount++; }

void DescriptorPool::Release()
{
	m_RefCount--;
	if (m_RefCount == 0) { delete this; }
}

uint32_t DescriptorPool::GetCount() const { return m_RefCount; }

DescriptorHandle* DescriptorPool::AllocHandle()
{
	// 初期化関数です.
	auto func = [&](uint32_t index, DescriptorHandle* pHandle)
	{
		auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += m_DescriptorSize * index;

		auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += m_DescriptorSize * index;

		pHandle->HandleCPU = handleCPU;
		pHandle->HandleGPU = handleGPU;
	};

	// 初期化関数を実行してからハンドルを返却します.
	return m_Pool.Alloc(func);
}

void DescriptorPool::FreeHandle(DescriptorHandle*& pHandle)
{
	if (pHandle != nullptr)
	{
		// ハンドルをプールに戻します.
		m_Pool.Free(pHandle);

		// nullptrでクリアしておきます.
		pHandle = nullptr;
	}
}

uint32_t DescriptorPool::GetAvailableHandleCount() const { return m_Pool.GetAvailableCount(); }

uint32_t DescriptorPool::GetAllocatedHandleCount() const { return m_Pool.GetUsedCount(); }

uint32_t DescriptorPool::GetHandleCount() const { return m_Pool.GetSize(); }

ID3D12DescriptorHeap* const DescriptorPool::GetHeap() const { return m_pHeap.Get(); }