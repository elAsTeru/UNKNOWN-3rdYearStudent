#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"
#include "Tool/InputMgr.h"

namespace Component
{
	class CPlayerMove : public Base
	{
	public:
		// Public Variable
		float speed{};		// 移動速度
		float timeCounter;

		// Public Method
		void Start()override 
		{
		};

		// Public Method
		/// <summary>
		/// 移動処理、移動処理が入った場合n入力量を返却
		/// </summary>
		Vector2 Update()
		{
			// 移動Axisの取得
			Vector2 axis = Input::Mgr::GetAxisL();

			// 入力が無ければ処理を終了
			if((axis.x == 0.000f) && (axis.y == 0.000f))
			{ return Vector2::Zero; }

			// 入力量を移動量に(乗算)
			axis.x *= (speed * Sys::Timer::GetHitStopTime());
			axis.y *= (speed * Sys::Timer::GetHitStopTime());
			// 移動量を適応
			this->parent->transform->position.x += axis.x;
			this->parent->transform->position.z += axis.y;


			//this->parent->transform->rotation.y = MyMath::Vec2ToAnglesLH(axis);
			return axis;	// 移動処理を行った
		}
	};
}