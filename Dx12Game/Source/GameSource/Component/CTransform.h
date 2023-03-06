#pragma once
#include "ComponentBase.h"
#include "MyMath.h"

namespace Component
{
	class CTransform : public Base
	{
	public:
		void Start()override 
		{
			this->position = { 0.0f };
			this->rotation = { 0.0f };
			this->scale = { 1.0f };

			this->matrix
				= Matrix::CreateScale(this->scale);
		}

		Vector3 position;	// 位置
		Vector3 rotation;	// 向き
		Vector3 scale;		// 拡大率

		XMMATRIX matrix;	// 座標系
	};
}