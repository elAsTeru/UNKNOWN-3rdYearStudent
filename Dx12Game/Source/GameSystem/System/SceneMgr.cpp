#include "SceneMgr.h"

#include "Title.h"
#include "Select.h"
#include "Stage.h"
#include "GameObjectMgr.h"
#include "Logger.h"

namespace Scene
{
	Mgr* Mgr::singleton = nullptr;

	Mgr::Mgr() :
		nowScene(new Title),
		nextScene(nullptr),
		isQuit(false)
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
		}
	}

	void Mgr::OnTerm()
	{
		if (singleton->nowScene)
		{
			delete singleton->nowScene;
			singleton->nowScene = nullptr;
		}
		if (singleton->nextScene)
		{
			delete singleton->nextScene;
			singleton->nextScene = nullptr;
		}
		if (singleton)
		{
			delete singleton;
			singleton = nullptr;
		}
	}

	bool Mgr::Update()
	{
		singleton->nowScene->Update();

		// 次のシーンの変更があるか？
		if (singleton->nextScene)
		{
			delete singleton->nowScene;
			singleton->nowScene = singleton->nextScene;
			singleton->nextScene = nullptr;
		}
		// 現在のシーンが空ならループ終了
		if (singleton->isQuit)
		{
			return false;
		}
		return true; // 正常終了
	}

	void Mgr::LoadScene(std::string _Name)
	{
		if(singleton->isQuit)
		{ return; }

		// 次のシーンが既に決まっている場合
		if (singleton->nextScene)
		{
			// 削除する
			delete singleton->nextScene;
		}

		if (_Name == "Title")
		{
			singleton->nextScene = new Title;
		}
		else if(_Name == "Select")
		{
			singleton->nextScene = new Select;
		}
		else if (_Name == "Stage1")
		{
			Scene::Stage* temp = new Stage;
			temp->SetStage("Resource/JsonData/EnemyDataStage1.json");
			singleton->nextScene = temp;
		}
		else if (_Name == "Stage2")
		{
			Scene::Stage* temp = new Stage;
			temp->SetStage("Resource/JsonData/EnemyDataStage2.json");
			singleton->nextScene = temp;
		}
		else if (_Name == "Stage3")
		{
			Scene::Stage* temp = new Stage;
			temp->SetStage("Resource/JsonData/EnemyDataStage3.json");
			singleton->nextScene = temp;
		}
	}
}