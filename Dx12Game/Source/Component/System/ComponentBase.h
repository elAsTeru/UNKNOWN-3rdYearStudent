#pragma once

namespace GameObject
{
	class Base;
}

namespace Component
{
	/// <summary>
	/// BaseClass : Component
	/// </summary>
	class Base
	{
	public:
		Base() {}
		virtual ~Base() {}

		//Public Variable
		bool isEnable = true;				// ?コンポーネントが有効か
		GameObject::Base* parent = nullptr;	// コンポーネントを所有するオブジェクト
		//Public Method
		virtual void Start() = 0;			// 開始
	};
}