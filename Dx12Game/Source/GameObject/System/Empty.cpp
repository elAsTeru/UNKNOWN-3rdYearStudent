#include "Empty.h"
#include "Dx12Wrapper.h"

namespace GameObject
{
	Empty::Empty() :
		Base(Tag::Untagged, "Empty")
	{
	}

	Empty::~Empty()
	{

	}

	void Empty::Update()
	{
		Draw();
	}

	void Empty::Draw()
	{
		MyDX::Dx12Wrapper::DrawBasicMesh({ this->transform->matrix, MyRes::MeshType::Cube, 1 });
	}
} 