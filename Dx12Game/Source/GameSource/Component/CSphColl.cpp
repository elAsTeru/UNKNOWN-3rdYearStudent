#include "CSphColl.h"
#include "ColliderMgr.h"
#include "GameObjectBase.h"
#include "Dx12Wrapper.h"

namespace Component
{
	void CSphColl::Start()
	{
		Sys::ColliderMgr::AddCollider(this);
		this->isTrigger = true;
		this->center = &parent->transform->position;
		this->radius = 1;
	}

	void CSphColl::DebugUpdate()
	{
		DirectX::XMMATRIX matrix
			= Matrix::CreateScale({ this->radius })
			* Matrix::CreateTranslation(*this->center);

		MyDX::Dx12Wrapper::DrawMesh(matrix, Res::MeshType::Sphere, Res::MaterialType::Green);
	}
}