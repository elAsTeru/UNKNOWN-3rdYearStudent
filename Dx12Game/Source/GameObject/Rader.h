#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class SphColl;
};

namespace GameObject
{
	class AvoidTracker;

	class Rader : public Base
	{
	public:
		Rader();
		~Rader();

		// Public Method
		void Update() override;			// 更新
		void Init()override;
		void SetForce(const XMFLOAT3& _ForceNorVec3) { this->forceNorVec3 = _ForceNorVec3; }

		GameObject::AvoidTracker* parentObj;
	private:
		// Private Variable
		// 生成のみ
		XMFLOAT3 forceNorVec3;		// 移動方向の3次単位ベクトル

		// Component Variable
		Component::SphColl* sphColl;	// 球のあたり判定コンポーネント用

		// Component Method
		void OnTriggerEnter(Base* _Other)override;
	};
}