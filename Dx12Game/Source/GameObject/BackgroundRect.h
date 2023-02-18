#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class RectMgr;

	class BackgroundRect : public Base
	{
	public:
		BackgroundRect();
		~BackgroundRect();

		void Start() {}
		void Init()override;
		void Update();
		void Draw();
		void SetParent(RectMgr* _Parent) { parent = _Parent; }
	private:
		RectMgr* parent;	// 親オブジェクト(Mgr)
	};
}