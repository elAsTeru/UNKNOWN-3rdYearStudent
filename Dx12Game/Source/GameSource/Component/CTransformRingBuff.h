#pragma once
#include "ComponentBase.h"
#include <list>
#include "CTransform.h"

#include <iostream>

namespace Component
{
	/// <summary>
	/// 変換系の情報をためておく
	/// </summary>
	class CTransformRingBuff : public Base
	{
	public:
		// Public Variable

		float interval;						// 記録する間隔

	private:
		// Private Variable
		int buffSize;						// 記録する数
		std::list<CTransform> buffs;
		uint8_t prev;						// 前回記録した位置
		float timeCounter;


	public:

		// Public Method

		void Start()override
		{};

		void Init()
		{
			this->buffs.clear();
			this->prev = 0;
			this->timeCounter = 0.0f;
			this->interval = 0.0f;
			this->buffSize = 0;
		}

		void Update(const float& _DeltaTime)
		{
			// 計測時間が記録間隔を超えたら
			this->timeCounter += _DeltaTime;
			if (timeCounter < this->interval)
			{
				return;
			}
			this->timeCounter = 0.0f;

			// 前回の記録位置から今回記録する位置を取得

			// 変換系の情報を記録
			CTransform* transform = parent->transform;

			if (buffs.size() >= buffSize)
			{
				this->buffs.pop_back();
			}

			this->buffs.emplace_front(*transform);
		}

		// 値を全てクリア後、サイズを変更する
		void SetBuffSize(const uint8_t& _Size)
		{
			this->buffs.clear();
			this->buffs.resize(_Size);
			this->buffSize = _Size;

			for (auto buff : buffs)
			{
				buff = *this->parent->transform;
			}
		}

		CTransform GetBuff(const uint8_t& _Num)
		{
			if (_Num > this->buffSize)
			{
				return *this->buffs.begin();
			}

			return *std::next(this->buffs.begin(), _Num);
		}
	};
}