#include "Stage.h"

#include "Dx12Wrapper.h"
#include "Tool/Sound.h"
#include "Tool/InputMgr.h"

#include "RectMgr.h"
#include "EnemyMgr.h"
#include "Field.h"
#include "Player.h"

#include "CubeGenerator.h"
#include "CLineMove.h"
#include "EnumIterator.h"

namespace Scene
{
	Stage::Stage():
		Base("Stage"),
		bgmHandle(-1),
		enemyMgr(nullptr),
		timeCounter(0),
		field(nullptr),
		state(State::In)
	{
		GameObject::Mgr::DeactivateAll();

		MyDX::Tool::Sound::PlayBGM(Res::BGMType::Game);

		auto rectMgr = static_cast<GameObject::RectMgr*>(GameObject::Mgr::Find("RectMgr"));
		rectMgr->SetActive(true);
		rectMgr->Run();
		
		enemyMgr = new GameObject::EnemyMgr;

		field = new GameObject::Field;

		player = static_cast<GameObject::Player*>(GameObject::Mgr::Find("Player"));
		player->Init();
		player->transform->position = {};
		player->transform->rotation = {};
		player->SetActive(true);

		for (int i = 0; i < 4; ++i)
		{
			auto cubeGen = static_cast<GameObject::CubeGenerator*>(GameObject::Mgr::FindDeactiveObj("CubeGenerator"));
			cubeGen->cLineMove->Init();
			cubeGen->cLineMove->duration = 10.0f;
			//cubeGen->SetInterval(0.2f);	// 順列は0.5、ランダムは0.2
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

		// ディレクショナルライト設定
		Vector3 dir(0, -1, 0);
		MyDX::Dx12Wrapper::SetDirLightDir(dir);
	}

	Stage::~Stage()
	{
		delete field;
		field = nullptr;

		delete enemyMgr;
		enemyMgr = nullptr;
	}

	void Stage::Update()
	{
		this->timeCounter += Sys::Timer::GetDeltaTime();		// 経過時間

		if (this->state == State::In)
		{
			// フェードイン
			this->state = State::Game;
		}
		else if (this->state == State::Game)
		{
			if ((Input::Mgr::GetPadLog().back == DirectX::GamePad::ButtonStateTracker::RELEASED)
				|| (Input::Mgr::GetKeyLog().IsKeyReleased(DirectX::Keyboard::Escape)))
			{

				Scene::Mgr::LoadScene("Select");
			}

			if (!this->player->GetActive())
			{
				this->state = State::Result;
			}
			// 現在のフェーズの敵が全てで終わったら
			else if (enemyMgr->Update(static_cast<const float>(this->timeCounter)))
			{
				// 演出がないので代わりに敵が全ていなくなったら
				if (!GameObject::Mgr::GetActiveNum(Tag::Enemy))
				{
					this->timeCounter = 0;
					// 背景を動かす
					static_cast<GameObject::RectMgr*>(GameObject::Mgr::Find("RectMgr"))->Run();
					// 現在のフェーズがラストで次のがない場合
					if (!enemyMgr->Next())
					{
						GameObject::Mgr::DeactivateSomeAll("EnemyBullet");
						GameObject::Mgr::DeactivateSomeAll("EnemyMissile");

						this->state = State::Result;
					}
				}
			}

			// プレイヤーの追従(カメラオブジェクト未実装)
			auto& playerPos = player->transform->position;
			Vector3 cameraPos{ playerPos.x / 6.0f,54.0f ,playerPos.z / 4.0f};
			MyDX::Dx12Wrapper::SetCameraPos(cameraPos);
			MyDX::Dx12Wrapper::SetCameraTargetPos({ cameraPos.x,0,cameraPos.z});

			MyDX::Dx12Wrapper::SetCameraUpward({ 0, 0, 1 });

			Other::Score::DrawScore();	// 右上のスコア表示

			// 画面下のフェーズ表示
			uint8_t phaseNum = enemyMgr->GetClearPhaseNum();
			typedef EnumIterator<Res::MaterialType, Res::MaterialType::Phase1, Res::MaterialType::Phase4>typeItr;
			typeItr().Incriment(phaseNum);
			XMMATRIX phaseMat
				= Matrix::CreateScale(0.05f)
				* Matrix::CreateTranslation(0.0f, 0.0f, -0.32f);
			MyDX::Dx12Wrapper::DrawMesh2D(phaseMat,Res::MeshType::Board,*typeItr(), 1.0f);
			
		}
		else if (this->state == State::Result)
		{
			//Sys::Timer::RunHitStop();	// 敵・プレイヤーの動作を停止させる

			Other::Score::CalcResult(enemyMgr->GetClearPhaseNum(), player->GetLifeNum());
			Other::Score::DrawResult();
		}
		else // this->stae == State::Out
		{
			// フェードアウト
		}

		field->Update();
	}
	
	void Stage::SetStage(std::string _FilePath)
	{
		enemyMgr->Load(_FilePath);
	}
}