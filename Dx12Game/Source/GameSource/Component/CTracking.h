#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"
#include <cmath>

namespace Component
{
	class CTracking : public Base
	{
	public:
		// Public Method
		void Start()override 
		{
		};

		void Init(GameObject::Base* _Target ,const float _Speed)
		{
			this->aimPos = _Target;
			this->speed = _Speed;
		}

		Vector3 Update()
		{
			// ターゲットがいなければ探索する
			if(!aimPos)
			{ { return{}; } }

			// 目的地を取得
			Vector3& targetPos = aimPos->transform->position;
			// 距離を求め、進む方向を決定
			Vector3 forward = targetPos - parent->transform->position;
			// 距離が0.0以下の場合終了
			if (std::abs(forward.x) <= 0.0f && std::abs(forward.z) <= 0.0f)
			{ return{}; }
			
			// 距離から単位ベクトルを作成
			forward.Normalize();
			// 移動処理
			parent->transform->position += forward * Sys::Timer::GetHitStopTime() * this->speed;
			// 移動方向の角度を取得
			this->dir = static_cast<float>(MyMath::Vec2ToAnglesLH(Vector2(forward.x, forward.y)));

			return forward;
		}

	private:
		// Private Variable
		GameObject::Base* aimPos;
		float dir;
		float speed;
	};
}