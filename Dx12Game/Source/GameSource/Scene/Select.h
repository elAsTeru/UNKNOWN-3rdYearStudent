#pragma once
#include "SceneBase.h"

namespace GameObject
{
	class SelectUI;
}

namespace Scene
{
	class Select : public Base
	{
	public:
		Select();
		virtual ~Select();

		//Public Method
		void Update()override;

	private:
		// Private Variable
		GameObject::SelectUI* selectUI = nullptr;
	};
}