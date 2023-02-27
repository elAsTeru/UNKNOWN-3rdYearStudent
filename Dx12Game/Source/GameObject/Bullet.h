#pragma once
// -----------------------
// プレイヤーが使用する弾
// -----------------------
#include "GameObjectBase.h"

namespace Component
{
	class SphColl;
	class CMoveForwardY;
	class CFixPos;
}

namespace GameObject
{
	class Bullet : public Base
	{
	public:
		Bullet();
		~Bullet();

		void Init()override;
		void Update()override;
		void Draw()const override;

	private:
		// Component Variable
		
		Component::SphColl* sphColl;
		Component::CMoveForwardY* cMoveForwardY;
		Component::CFixPos* cFixPos;

		// Component Method
		
		void OnTriggerEnter(Base* _Other) override;
	};
}

// プレイヤーの移動速度と向きを弾の速度に足したい
// 移動方向に打つと弾が早く
// 移動方向と逆に打つと遅くなる