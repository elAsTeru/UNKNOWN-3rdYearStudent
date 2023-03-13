#pragma once
#include "SceneBase.h"

namespace GameObject
{
	class EnemyMgr;
	class Field;
}

namespace Scene
{
    class Title : public Base
    {
	public:
		Title();
		virtual ~Title();
		
		//Public Method

		void Update()override;

	private:
		// Private Variable

		GameObject::EnemyMgr* enemyMgr;
		double titleTimer;						// タイトルシーン用のタイマー
		GameObject::Field* field;
		int handle;
    };
}