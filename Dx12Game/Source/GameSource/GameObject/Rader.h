#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class CSphColl;
};

/// <summary> 避け追尾敵(専用)が使用するプレイヤーの弾の位置を把握するためのオブジェクト </summary>
namespace GameObject
{
	class AvoidTracker;

	class Rader : public Base
	{
	public:
		Rader();
		~Rader(){}

		// Public Method
		void Init()override;
		void Update()override;
		void Draw()const{};
		/// <summary> 親オブジェクトの向きを設定 </summary>
		void SetDir(const XMFLOAT3& _ForceNorVec3) { this->DirVec = _ForceNorVec3; }

		GameObject::AvoidTracker* parentObj;
	private:
		// Private Variable

		XMFLOAT3 DirVec;	// 移動方向の3次単位ベクトル
		float dist;			// レーダーと親の距離

		// Component Variable
		Component::CSphColl* sphColl;	// 球のあたり判定コンポーネント用

		// Component Method
		void OnTriggerEnter(Base* _Other)override;
	};
}