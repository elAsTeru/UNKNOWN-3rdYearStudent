#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class Empty : public Base
	{
	public:
		Empty();
		~Empty();

		void Start() {}
		void Init()override {}
		void Update()override;
		void Draw();
	};
}