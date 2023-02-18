#include "EnemyMissile.h"

#include "MeshMgr.h"
#include "SphereCollider.h"
#include "EffekseerMgr.h"
#include "SphereCollider.h"
#include "CRotYAimPos.h"
#include "CMoveForwardY.h"

namespace GameObject
{
	EnemyMissile::EnemyMissile() :
		Base(Tag::Bullet, "EnemyMissile"),
		RotSpeedMissile(40.0f),
		MoveSpeedMissile(6.0f),
		InvalidCollDuration(1.0f),
		RotSpeed(5.0f),
		sphColl(AddComponent<Component::SphColl>()),
		cRotYAimPos(AddComponent<Component::CRotYAimPos>()),
		cMoveForwardY(AddComponent<Component::CMoveForwardY>()),
		targetPos(GameObject::Mgr::Find("Player")->transform->position)
	{
		this->transform->scale = Vector3(3.0f);
		this->sphColl->radius = 0.5f;
		this->cRotYAimPos->isRunBeforeFix = false;
		this->cRotYAimPos->rotSpeed = RotSpeedMissile;
		this->cMoveForwardY->speed = MoveSpeedMissile;
	}

	EnemyMissile::~EnemyMissile()
	{
	}

	void EnemyMissile::Init()
	{
		this->sphColl->isEnable = false;
		this->timeCounter = 0.0f;
		this->rot = 0.0f;
	}

	void EnemyMissile::Update()
	{
		if (this->timeCounter += MySys::Timer::GetHitStopTime(); this->timeCounter >= this->InvalidCollDuration)
		{
			this->sphColl->isEnable = true;
		}

		// 目標の方向を向く処理
		this->cRotYAimPos->aimPos = this->targetPos;
		this->cRotYAimPos->Update();

		// Yの角度から向いている方向に進む処理
		this->cMoveForwardY->Update();

		// 描画処理
		float modelDir = MyMath::ToRadians(270);
		transform->matrix
			= Matrix::CreateScale(this->transform->scale)
			* Matrix::CreateRotationY(this->transform->rotation.y + modelDir)
			* Matrix::CreateTranslation(this->transform->position);
		MyDX::Dx12Wrapper::DrawBasicMesh({ this->transform->matrix,MyRes::MeshType::Missile,7 });

		Matrix subMat
			= Matrix::CreateRotationZ(rot += this->RotSpeed * MySys::Timer::GetDeltaTime())
			* this->transform->matrix;
		MyDX::Dx12Wrapper::DrawBasicMesh({ subMat,MyRes::MeshType::Tail,7 });
	}

	void EnemyMissile::OnTriggerEnter(Base* _Other)
	{
		// プレイヤーかプレイヤーの弾にhitしたら
		if (_Other->tag == Tag::Player)
		{
			Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Explosion2, this->transform->position, false);
			MyObj::Sound::Play(14, false, true);
			// 自分を非アクティブに
			SetActive(false);
		}
		else if (_Other->name == "Boss")
		{
			Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Explosion2, this->transform->position, false);
			MyObj::Sound::Play(14, false, true);
			// 自分を非アクティブに
			SetActive(false);
		}
		else if (_Other->name == "Bullet")
		{
			// スコアを加算する
			MyObj::Score::AddEliminateNum(MyRes::ScoreType::Missile);
			Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Hit, this->transform->position, false);
			MyObj::Sound::Play(6, false, true);
			// 自分を非アクティブに
			SetActive(false);
		}
	}
}