#pragma once
#include "SceneBase.h"

namespace GameObject
{
	class Base;
	class EnemyMgr;
	class Field;
	class Player;
}

namespace Scene
{
    class Stage : public Base
    {
	public:
		Stage();
		virtual ~Stage();
		
		//Public Method

		/// <summary>
		/// この中にゲームの処理を書く
		/// </summary>
		/// <returns>次のシーンのポインタ</returns>
		virtual void Update()override;
		void SetStage(std::string _FilePath);
	private:
		enum class State
		{
			In,
			Game,
			Result,
			Out
		};
		State state;

		// Private Variable
		int bgmHandle;
		GameObject::EnemyMgr* enemyMgr;
		float timeCounter;						// タイトルシーン用のタイマー
		GameObject::Field* field;
		GameObject::Player* player;
    };
}