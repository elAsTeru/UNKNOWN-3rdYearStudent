#pragma once
// ---------------------------------------------------------------------------------------------
// 背景のキューブ、生成後一定時間で無効化
// ---------------------------------------------------------------------------------------------
#include "GameObjectBase.h"

namespace GameObject
{
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

		float speed;		// 移動速度
		Vector3 dir;		// 進む方向
		float duration;		// 持続時間
		float timeCounter;	// 時間カウンタ
	};
}