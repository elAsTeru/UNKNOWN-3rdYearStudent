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
		
		void Update()override;
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
		GameObject::EnemyMgr* enemyMgr;
		float timeCounter;						// タイトルシーン用のタイマー
		GameObject::Field* field;
		GameObject::Player* player;
    };
}