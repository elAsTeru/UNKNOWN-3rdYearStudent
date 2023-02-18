#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class SphColl;
	class CPlayerMove;
	class CFixPos;
	class PlayerShot;
	class CSpawn;
	class CRotYAimPos;
}

namespace GameObject
{
	class Player : public Base
	{
	public:
		Player();
		~Player();

		void Init()override;

		void AddLife();
		const int& GetLifeNum() { return this->life; }

		// Constant Variable
		const float MaxSpeed;
	private:
		// Private Variable
		enum class State
		{
			Spawn,
			Normal,
			Hit,
			Eliminate
		};
		State state;
		int life;			// 残機

		// 点滅処理
		const int BlinkNum;
		float timeCounter;	// 時間カウンタ
		bool isVisible;		// 可視状態
		int runCounter;		// 実行回数
		uint32_t elimiEfkHandle;	// やられエフェクトハンドル

		// Private Method
		void Update()override;

		// Component Variable
		Component::SphColl* sphColl;
		Component::CPlayerMove* cMover;
		Component::CFixPos* cFixPos;
		Component::PlayerShot* cBullet;
		Component::CSpawn* cSpawn;
		Component::CRotYAimPos* cRotYAimPos;
		// Component Method
		void OnTriggerEnter(Base* _Other) override;

		int efkHandle;
	};
}