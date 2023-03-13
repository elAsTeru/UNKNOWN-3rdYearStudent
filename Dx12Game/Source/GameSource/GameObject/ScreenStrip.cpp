#include "ScreenStrip.h"
#include "Dx12Wrapper.h"

namespace GameObject
{
	ScreenStrip::ScreenStrip() :
		Base(Tag::BackGround, "ScreenStrip")
	{
	}

	ScreenStrip::~ScreenStrip()
	{
	}

	void ScreenStrip::Init()
	{
		this->dist = 0.0f;
		this->alpha = 1.0f;
	}

	void ScreenStrip::Draw()const
	{
		XMMATRIX mat = Matrix::CreateTranslation(0, 0, this->dist);
		MyDX::Dx12Wrapper::DrawMesh2D(mat, Res::MeshType::Board, Res::MaterialType::Gray, this->alpha);

		mat = Matrix::CreateTranslation(0, 0, -this->dist);
		MyDX::Dx12Wrapper::DrawMesh2D(mat, Res::MeshType::Board, Res::MaterialType::Gray, this->alpha);
	}
}