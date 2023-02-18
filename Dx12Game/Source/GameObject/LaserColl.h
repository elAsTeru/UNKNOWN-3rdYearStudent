#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class SphColl;
};

namespace GameObject
{
	class LaserColl : public Base
	{
	public:
		LaserColl();
		~LaserColl();

		// Public Method
		void Update() override;			// 更新
		void Init()override;

	private:
		// Component Variable
		float timeCounter;
		Component::SphColl* sphColl;	// 球のあたり判定コンポーネント用
		// Component Method
		void OnTriggerEnter(Base* _Other)override;
	};
}