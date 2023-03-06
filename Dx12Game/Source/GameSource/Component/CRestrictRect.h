#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"

const float MaxRight = 16 * 2.0f;		// 画面比率 * 倍率
const float MaxDepth = 9 * 2.0f;

namespace Component
{
	/// <summary> オブジェクトの移動に矩形の移動制限をかける。 </summary>
	class CRestrictRect : public Base
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
			if (parent->transform->position.x >= MaxRight)
			{
				if (this->isEnable)
				{
					parent->transform->position.x = static_cast<float>(MaxRight - 0.1f);
				}
				isRun = true;
			}
			// 左
			else if (parent->transform->position.x <= -MaxRight)
			{
				if (this->isEnable)
				{
					parent->transform->position.x = static_cast<float>(-MaxRight + 0.1f);
				}
				isRun = true;
			}

			// 上
			if (parent->transform->position.z >= MaxDepth)
			{
				if (this->isEnable)
				{
					parent->transform->position.z = static_cast<float> (MaxDepth - 0.1f);
				}
				isRun = true;
			}
			// 下
			else if (parent->transform->position.z <= -MaxDepth)
			{
				if (this->isEnable)
				{
					parent->transform->position.z = static_cast<float>(-MaxDepth + 0.1);
				}
				isRun = true;
			}
			return isRun;
		}
	};
}