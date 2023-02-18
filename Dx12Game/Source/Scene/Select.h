#pragma once
#include "SceneBase.h"

namespace GameObject
{
	class SelectUI;
	class MenuUI;
}

namespace Scene
{
	class Select : public Base
	{
	public:
		Select();
		virtual ~Select();

		//Public Method
		virtual void Update()override;

	private:
		// Private Variable
		int bgmHandle;	// BGMのハンドル
		GameObject::SelectUI* selectUI = nullptr;
		GameObject::MenuUI* menuUI = nullptr;
	};
}