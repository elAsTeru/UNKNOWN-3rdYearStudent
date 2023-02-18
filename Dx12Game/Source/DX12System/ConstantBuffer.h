#pragma once

#include <d3d12.h>
#include <ComPtr.h>
#include <vector>

class DescriptorHandle;
class DescriptorPool;

class ConstantBuffer
{
	ConstantBuffer(const ConstantBuffer&) = delete;       // アクセス禁止.
	void operator = (const ConstantBuffer&) = delete;       // アクセス禁止.
public:
	ConstantBuffer();
	~ConstantBuffer();


	//Public Methods

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="pDevice">デバイス</param>
	/// <param name="pPool">ディスクリプタプール</param>
	/// <returns>true:初期化成功 / false:初期化失敗</returns>
	bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, size_t size);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Term();

	/// <summary>
	/// GPU仮想アドレスを取得
	/// </summary>
	/// <returns>GPU仮想アドレスが返却される</returns>
	D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

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

	/// <summary>
	/// メモリマッピング済みポインタを取得
	/// </summary>
	/// <returns>メモリマッピング済みポインタが返却される
	void* GetPtr() const;

	/// <summary>
	/// メモリマッピング済みポインタを取得
	/// </summary>
	/// <returns>メモリマッピング済みポインタが返却される</returns>
	template<typename T>
	T* GetPtr() { return reinterpret_cast<T*>(GetPtr()); }

private:
	ComPtr<ID3D12Resource>			m_pCB;			//!< 定数バッファ.
	DescriptorHandle*				m_pHandle;		//!< ディスクリプタハンドルです.
	DescriptorPool*					m_pPool;		//!< ディスクリプタプールです.
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc;			//!< 定数バッファビューの構成設定.
	void*							m_pMappedPtr;	//!< マップ済みポインタ.
};