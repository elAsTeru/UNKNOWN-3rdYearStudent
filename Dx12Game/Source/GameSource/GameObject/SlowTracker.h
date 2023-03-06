#pragma once
#include "GameObjectBase.h"
#include "CSphColl.h"

namespace Component
{
	class CTracking;
	class  CRestrictRect;
};

namespace GameObject
{
	class SlowTracker : public Base
	{
	public:
		SlowTracker();
		~SlowTracker();

		// Public Method
		void Init()override;
		void Update() override;			// 更新
		void Draw()const override;
		void SetTarget(Base* _TargetObject) {targetObject = _TargetObject; }	// 目標を設定
		void Eliminate();
	private:
		enum class Mode : unsigned char
		{
			Spawn,
			Normal
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
		void Spawn();

		// Component Variable
		Component::CSphColl* sphColl;	// 球のあたり判定コンポーネント用
		// Component Method
		void OnTriggerEnter(Base* _Other)override;

		Component::CTracking* cTracking;
		Component::CRestrictRect* cRestRect;
	};
}