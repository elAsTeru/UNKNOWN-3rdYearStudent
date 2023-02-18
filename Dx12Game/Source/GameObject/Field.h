#pragma once
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
		void Draw();

	private:
		// Private Variable	
		// Private Method
	};
}