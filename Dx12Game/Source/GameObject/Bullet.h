#pragma once
#include "GameObjectBase.h"
#include "SphereCollider.h"

namespace GameObject
{
	class Bullet : public Base
	{
	public:
		Bullet();
		~Bullet();

		void Start() {}
		void Init()override;

	private:
		// Private Variable
		float speed;						// 弾の速度

		// Private Method
		void Update();

		// Component Variable
		Component::SphColl* sphColl;
		// Component Method
		void OnTriggerEnter(Base* _Other) override;
	};
}

// プレイヤーの移動速度と向きを弾の速度に足したい
// 移動方向に打つと弾が早く
// 移動方向と逆に打つと遅くなる