#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"
#include <cmath>

namespace Component
{
	/// <summary>
	/// 向いている方向に進ませる(rotation.yの向きを使用)
	/// </summary>
	class CMoveForwardY : public Base
	{
	public:
		// Private Variable
		float speed;		// 回転速度

		// Public Method
		void Start()override 
		{};

		void Init()
		{
			this->speed = 0.0f;
		}

		void Update()
		{
			// 向きYをベクトル(XZ)に変換
			auto temp = MyMath::AngleToVecs2LH(this->parent->transform->rotation.y);
			Vector3 unitDir(temp.x, 0, temp.y);
			// 移動速度を乗算して移動
			this->parent->transform->position += unitDir * this->speed * MySys::Timer::GetHitStopTime();
		}
	};
}