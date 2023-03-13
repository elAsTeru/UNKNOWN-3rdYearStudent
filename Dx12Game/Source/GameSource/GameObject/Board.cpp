#include "Board.h"
#include "Dx12Wrapper.h"

namespace GameObject
{
	Board::Board() :
		Base(Tag::BackGround, "Board")
	{
	}

	Board::~Board()
	{
	}

	void Board::Init()
	{
		this->alpha = 1.0f;
	}

	void Board::Draw()const
	{
		XMMATRIX mat = Matrix::CreateScale(1);
		MyDX::Dx12Wrapper::DrawMesh2D(mat, Res::MeshType::Board, Res::MaterialType::Black, this->alpha);
	}
}