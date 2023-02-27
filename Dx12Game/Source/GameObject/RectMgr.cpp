#include "RectMgr.h"

#include <cmath>
#include "GameObjectMgr.h"
#include "GameMath.h"

namespace GameObject
{
	RectMgr::RectMgr() :
		Base(Tag::BackGround, "RectMgr"),
		pool_size(20),
		DistDepth(10),
		MaxScaleWidth(16 * 4.5f),
		MaxScaleHeight(9 * 4.5f),
		Duration(0.15f),
		RunNum(20),
		timeCounter(0.0f),
		remainingRunNum(0)
	{
		// BackgroundRectをプールサイズ分生成
		for (uint32_t i = 0; i < pool_size; ++i)
		{
			auto* rect = new BackgroundRect;
			rect->SetParent(this);
			pool.emplace_back(rect);
		}
		InitRects();
	}

	RectMgr::~RectMgr()
	{
		for (int i = 0; i < pool_size; ++i)
		{
			delete pool.back();
			pool.pop_back();
		}
		pool.clear();
	}

	void RectMgr::Update()
	{
		if (remainingRunNum)
		{
			// 時間をカウント
			timeCounter += Sys::Timer::GetDeltaTime();
			// 1間隔の移動に所要する時間を経過していたら
			if (timeCounter > Duration)
			{
				// 経過時間を所要時間にする
				timeCounter = Duration;
			}

			// プールの座標を更新
			for (int i = 0; i < pool_size; ++i)
			{
				// 設定間隔で並べる + 1間隔 * 経過時間 / 所要時間
				pool[i]->transform->position.y = (-i * DistDepth) + DistDepth * timeCounter / Duration;
			}

			// 経過時間が所要時間ならリセット
			if (timeCounter == Duration)
			{
				timeCounter = 0.0f;
				// 一度実行が終わっているので残りの実行回数をデクリメントする
				--remainingRunNum;
			}
		}
		else
		{
			// プールの座標を更新
			for (int i = 0; i < pool_size; ++i)
			{
				// 設定間隔で並べる
				pool[i]->transform->position.y = -i * DistDepth;
			}
		}

	}

	void RectMgr::Draw() const
	{
		// プールの座標を更新
		for (int i = 0; i < pool_size; ++i)
		{
			// 手前から設置されて行ってるので段々と小さくなる処理
			if (pool[i]->transform->position.y < 0.0f)
			{
				float mag = MyGameMath::FadeMagnification(false, -pool[i]->transform->position.y, DistDepth * pool_size, 0.2f, 1.0f);

				pool[i]->transform->scale.x = MaxScaleWidth * mag;
				pool[i]->transform->scale.z = MaxScaleHeight * mag;
			}
			else
			{
				pool[i]->transform->scale.x = MaxScaleWidth;
				pool[i]->transform->scale.z = MaxScaleHeight;
			}

			pool[i]->Update();
		}
	}

	void RectMgr::InitRects()
	{
		for (int i = 0; i < pool_size; ++i)
		{
			pool[i]->transform->position.y = -i * DistDepth;
			pool[i]->transform->scale.x = MaxScaleWidth;
			pool[i]->transform->scale.z = MaxScaleHeight;

			((pool_size - i) / pool_size);
		}
	}
}