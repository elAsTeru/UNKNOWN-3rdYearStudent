#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"
#include <cmath>

namespace Component
{
	/// <summary>
	/// 指定された座標(target)に向かって移動
	/// </summary>
	class CMoveAimPos : public Base
	{
	public:
		// Private Variable
		Vector3 aimPos;		// 目標座標
		float speed;		// 移動速度

		// Public Method
		void Start()override 
		{};

		void Init()
		{
			this->aimPos = Vector3(0.0f);
			this->speed = 0.0f;
		}

		/// <summary>
		/// 移動方向の単位ベクトルを返却する
		/// </summary>
		Vector3 Update()
		{
			// this→targetへのベクトル
			Vector3 startPos = this->parent->transform->position;
			Vector3 vec = this->aimPos - startPos;
			// 単位ベクトル化
			Vector3 dir = vec;
			dir.Normalize();
			// 移動処理
			this->parent->transform->position += dir * speed * Sys::Timer::GetHitStopTime();
			// 移動後の位置が目標を通過してしまっていたら
			if (vec.Length() < Vector3(this->parent->transform->position - startPos).Length())
			{
				// 補正する
				this->parent->transform->position = aimPos;
			}
			return dir;
		}
	};
}