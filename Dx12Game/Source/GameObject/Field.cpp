#include "Field.h"
#include "Dx12Wrapper.h"

namespace GameObject
{
	DirectX::XMMATRIX sub{};										// サブfield
	DirectX::XMFLOAT4 color = { 25 / 255.0f, 1, 9 / 255.0f, 1 };	// 色
	const Vector2 Aspect(16, 9);									// アスペクト比

	Field::Field() :
		Base(Tag::BackGround, "Field")
	{
		this->transform->scale = { Aspect.x * 4, 1, Aspect.y * 4 };
		this->transform->matrix = Matrix::CreateScale(this->transform->scale);

		sub = Matrix::CreateScale(Aspect.x * 4.1f, 1, Aspect.y * 4.2f);
	}

	Field::~Field()
	{

	}

	void Field::Update()
	{
		Draw();
	}

	void Field::Draw()const
	{
		MyDX::Dx12Wrapper::DrawRect(this->transform->matrix, color);
		MyDX::Dx12Wrapper::DrawRect(sub, color);
	}
}