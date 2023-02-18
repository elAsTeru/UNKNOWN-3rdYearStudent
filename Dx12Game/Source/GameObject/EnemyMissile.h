#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class SphColl;
	class CRotYAimPos;
	class CMoveForwardY;
}

namespace GameObject
{
	/// <summary>
	/// 指定した＜位置＞に指定した＜速度＞で向かうミサイル
	/// </summary>
	class EnemyMissile : public Base
	{
	public:
		EnemyMissile();
		~EnemyMissile();

		void Start() {}
		void Init()override;

	private:
		// Private Const Variable
		const float RotSpeedMissile;		// ミサイルの回転速度
		const float MoveSpeedMissile;		// ミサイルの移動速度
		const float InvalidCollDuration;	// 初期化後、有効になってから当たり判定無効時間
		const float RotSpeed;
		// Private Variable
		Vector3& targetPos;
		float speed;
		float timeCounter;
		float rot;
		// Private Method
		void Update();

		// Component Variable
		Component::SphColl* sphColl;
		Component::CRotYAimPos* cRotYAimPos;
		Component::CMoveForwardY* cMoveForwardY;
		// Component Method
		void OnTriggerEnter(Base* _Other) override;
	};
}