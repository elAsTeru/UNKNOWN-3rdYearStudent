#include "Select.h"
#include "Tool/Sound.h"
#include "SelectUI.h"
#include "Tool/InputMgr.h"
#include "CubeGenerator.h"
#include "CLineMove.h"

namespace Scene
{
	Select::Select():
		Base("Select")
	{
		GameObject::Mgr::DeactivateAll();


		this->selectUI = new GameObject::SelectUI;

		GameObject::Mgr::Find("RectMgr")->SetActive(false);		// タイトルを飛ばす場合、保留リストに入っているので、強制的に無効化

		MyDX::Tool::Sound::PlaySE(Res::SEType::SelectIn);
		MyDX::Tool::Sound::PlayBGM(Res::BGMType::Select);

		Other::Score::Reset();

		Input::Mgr::GetPad()->SetVibration(0, 0, 0);	// 振動終了


		struct Line
		{
			Vector3 p1;
			Vector3 p2;
		};
		// 開始地点が被らないように値を設定
		Line lines[] =
		{
			{Vector3(-42, -500, -28),Vector3(42, -500, -28)},
			{Vector3(42, -500, 28),Vector3(-42, -500, 28)},
			{Vector3(42, -500, -28),Vector3(42, -500, 28)},
			{Vector3(-42, -500, 28),Vector3(-42, -500, -28)}
		};
		// 背景設定
		for (int i = 0; i < sizeof(lines) / sizeof(Line); ++i)
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

			cubeGen->cLineMove->SetLine(lines[i].p1, lines[i].p2);
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