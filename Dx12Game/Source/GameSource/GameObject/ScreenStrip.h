#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	/// <summary> 画面上下の帯 </summary>
	class ScreenStrip : public Base
	{
	public:
		ScreenStrip();
		~ScreenStrip();

		void Init();
		void Update(){};
		void Draw()const override;

		void SetDist(float _Dist) { this->dist = _Dist; }
		void SetAlpha(float _Alpha) { this->alpha = _Alpha; }
	
	private:
		// Private Variable
		float dist;		// 画面中央からどれくらい離れているか？
		float alpha;	// 透過度
	};
}