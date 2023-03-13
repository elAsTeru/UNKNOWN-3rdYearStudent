#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	/// <summary> フェード等を行う際に使用する板 </summary>
	class Board : public Base
	{
	public:
		Board();
		~Board();

		void Init();
		void Update(){};
		void Draw()const override;

		void SetAlpha(float _Alpha) { this->alpha = _Alpha; }
		const float GetAlpha() { return this->alpha; }
	private:
		// Private Variable
		float alpha;	// 透過度
	};
}