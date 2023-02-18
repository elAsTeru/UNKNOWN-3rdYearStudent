#include "BackgroundRect.h"

#include <cmath>
#include "Dx12Wrapper.h"
#include "RectMgr.h"

namespace GameObject
{
	BackgroundRect::BackgroundRect() :
		Base(Tag::BackGround, "BackgroundRect")
	{
	}

	BackgroundRect::~BackgroundRect()
	{
	}

	void BackgroundRect::Init()
	{
	}

	void BackgroundRect::Update()
	{
	}

	void BackgroundRect::Draw()
	{
		// 自身のY座標からα価を設定する(0~-の値を0~1の値にする)
		float alpha = 1.0f;
		if (parent)
		{
			float maxDepth = parent->pool_size * parent->DistDepth;
			// 0が1,1が0になるように反転する
			float ratioDepth = 1.0f - (-this->transform->position.y / maxDepth);
			alpha = ratioDepth;
		}

		transform->matrix
			= Matrix::CreateScale(transform->scale)
			* Matrix::CreateTranslation(transform->position);

		DirectX::XMFLOAT4 color{ 51 / 255.0f,38 / 255.0f,64 / 255.0f, alpha };

		MyDX::Dx12Wrapper::DrawRect(transform->matrix, color);
	}
}