#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class StageUI : public Base
	{
	public:
		StageUI();
		~StageUI();

		void Start() {}
		void Init()override {}
		void Update();

	private:
		// Private Variable	
		// Private Method
		void Draw();
	};
}