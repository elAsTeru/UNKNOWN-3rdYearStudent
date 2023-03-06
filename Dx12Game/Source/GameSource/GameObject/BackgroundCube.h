#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	/// <summary> 背景に流れるキューブ </summary>
	class BackgroundCube : public Base
	{
	public:
		BackgroundCube();
		~BackgroundCube();

		void Init()override;
		void Update()override;
		void Draw()const override;
	
	private:
		// Private Variable

		const float LifeTime;	// 生存時間
		const float Speed;		// 移動速度
		const Vector3 MoveDir;	// 移動方向

		float lifeTime;		// 生存時間
	};
}