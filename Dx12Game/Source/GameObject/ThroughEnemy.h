#pragma once
#include "GameObjectBase.h"
#include "SphereCollider.h"

namespace GameObject
{
	class ThroughEnemy : public Base
	{
	public:
		ThroughEnemy();
		~ThroughEnemy();

		// Public Variable
		bool isInvincible;		// ?無敵か?

		// Public Method
		void Init()override;
		void Update()override;
		void Draw()const override;
		void Eliminate();

	private:
		enum class Mode : unsigned char
		{
			Spawn,
			Normal,
			Dead
		};
		Mode mode;
		// Private Variable
		// 共有
		float timeCounter;			// 時間計測計
		// 生成のみ
		const float MaxSpawnScale;	// 生成用の最大拡大率
		const float SpawnDuration;	// 生成継続時間
		const float ExpanRunNum;	// 拡大実行回数
		float spawnScale;			// 生成用の拡大率

		float speed;

		const float ModelDir;		// モデルの向き

		// Private Method
		void Move();
		void Spawn();
		void Shot();


		/// <summary>
		/// 向きと設定から、移動した場合の座標を取得する
		/// </summary>
		Vector3 MoveForward(const Vector3 _Rotation, const float _Speed);		// 向いている方向に移動する処理 // 未実装

		// Component Variable
		Component::SphColl* sphColl;
		void OnTriggerEnter(Base* _Other) override;
	};
}