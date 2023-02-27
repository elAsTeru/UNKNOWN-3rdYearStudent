#include "Select.h"
#include "Sound.h"
#include "SelectUI.h"
#include "InputMgr.h"
#include "CubeGenerator.h"
#include "CLineMove.h"

namespace Scene
{
	Select::Select():
		Base("Select"),
		bgmHandle(-1)
	{
		GameObject::Mgr::DeactivateAll();


		this->selectUI = new GameObject::SelectUI;

		GameObject::Mgr::Find("RectMgr")->SetActive(false);		// タイトルを飛ばす場合、保留リストに入っているので、強制的に無効化

		MyObj::Sound::PlaySE(Res::SEType::SelectIn);
		MyObj::Sound::PlayBGM(Res::BGMType::Select);

		MyObj::Score::Reset();

		Input::Mgr::GetPad()->SetVibration(0, 0, 0);	// 振動終了

		for (int i = 0; i < 4; ++i)
		{
			auto cubeGen = static_cast<GameObject::CubeGenerator*>(GameObject::Mgr::FindDeactiveObj("CubeGenerator"));
			cubeGen->cLineMove->Init();
			cubeGen->cLineMove->duration = 10.0f;
			cubeGen->cLineMove->type = Component::CLineMove::MoveType::Random;
			if (i <= 1)
			{
				// 0~32だと32の部分と別の0の部分が被るので-1する
				cubeGen->cLineMove->minDiv = 0;
				cubeGen->cLineMove->maxDiv = 32 - 1;
			}
			else
			{
				// 0~16だと16の部分と別の0の部分が被るので-1する
				cubeGen->cLineMove->minDiv = 0;
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
				// 右 // 下から上
				cubeGen->cLineMove->p1 = Vector3(42, -500, -28);
				cubeGen->cLineMove->p2 = Vector3(42, -500, 28);
				break;
			case 3:
				// 左 // 上から下
				cubeGen->cLineMove->p1 = Vector3(-42, -500, 28);
				cubeGen->cLineMove->p2 = Vector3(-42, -500, -28);
			}
			cubeGen->SetActive(true);
		}
	}

	Select::~Select()
	{
		delete this->selectUI;
	}

	void Select::Update()
	{
		// カメラ設定
		MyDX::Dx12Wrapper::SetCameraPos({ 0, 60.0f, 0 });
		MyDX::Dx12Wrapper::SetCameraTargetPos({ 0, -30.0f, 0 });
		MyDX::Dx12Wrapper::SetCameraUpward({ 0, 0, 1 });
		// ディレクショナルライト設定
		Vector3 dir(0, -1, 0);
		MyDX::Dx12Wrapper::SetDirLightDir(dir);

		selectUI->Update();
		//menuUI->Update();
	}
}