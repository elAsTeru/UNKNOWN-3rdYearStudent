#pragma once
// -------------------------------
// フィールドの四角に続く奥の四角
// -------------------------------
#include "GameObjectBase.h"

namespace GameObject
{
	class RectMgr;

	class BackgroundRect : public Base
	{
	public:
		BackgroundRect();
		~BackgroundRect();

		void Init()override;
		void Update();
		void Draw()const override;

		void SetParent(RectMgr* _Parent) { parent = _Parent; }
	private:
		RectMgr* parent;	// 親オブジェクト(Mgr)
	};
}