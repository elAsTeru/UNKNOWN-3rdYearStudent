#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class CLineMove;
	class CGenerator;
}


namespace GameObject
{
	class CubeGenerator : public Base
	{
	public:
		CubeGenerator();
		~CubeGenerator();

		void Start() {}
		void Init()override;
		void Update()override;
		void Draw();
		void SetInterval(const float _Time);

		Component::CLineMove* cLineMove;
	private:
		Component::CGenerator* cGenerator;
	};
}