#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class CSphColl;
	class CMoveForwardY;
	class  CRestrictRect;
}

namespace GameObject
{
	/// <summary> プレイヤーが撃つ弾 </summary>
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
		
		Component::CSphColl* sphColl;
		Component::CMoveForwardY* cMoveForwardY;
		Component::CRestrictRect* cRestRect;

		// Component Method
		
		void OnTriggerEnter(Base* _Other) override;
	};
}

// プレイヤーの移動速度と向きを弾の速度に足したい
// 移動方向に打つと弾が早く
// 移動方向と逆に打つと遅くなる