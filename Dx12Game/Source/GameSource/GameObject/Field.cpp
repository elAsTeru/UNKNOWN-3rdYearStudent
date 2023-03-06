#include "Field.h"
#include "Dx12Wrapper.h"

namespace GameObject
{
	const DirectX::XMFLOAT4 Green(25 / 255.0f, 1, 9 / 255.0f, 1);

	Field::Field() :
		Base(Tag::BackGround, "Field"),
		Aspect(16,9),
		InRect(this->Aspect * 4.0f),
		OutRect(this->Aspect.x * 4.1f, this->Aspect.y * 4.2f)
	{
		// 内側の四角の描画設定
		this->transform->scale = { this->InRect.x,1,this->InRect.y };
		this->transform->matrix = Matrix::CreateScale(this->transform->scale);

		// 外側の四角の描画設定
		this->subMat = Matrix::CreateScale(this->OutRect.x, 1, this->OutRect.y);
	}

	Field::~Field()
	{

	}

	void Field::Update()
	{
		// 更新リストに未追加なのでここで描画
		Draw();
	}

	void Field::Draw()const
	{
		MyDX::Dx12Wrapper::DrawRect(this->transform->matrix, Green);
		MyDX::Dx12Wrapper::DrawRect(this->subMat, Green);
	}
}