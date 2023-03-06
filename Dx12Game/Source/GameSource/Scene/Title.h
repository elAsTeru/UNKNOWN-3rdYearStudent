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

		/// <summary>
		/// この中にゲームの処理を書く
		/// </summary>
		/// <returns>次のシーンのポインタ</returns>
		virtual void Update()override;

	private:
		// Private Variable
		int bgmHandle;

		GameObject::EnemyMgr* enemyMgr;
		double titleTimer;						// タイトルシーン用のタイマー
		GameObject::Field* field;
		int handle;
    };
}