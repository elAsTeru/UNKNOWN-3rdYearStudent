#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class BackgroundCube : public Base
	{
	public:
		BackgroundCube();
		~BackgroundCube();

		void Start() {}
		void Init()override;
		void Update();
		void Draw();
	
	private:
		// Private Variable
		float speed;		// 移動速度
		Vector3 dir;		// 進む方向
		float duration;		// 持続時間
		float timeCounter;	// 時間カウンタ
	};
}