#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"
#include "InputMgr.h"

namespace Component
{
	class CFixPos : public Base
	{
	public:
		// Public Variable

		// Public Method
		void Start()override 
		{
		};

		bool Update()
		{
			bool isRun = false;

			// 画面外に出ていたら戻す
			// 右
			if (parent->transform->position.x >= MAX_RIGHT)
			{
				parent->transform->position.x = static_cast<float>(MAX_RIGHT - 0.1f);
				isRun = true;
			}
			// 左
			else if (parent->transform->position.x <= -MAX_RIGHT)
			{
				parent->transform->position.x = static_cast<float>(-MAX_RIGHT + 0.1f);
				isRun = true;
			}

			// 上
			if (parent->transform->position.z >= MAX_DEPTH)
			{
				parent->transform->position.z = static_cast<float> (MAX_DEPTH - 0.1f);
				isRun = true;
			}
			// 下
			else if (parent->transform->position.z <= -MAX_DEPTH)
			{
				parent->transform->position.z = static_cast<float>(-MAX_DEPTH + 0.1);
				isRun = true;
			}
			return isRun;
		}
	};
}