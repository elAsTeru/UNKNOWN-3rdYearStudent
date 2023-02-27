#pragma once
// -----------------------------
// ステージの四角を描画
// -----------------------------
#include "GameObjectBase.h"

namespace GameObject
{
	class Field : public Base
	{
	public:
		Field();
		~Field();

		void Start() {}
		void Init()override {}
		void Update()override;
		void Draw()const override;

	private:
		// Private Variable	
		// Private Method
	};
}