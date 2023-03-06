#include "GameObjectBase.h"

namespace GameObject
{
	Base::Base(Tag _Tag, std::string _Name) :
		gameObjTag(_Tag),
		gameObjName(_Name),
		isActive(true)
	{

		////���g���I�u�W�F�N�g���X�g�ɒǉ�����
		//GameObjectMgr::AddObject(this);
		
		// �W���R���|�[�l���g
		transform = AddComponent<Component::CTransform>();	// ���W�n�R���|�[�l���g
	}

	Base::~Base()
	{
		//�R���|�[�l���g�̊J��
		for (auto com : componentList)
		{
			if (com)
			{
				delete com;
				com = nullptr;
			}
		}
		componentList.clear();
	}
}