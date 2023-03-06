#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	/// <summary> ステージの四角を描画 </summary>
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
		const Vector2 Aspect;		// 四角のアスペクト比
		const Vector2 InRect;		// 内側の横、縦幅
		const Vector2 OutRect;		// 外側の横、縦幅

		DirectX::XMMATRIX subMat;	// 外側の四角の行列
	};
}