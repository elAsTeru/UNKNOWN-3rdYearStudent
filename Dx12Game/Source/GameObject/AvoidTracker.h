#pragma once
// -------------------------
// プレイヤーの弾を避ける敵
// -------------------------
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

		void Init()override;			// 初期化
		void Update() override;			// 更新
		void Draw() const override;		// 描画

		void Avoid(int _Dir);
	private:
		// Private Variable

		const float AvoidSpeed;		// 避け速度
		const float Speed;			// 通常の移動速度
		XMFLOAT3 forceNorVec3;		// 移動方向の3次単位ベクトル

		// Component Variable

		Component::SphColl* sphColl;		// 球のあたり判定コンポーネント用
		Component::CTracking* cTracking;	// 追跡用コンポーネント
		Component::CFixPos* cFixPos;		// フィールドとの当たり判定コンポーネント

		GameObject::Rader* raderObj;		// 弾除け用のオブジェクト

		// Component Method

		void OnTriggerEnter(Base* _Other)override;
	};
}