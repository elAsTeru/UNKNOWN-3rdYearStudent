#include "SphereCollider.h"
#include "ColliderMgr.h"
#include "GameObjectBase.h"
#include "Dx12Wrapper.h"

namespace Component
{
	void SphColl::Start()
	{
		Sys::ColliderMgr::AddCollider(this);
		this->isTrigger = true;
		this->center = &parent->transform->position;
		this->radius = 1;
	}

	void SphColl::DebugUpdate()
	{
		XMMATRIX matrix
			= Matrix::CreateScale({ this->radius })
			* Matrix::CreateTranslation(*this->center);

		MyDX::Dx12Wrapper::DrawBasicMesh({ matrix, Res::MeshType::Sphere, Res::MaterialType::Green });
	}
}