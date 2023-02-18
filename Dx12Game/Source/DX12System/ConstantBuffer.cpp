


#include "ConstantBuffer.h"
#include "DescriptorPool.h"

ConstantBuffer::ConstantBuffer():
	m_pCB			(nullptr),
	m_pHandle		(nullptr),
	m_pPool			(nullptr),
	m_pMappedPtr	(nullptr)
{}

ConstantBuffer::~ConstantBuffer() { Term(); }

bool ConstantBuffer::Init(ID3D12Device* pDevice, DescriptorPool* pPool, size_t size)
{
    if (pDevice == nullptr || pPool == nullptr || size == 0)
    {
        return false;
    }

    assert(m_pPool == nullptr);
    assert(m_pHandle == nullptr);

    m_pPool = pPool;
    m_pPool->AddRef();

    size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
    UINT64 sizeAligned = (size + (align - 1)) & ~(align - 1); // alignに切り上げる.

    // ヒーププロパティ.
    D3D12_HEAP_PROPERTIES prop{};
    prop.Type = D3D12_HEAP_TYPE_UPLOAD;
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    prop.CreationNodeMask = 1;
    prop.VisibleNodeMask = 1;

    // リソースの設定.
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = sizeAligned;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // リソースを生成.
    auto hr = pDevice->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_pCB.GetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    // メモリマッピングしておきます.
    hr = m_pCB->Map(0, nullptr, &m_pMappedPtr);
    if (FAILED(hr))
    {
        return false;
    }

    m_Desc.BufferLocation = m_pCB->GetGPUVirtualAddress();
    m_Desc.SizeInBytes = UINT(sizeAligned);
    m_pHandle = pPool->AllocHandle();

    pDevice->CreateConstantBufferView(&m_Desc, m_pHandle->HandleCPU);

    // 正常終了.
    return true;
}

void ConstantBuffer::Term()
{
    // メモリマッピングを解除して，定数バッファを解放します.
    if (m_pCB != nullptr)
    {
        m_pCB->Unmap(0, nullptr);
        m_pCB.Reset();
    }

    // ビューを破棄.
    if (m_pPool != nullptr)
    {
        m_pPool->FreeHandle(m_pHandle);
        m_pHandle = nullptr;
    }

    // ディスクリプタプールを解放.
    if (m_pPool != nullptr)
    {
        m_pPool->Release();
        m_pPool = nullptr;
    }

    m_pMappedPtr = nullptr;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress() const { return m_Desc.BufferLocation; }

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetHandleCPU() const
{
    if (m_pHandle == nullptr)
    {
        return D3D12_CPU_DESCRIPTOR_HANDLE();
    }

    return m_pHandle->HandleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer::GetHandleGPU() const
{
    if (m_pHandle == nullptr)
    {
        return D3D12_GPU_DESCRIPTOR_HANDLE();
    }

    return m_pHandle->HandleGPU;
}

void* ConstantBuffer::GetPtr() const { return m_pMappedPtr; }