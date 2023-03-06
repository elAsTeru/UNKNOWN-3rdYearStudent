#include "GameObjectBase.h"

namespace GameObject
{
	Base::Base(Tag _Tag, std::string _Name) :
		gameObjTag(_Tag),
		gameObjName(_Name),
		isActive(true)
	{

		////自身をオブジェクトリストに追加する
		//GameObjectMgr::AddObject(this);
		
		// 標準コンポーネント
		transform = AddComponent<Component::CTransform>();	// 座標系コンポーネント
	}

	Base::~Base()
	{
		//コンポーネントの開放
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