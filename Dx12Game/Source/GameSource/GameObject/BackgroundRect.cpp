#include "BackgroundRect.h"
#include <cmath>
#include "Dx12Wrapper.h"

#include "RectMgr.h"

namespace GameObject
{
	const Vector4 Purple(0.2f, 0.15f, 0.25f, 1.0f);

	BackgroundRect::BackgroundRect() :
		Base(Tag::BackGround, "BackgroundRect")
	{
	}

	BackgroundRect::~BackgroundRect()
	{
	}

	void BackgroundRect::Update()
	{
		// 更新リストに未追加なのでここで描画
		Draw();
	}

	void BackgroundRect::Draw() const
	{
		// 自身のY座標からα値を設定する(0~-の値を0~1の値にする)
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

		Vector4 color(Purple.x, Purple.y, Purple.z, alpha);

		MyDX::Dx12Wrapper::DrawRect(transform->matrix, color);
	}
}