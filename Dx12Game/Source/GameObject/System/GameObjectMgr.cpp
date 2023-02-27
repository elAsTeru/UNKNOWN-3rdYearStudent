#include "GameObjectMgr.h"
#include "GameObjectBase.h"
#include "Logger.h"

#include "Player.h"
#include "Bullet.h"
#include "Rander.h"
#include "Reverser.h"
#include "ThroughEnemy.h"
#include "SlowTracker.h"
#include "AvoidTracker.h"
#include "LaserColl.h"
#include "Laser.h"
#include "Boss.h"
#include "Shield.h"
#include "CubeGenerator.h"
#include "BackgroundCube.h"
#include "EnemyBullet.h"
#include "EnemyMissile.h"
#include "RectMgr.h"

namespace GameObject
{
	Mgr* Mgr::singleton = nullptr;

	Mgr::Mgr()
	{
	}

	Mgr::~Mgr()
	{
	}

	void Mgr::OnInit()
	{
		if (!singleton)
		{
			singleton = new Mgr;
			// Make Pool
			{
				CreatePool<GameObject::Player>(1);
				CreatePool<GameObject::Boss>(1);
				CreatePool<GameObject::Shield>(30);
				CreatePool<GameObject::LaserColl>(100);
				CreatePool<GameObject::Laser>(5);
				CreatePool<GameObject::EnemyMissile>(20);
				CreatePool<GameObject::Bullet>(40);
				CreatePool<GameObject::EnemyBullet>(300);
				CreatePool<GameObject::ThroughEnemy>(60);
				CreatePool<GameObject::SlowTracker>(40);
				CreatePool<GameObject::AvoidTracker>(20);
				CreatePool<GameObject::Rander>(4);
				CreatePool<GameObject::Reverser>(6);
				CreatePool<GameObject::CubeGenerator>(4);
				CreatePool<GameObject::BackgroundCube>(400);
				CreatePool<GameObject::RectMgr>(1);

				Update();
			}
		}
	}

	void Mgr::OnTerm()
	{
		if (singleton)
		{
			RemoveAllObject();
			// ���
			delete singleton;
			singleton = nullptr;
		}
	}

	void Mgr::Update()
	{
		// ���ׂẴI�u�W�F�N�g�̍X�V
		//for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		//{
		//	for (auto it = singleton->objects[tag].begin(); it != singleton->objects[tag].end();)
		//	{
		//		auto itr = *it;
		//		if (!itr)
		//		{
		//			continue;
		//		}
		//		//if (itr->GetActive()())
		//		//{

		//		//}
		//		itr->Update();
		//		++it;
		//	}
		//}

		// �S�ẴI�u�W�F�N�g���X�V
		for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		{
			for (auto object : singleton->objects[tag])
			{
				if (object->GetActive())
				{
					object->Update();
				}
			}
		}

		// �ۗ����X�g���̃f�[�^�̒ǉ�
		for (auto pending : singleton->pendingData)
		{
			Tag t = pending->tag;
			singleton->objects[t].emplace_back(pending);
		}
		singleton->pendingData.clear();

		//// �S�Ă̎���ł���A�N�^�[���ꎞ�ۊ�
		//std::vector<GameObjectBase*>deadObjects;
		//for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		//{
		//	for (auto object : singleton->objectData[tag])
		//	{
		//		if (object->gameObjState.GetActivity() == GameObjectState::Activity::IsDead)
		//		{
		//			deadObjects.emplace_back(object);
		//		}
		//	}
		//}

		//// ����ł���A�N�^�[��delete(�z�񂩂���폜)
		//for (auto object : deadObjects)
		//{
		//	delete object;
		//}
		//deadObjects.clear();
	}

	void Mgr::Draw()
	{
		// �S�ẴI�u�W�F�N�g�̕`��
		for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		{
			for (auto object : singleton->objects[tag])
			{
				if (object->GetActive())
				{
					object->Draw();
				}
			}
		}
	}

	void Mgr::AddObject(Base* _Object)
	{
		// �ۗ����X�g�ɒǉ�����
		singleton->pendingData.emplace_back(_Object);
	}

	void Mgr::RemoveObject(Base* _Object)
	{
		// ?�Ώۂ̃I�u�W�F�N�g�͕ۗ��f�[�^���ɂ��邩
		auto iter = std::find(singleton->pendingData.begin(), singleton->pendingData.end(), _Object);
		if (iter != singleton->pendingData.end())
		{
			//�ۗ��f�[�^�����Ƀf�[�^���ړ����āA�f�[�^������
			std::iter_swap(iter, singleton->pendingData.end() - 1);
			singleton->pendingData.pop_back();
			delete _Object;
			_Object = nullptr;
			return;
		}

		// ?�Ώۂ̃I�u�W�F�N�g�͗L���f�[�^���ɂ��邩
		auto tag = _Object->tag;
		iter = std::find(singleton->objects[tag].begin(), singleton->objects[tag].end(), _Object);
		if (iter != singleton->objects[tag].end())
		{
			// �L���f�[�^�����Ƀf�[�^���ړ����āA�f�[�^����
			std::iter_swap(iter, singleton->objects[tag].end() - 1);
			singleton->objects[tag].pop_back();
			delete _Object;
			_Object = nullptr;
		}
	}

	const void Mgr::RemoveAllObject()
	{
		// �L���f�[�^(map)�̍폜(�R���|�[�l���g���폜�����)
		for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		{
			while (!singleton->objects[tag].empty())
			{
				delete singleton->objects[tag].back();
				singleton->objects[tag].pop_back();
			}
		}
		singleton->objects.clear();

		// �ۗ��f�[�^(vector)�̍폜(�R���|�[�l���g���폜�����)
		while (!singleton->pendingData.empty())
		{
			delete singleton->pendingData.back();
			singleton->pendingData.pop_back();
		}
		singleton->pendingData.clear();
	}

	int Mgr::GetActiveNum(const Tag _Tag)
	{
		int counter{};
		for (auto obj : singleton->objects[_Tag])
		{
			// �L���I�u�W�F�N�g����������J�E���g����
			if (obj->GetActive())
			{
				++counter;
			}
		}
		return counter;
	}

	Base* Mgr::FindActiveObj(const std::string _GameObjName)
	{
		for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		{
			for (auto object : singleton->objects[tag])
			{
				if (object->name == _GameObjName)
				{
					if (object->GetActive())
					{
						return object;
					}
				}
			}
		}
		OutputLog("Not found or Not enough pools active : GameObject of that name\n");
		return nullptr;
	}

	Base* Mgr::FindDeactiveObj(const std::string _GameObjName)
	{
		for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		{
			for (auto object : singleton->objects[tag])
			{
				if (object->name == _GameObjName)
				{
					if (!object->GetActive())
					{
						return object;
					}
				}
			}
		}
		OutputLog("Not found or Not enough pools deactive : GameObject of that name\n");
		return nullptr;
	}

	Base* Mgr::Find(const std::string _GameObjName)
	{
		// �o�^����Ă���I�u�W�F�N�g����T��
		for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		{
			for (auto object : singleton->objects[tag])
			{
				if (object->name == _GameObjName)
				{
					return object;
				}
			}
		}

		// �ۗ��I�u�W�F�N�g����T��
		for (auto pending : singleton->pendingData)
		{
			if (pending->name == _GameObjName)
			{
				return pending;
			}
		}

		OutputLog("Not found or Not enough pools deactive : GameObject of that name\n");
		return nullptr;
	}

	void Mgr::DeactivateAll()
	{
		for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		{
			for (auto object : singleton->objects[tag])
			{
				object->SetActive(false);
			}
		}
	}

	void Mgr::DeactivateSomeAll(const std::string _GameObjName)
	{
		while (1)
		{
			auto obj = FindActiveObj(_GameObjName);
			if (obj)
			{
				obj->SetActive(false);
			}
			else
			{
				break;
			}
		}
	}

	template<class T>
	inline void Mgr::CreatePool(uint16_t _Size)
	{
		for (uint16_t i = 0; i < _Size; ++i)
		{
			auto obj = new T;
			obj->SetActive(false);
			AddObject(obj);
		}
	}
}