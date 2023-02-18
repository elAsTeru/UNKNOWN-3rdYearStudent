﻿#include "Player.h"
#include <cmath>
#include "SphereCollider.h"
#include "CPlayerMove.h"
#include "CFixPos.h"
#include "CPlayerShot.h"
#include "CSpawn.h"
#include "CRotYAimPos.h"

#include "EffekseerMgr.h"

// プレイヤーの残機増加処理はスコアの加算処理内にある

namespace GameObject
{
	Player::Player() :
		Base(Tag::Player, "Player"),
		MaxSpeed(20),
		BlinkNum(5),
		sphColl(AddComponent<Component::SphColl>()),
		cMover(AddComponent<Component::CPlayerMove>()),
		cFixPos(AddComponent<Component::CFixPos>()),
		cBullet(AddComponent<Component::PlayerShot>()),
		cSpawn(AddComponent<Component::CSpawn>()),
		cRotYAimPos(AddComponent<Component::CRotYAimPos>())
	{
		// あたり判定の追加
		this->sphColl->radius = 0.5;
		this->cRotYAimPos->rotSpeed = 1000;
	}

	Player::~Player()
	{
	}

	void Player::Init()
	{
		// Init Variable
		this->state = State::Spawn;
		this->life = 1;
		this->timeCounter = 0;
		this->isVisible = true;
		this->runCounter = 0;
		this->elimiEfkHandle = -1;
		// Init Component
		this->sphColl->isEnable = false;			// 当たり判定無効

		this->cMover->speed = this->MaxSpeed;		// 操作に移動速度を渡す

		this->cSpawn->meshType = MyRes::MeshType::Player;
		this->cSpawn->textureID = 1;
		this->cSpawn->duration = 0.4f;
		this->cSpawn->maxScale = 2;
		this->cSpawn->Run(4);						// 出現処理設定
	}

	void Player::Update()
	{
		// 行動回数が多い順 Normal > Hit > Spawn == Eliminate
		if (this->state == State::Normal)
		{
			// 移動処理と、移動してた場合正面を向く＆エフェクトを出す処理
			if (Vector2 axis = this->cMover->Update(); axis != Vector2::Zero)
			{
				this->cRotYAimPos->aimPos = this->transform->position + Vector3(axis.x, 0, axis.y);
				this->cRotYAimPos->Update();
				efkHandle = Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Track,this->transform->position, false, this->transform->rotation);
			}
			this->cFixPos->Update();	// 移動処理後、フィールドから出てた場合、補正する
			this->cBullet->Update();	// 射撃処理
		}
		else if (this->state == State::Hit)
		{
			// 通常処理も行う
			// 移動処理と、移動してた場合正面を向く処理
			if (Vector2 axis = this->cMover->Update(); axis != Vector2::Zero)
			{
				efkHandle = Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Track, this->transform->position, false, this->transform->rotation);
			}
			this->cFixPos->Update();	// 移動処理後、フィールドから出てた場合、補正する
			this->cBullet->Update();	// 射撃処理

			// ↓↓↓HIT状態の時のみ行う処理
			this->timeCounter += MySys::Timer::GetDeltaTime();
			Input::Mgr::GetPad()->SetVibration(0, 0.4f, 0.4f);

			// 点滅させる
			if (this->timeCounter >= 0.1f)
			{
				this->timeCounter = 0;
				this->isVisible = !this->isVisible;

				// 実行回数分終えたら
				if (++this->runCounter; this->runCounter >= this->BlinkNum)
				{
					Input::Mgr::GetPad()->SetVibration(0, 0, 0);
					runCounter = 0;
					// 全て可視化
					this->isVisible = true;
					// 当たり判定有効化
					this->sphColl->isEnable = true;
					// 通常モードに移行
					this->state = State::Normal;
				}
			}
		}
		else if (this->state == State::Spawn)
		{
			float magnification;	// 出現演出の現在の拡大倍率
			if (magnification = this->cSpawn->Update();magnification == -1)
			{
				// 当たり判定有効化
				this->sphColl->isEnable = true;
				// モード変更
				this->state = State::Normal;
			}
			else if (magnification == 1)
			{
				Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Spawn, this->transform->position, false, this->transform->rotation);
				MyObj::Sound::Play(3, false, true);	// プレイヤーの生成SE
			}
		}
		else if (this->state == State::Eliminate)
		{
			this->timeCounter += MySys::Timer::GetHitStopTime();
			Input::Mgr::GetPad()->SetVibration(0, 0.8f, 0.8f);
			Effect::EffekseerMgr::SetPlaySpeed(this->elimiEfkHandle, MySys::Timer::GetHitStopTime() * 100.0f);
			// 点滅させる
			if (this->timeCounter >= 0.1f)
			{
				this->timeCounter = 0;
				this->isVisible = !isVisible;

				// 実行回数分終えたら
				if (++this->runCounter; this->runCounter >= this->BlinkNum)
				{
					Input::Mgr::GetPad()->SetVibration(0, 0, 0);
					this->runCounter = 0;
					// 全て可視化
					this->isVisible = false;
					this->SetActive(false);
				}
			}
		}

		if (this->isVisible)
		{
			// モデルの向き調整分
			float modelDir = MyMath::ToRadians(270);
			this->transform->matrix
				= Matrix::CreateRotationY(this->transform->rotation.y + modelDir)
				* Matrix::CreateTranslation(this->transform->position);

			MyDX::Dx12Wrapper::DrawBasicMesh({ this->transform->matrix, MyRes::MeshType::Player, 2 });
			MyDX::Dx12Wrapper::SetPlayerPos(this->transform->position);
		}

		XMMATRIX uiMatrix
			= Matrix::CreateScale({ 0.05f })
			* Matrix::CreateTranslation(-0.03f, 0, 0.28f);

		std::wstring lifeWstr = std::to_wstring(this->life);

		if (this->life >= 0)
		{
			// UI仮置き
			MyDX::Dx12Wrapper::Draw2DUI({ uiMatrix,MyRes::MeshType::Board,8 });
			MyDX::Dx12Wrapper::DrawFont({ lifeWstr.c_str(), DirectX::XMFLOAT2(960, 40), {1,1,1,1},{},{},0.5 });
		}
	}

	void Player::AddLife()
	{
		++this->life;
	}

	void Player::OnTriggerEnter(Base* _Other)
	{
		// レーダー以外の敵に当たったら
		if ((_Other->name == "EnemyBullet") || (_Other->name == "EnemyMissile") || ((_Other->tag == Tag::Enemy) && (_Other->name != "Rader")))
		{
			// 連続で判定を受けないようにfalseに
			this->sphColl->isEnable = false;	// 当たり判定無効化
			MyObj::Sound::Play(4, false, true);	// プレイヤー死亡SE

			// 残機を減らして、0以上なら継続
			if (--this->life; life >= 0)
			{
				this->state = State::Hit;
			}
			// そうでなければ終了処理
			else
			{
				MySys::Timer::RunHitStop();		// ヒットストップ処理開始
				this->elimiEfkHandle = Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::PlayerEliminate, this->transform->position, false);
				this->state = State::Eliminate;
			}
		}
	}
}