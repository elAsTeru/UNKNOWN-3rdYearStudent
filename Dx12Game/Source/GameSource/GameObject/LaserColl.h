#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class CSphColl;
};

namespace GameObject
{
	class LaserColl : public Base
	{
	public:
		LaserColl();
		~LaserColl();

		// Public Method

		void Init()override;
		void Update() override;			// 更新
		void Draw()const{};

	private:
		// Private Variable

		const float MaxTime;	// 最大半径になる時間
		const float MaxRadius;	// 最大半径

		float timeCounter;

		// Component Variable

		Component::CSphColl* sphColl;	// 球のあたり判定コンポーネント用

		// Component Method

		void OnTriggerEnter(Base* _Other)override;
	};
}