#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class CTracking;
	class CFixPos;
	class SphColl;
};

namespace GameObject
{
	class Rader;

	class AvoidTracker : public Base
	{
	public:
		AvoidTracker();
		~AvoidTracker();

		// Public Method
		void Update() override;			// 更新
		void Init()override;
		void SetTarget(Base* _TargetObject) {targetObject = _TargetObject; }	// 目標を設定
		void Eliminate();

		void Avoid(int _Dir);
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

		float speed;				// 速度
		Base* targetObject;			// 目標
		XMFLOAT3 forceNorVec3;			// 移動方向の3次単位ベクトル

		// Private Method
		void Spawn();

		// Component Variable
		Component::SphColl* sphColl;	// 球のあたり判定コンポーネント用
		// Component Method
		void OnTriggerEnter(Base* _Other)override;

		Component::CTracking* cTracking;
		Component::CFixPos* cFixPos;
		Component::SphColl* cRadar;

		GameObject::Rader* rader;
	};
}