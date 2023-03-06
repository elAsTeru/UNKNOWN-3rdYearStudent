#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class CTracking;
	class CRestrictRect;
	class CSphColl;
};

namespace GameObject
{
	class Rader;

	/// <summary> プレイヤーを追跡し、プレイヤーの弾を避ける敵 </summary>
	class AvoidTracker : public Base
	{
	public:
		AvoidTracker();
		~AvoidTracker(){}

		// Public Method

		void Init()override;			// 初期化
		void Update() override;			// 更新
		void Draw() const override;		// 描画

		/// <summary> プレイヤーの弾を避けるために、左右に移動させる処理 </summary>
		void Avoid(bool _IsLeft);
	private:
		// Private Variable

		const float AvoidSpeed;		// 避け速度
		const float Speed;			// 通常の移動速度
		XMFLOAT3 forceNorVec3;		// 移動方向の3次単位ベクトル

		// Component Variable

		Component::CSphColl* sphColl;			// 球のあたり判定コンポーネント用
		Component::CTracking* cTracking;		// 追跡用コンポーネント
		Component::CRestrictRect* cRestRect;	// フィールドとの当たり判定コンポーネント

		GameObject::Rader* raderObj;			// 弾避け用のオブジェクトを格納する

		// Component Method

		void OnTriggerEnter(Base* _Other)override;
	};
}