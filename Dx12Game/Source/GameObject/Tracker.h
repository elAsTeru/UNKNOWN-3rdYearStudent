#pragma once
#include "GameObjectBase.h"
#include "SphereCollider.h"

namespace GameObject
{
	class Tracker : public Base
	{
	public:
		Tracker();
		~Tracker();

		// Public Method
		void Update() override;			// 更新
		void Init()override;
		void SetTarget(Base* _TargetObject) {targetObject = _TargetObject; }	// 目標を設定
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

		float speed;					// 速度
		Base* targetObject;	// 目標

		// Private Method
		void FixPosFromStage();		// ステージから出ないようにする
		void Spawn();

		// Component Variable
		Component::SphColl* sphColl;	// 球のあたり判定コンポーネント用
		// Component Method
		void OnTriggerEnter(Base* _Other)override;
	};
}