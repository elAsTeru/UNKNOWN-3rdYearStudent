#include "Title.h"

#include "Dx12Wrapper.h"
#include "Sound.h"

#include "RectMgr.h"
#include "TitleUI.h"
#include "EnemyMgr.h"
#include "Field.h"
#include "CubeGenerator.h"
#include "CLineMove.h"

namespace Scene
{
	GameObject::TitleUI* titleUI = nullptr;

	Title::Title() :
		Base("Title"),
		bgmHandle(-1),
		enemyMgr(nullptr),
		titleTimer(0),
		field(nullptr)
	{
		GameObject::Mgr::DeactivateAll();

		titleUI = new GameObject::TitleUI;

		bgmHandle = MyObj::Sound::Play(10, true, true);	// タイトルBGM再生

		auto rectMgr = static_cast<GameObject::RectMgr*>(GameObject::Mgr::Find("RectMgr"));
		rectMgr->SetActive(true);
		rectMgr->Run();

		enemyMgr = new GameObject::EnemyMgr;
		enemyMgr->Load("Resource/JsonData/EnemyDataTitle.json");

		field = new GameObject::Field;

		// 背景設定
		for (int i = 0; i < 3; ++i)
		{
			auto cubeGen = static_cast<GameObject::CubeGenerator*>(GameObject::Mgr::FindDeactiveObj("CubeGenerator"));
			cubeGen->cLineMove->Init();
			cubeGen->cLineMove->duration = 10.0f;
			cubeGen->cLineMove->type = Component::CLineMove::MoveType::Random;

			// 0~32だと32の部分と別の0の部分が被るので-1する
			cubeGen->cLineMove->minDiv = 0;
			cubeGen->cLineMove->maxDiv = 32 - 1;
			if (i > 1)
			{
				// 0~16だと16の部分と別の0の部分が被るので-1する
				cubeGen->cLineMove->maxDiv = 18 - 1;
			}

			// 開始地点を被らないようにする
			switch (i)
			{
			case 0:
				// 下 // 左から右
				cubeGen->cLineMove->p1 = Vector3(-42, -500, -28);
				cubeGen->cLineMove->p2 = Vector3(42, -500, -28);
				break;
			case 1:
				// 上 // 右から左
				cubeGen->cLineMove->p1 = Vector3(42, -500, 28);
				cubeGen->cLineMove->p2 = Vector3(-42, -500, 28);
				break;
			case 2:
				// 左 // 上から下
				cubeGen->cLineMove->p1 = Vector3(-42, -500, 28);
				cubeGen->cLineMove->p2 = Vector3(-42, -500, -28);
			}
			cubeGen->SetActive(true);
		}

		// カメラを設定
		Vector3 eyePos = { 50, 25, -15 };
		Vector3 targetPos = { -20, -30, 10 };
		Vector3 upward = { -1, 0, 1 };

		MyDX::Dx12Wrapper::SetCameraPos(eyePos);
		MyDX::Dx12Wrapper::SetCameraTargetPos(targetPos);
		MyDX::Dx12Wrapper::SetCameraUpward(upward);
	}

	Title::~Title()
	{
		delete titleUI;
		titleUI = nullptr;

		delete field;
		field = nullptr;

		delete enemyMgr;
		enemyMgr = nullptr;

		MyObj::Sound::Stop(10, bgmHandle);	// タイトルBGM停止
	}

	void Title::Update()
	{
		titleTimer += MySys::Timer::GetDeltaTime();		// 経過時間
		enemyMgr->Update(static_cast<const float>(titleTimer));

		field->Update();
		titleUI->Update();
	}
}