#pragma once

#include <cstdint>
#include <mutex>
#include <cassert>
#include <functional>

template<typename T>
class Pool
{
public:
	Pool() :
		m_pBuffer(nullptr),
		m_pActive(nullptr),
		m_pFree(nullptr),
		m_Capacity(0),
		m_Count(0)
	{}

	~Pool() { Term(); }


	//Public Methods

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="_count">確保するアイテム数</param>
	/// <returns>true:初期化に成功 / false:初期化に失敗</returns>
	bool Init(uint32_t _count)
	{
		std::lock_guard<std::mutex>guard(m_Mutex);

		m_pBuffer = static_cast<uint8_t*>(malloc(sizeof(Item) * (_count + 2)));
		if (m_pBuffer == nullptr) { return false; }

		m_Capacity = _count;

		//インデックスを割り振る
		for (auto i = 2u, j = 0u; i < m_Capacity + 2; ++i, ++j)
		{
			auto item = GetItem(i);
			item->m_Index = j;
		}

		m_pActive = GetItem(0);
		m_pActive->m_pPrev = m_pActive->m_pNext = m_pActive;
		m_pActive->m_Index = uint32_t(-1);

		m_pFree = GetItem(1);
		m_pFree->m_Index = uint32_t(-2);

		for (auto i = 1u; i < m_Capacity + 2; ++i)
		{
			GetItem(i)->m_pPrev = nullptr;
			GetItem(i)->m_pNext = GetItem(i + 1);
		}

		GetItem(m_Capacity + 1)->m_pPrev = m_pFree;

		m_Count = 0;

		return true;
	}

	/// <summary>
	/// 終了処理
	/// </summary>
	void Term()
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		if (m_pBuffer)
		{
			free(m_pBuffer);
			m_pBuffer = nullptr;
		}

		m_pActive = nullptr;
		m_pFree = nullptr;
		m_Capacity = 0;
		m_Count = 0;
	}

	/// <summary>
	/// アイテムを確保する
	/// </summary>
	/// <param name="func">ユーザによる初期化処理</param>
	/// <returns>確保したアイテムへのポインタ、確保に失敗した場合はnullptrが返却される</returns>
	T* Alloc(std::function<void(uint32_t, T*)> func = nullptr)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		if (m_pFree->m_pNext == m_pFree || m_Count + 1 > m_Capacity)
		{
			return nullptr;
		}

		auto item = m_pFree->m_pNext;
		m_pFree->m_pNext = item->m_pNext;

		item->m_pPrev = m_pActive->m_pPrev;
		item->m_pNext = m_pActive;
		item->m_pPrev->m_pNext = item->m_pNext->m_pPrev = item;

		m_Count++;

		// メモリ割り当て.
		auto val = new ((void*)item) T();

		// 初期化の必要があれば呼び出す.
		if (func != nullptr)
		{
			func(item->m_Index, val);
		}

		return val;
	}

	/// <summary>
	/// アイテムを解放する
	/// </summary>
	/// <param name="pValue">解放するアイテムへのポインタ</param>
	void Free(T* pValue)
	{
		if (pValue == nullptr)
		{
			return;
		}

		std::lock_guard<std::mutex> guard(m_Mutex);

		auto item = reinterpret_cast<Item*>(pValue);

		item->m_pPrev->m_pNext = item->m_pNext;
		item->m_pNext->m_pPrev = item->m_pPrev;

		item->m_pPrev = nullptr;
		item->m_pNext = m_pFree->m_pNext;

		m_pFree->m_pNext = item;
		m_Count--;
	}

	/// <summary>
	/// 総アイテム数を取得
	/// </summary>
	/// <returns>総アイテム数が返却される</returns>
	uint32_t GetSize() const { return m_Capacity; }

	/// <summary>
	/// 使用中のアイテム数を取得
	/// </summary>
	/// <returns>使用中のアイテム数が返却される</returns>
	uint32_t GetUsedCount() const { return m_Count; }

	/// <summary>
	/// 利用可能なアイテム数を取得
	/// </summary>
	/// <returns>利用可能なアイテム数が返却される</returns>
	uint32_t GetAvailableCount() const { return m_Capacity - m_Count; }

private:
	//Private Structure
	struct Item
	{
		T	m_Value;
		uint32_t	m_Index;
		Item* m_pNext;
		Item* m_pPrev;

		Item()
			: m_Value()
			, m_Index(0)
			, m_pNext(nullptr)
			, m_pPrev(nullptr)
		{ /* DO_NOTHING */
		}

		~Item()
		{ /* DO_NOTHING */
		}
	};


	//Private Variables
	uint8_t*	m_pBuffer;	//!< バッファ
	Item*		m_pActive;	//!< アクティブアイテムの頭
	Item*		m_pFree;	//!< フリーアイテムの先頭
	uint32_t	m_Capacity;	//!< 総アイテム数
	uint32_t	m_Count;	//!< 確保したアイテム数0
	std::mutex	m_Mutex;	//!< ミューテックス(排他制御の仕組み)


	//Private Method

	/// <summary>
	/// 
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	Item* GetItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);
		return reinterpret_cast<Item*>(m_pBuffer + sizeof(Item) * index);
	}

	/// <summary>
	/// アイテムにメモリを割り当てる
	/// </summary>
	/// <param name="index">取得するアイテムのインデックス</param>
	/// <returns>アイテムへのポインタが返却される</returns>
	Item* AssignItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);
		auto buf = (m_pBuffer + sizeof(Item) * index);
		return new (buf) Item;
	}

	Pool(const Pool&) = delete;
	void operator = (const Pool&) = delete;
};