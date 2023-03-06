#pragma once
#include "ComponentBase.h"
#include "DirectXTK12/SimpleMath.h"

namespace Component
{
	class CSphColl :public Base
	{
	public:
		// Public Variable
		bool		isTrigger;					// 衝突処理かめり込みを戻すか
		DirectX::SimpleMath::Vector3*	center;	// 中央
		float		radius;						// 半径

		// Public Method
		void Start() override;
		void DebugUpdate();

		//void SetIsTrigger(const bool _IsTrigger) { isTrigger = _IsTrigger; }
		//void SetCenter(const DirectX::SimpleMath::Vector3& _Center) { center = _Center; }
		//void SetRadius(const float _Radius) { radius = _Radius; }
	};
}