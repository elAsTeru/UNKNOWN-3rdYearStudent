#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class RectMgr;

	/// <summary> フィールドの矩形の奥に続いている矩形 </summary>
	class BackgroundRect : public Base
	{
	public:
		BackgroundRect();
		~BackgroundRect();

		void Init(){}
		void Update();
		void Draw()const override;

		void SetParent(RectMgr* _Parent) { parent = _Parent; }
	private:
		RectMgr* parent;	// 親オブジェクト(RectMgr)
	};
}