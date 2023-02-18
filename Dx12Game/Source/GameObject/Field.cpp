#include "Field.h"
#include "Dx12Wrapper.h"

namespace GameObject
{
	DirectX::XMMATRIX sub{};										// サブfield
	DirectX::XMFLOAT4 color = { 25 / 255.0f, 1, 9 / 255.0f, 1 };	// 色

	Field::Field() :
		Base(Tag::BackGround, "Field")
	{
		this->transform->scale = { 16 * 4, 1, 9 * 4 };
		this->transform->matrix = Matrix::CreateScale(this->transform->scale);

		sub = Matrix::CreateScale(16 * 4.1f, 1, 9 * 4.2f);
	}

	Field::~Field()
	{

	}

	void Field::Update()
	{
		Draw();
	}

	void Field::Draw()
	{
		MyDX::Dx12Wrapper::DrawRect(this->transform->matrix, color);
		MyDX::Dx12Wrapper::DrawRect(sub, color);
	}
}