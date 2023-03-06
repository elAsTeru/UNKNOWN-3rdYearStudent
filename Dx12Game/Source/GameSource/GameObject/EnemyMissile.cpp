#include "EnemyMissile.h"
#include "Dx12Wrapper.h"
#include "MeshMgr.h"
#include "CSphColl.h"
#include "Tool/EfkMgr.h"
#include "CSphColl.h"
#include "CRotYAimPos.h"
#include "CMoveForwardY.h"

namespace GameObject
{
	EnemyMissile::EnemyMissile() :
		Base(Tag::Bullet, "EnemyMissile"),
		RotSpeed(5.0f),
		InvalidCollDuration(1.0f),
		sphColl(AddComponent<Component::CSphColl>()),
		cRotYAimPos(AddComponent<Component::CRotYAimPos>()),
		cMoveForwardY(AddComponent<Component::CMoveForwardY>()),
		targetPos(GameObject::Mgr::Find("Player")->transform->position)
	{
		this->transform->scale = Vector3(3.0f);
		this->sphColl->radius = 0.5f;
		this->cRotYAimPos->isRunBeforeFix = false;
		this->cRotYAimPos->rotSpeed = 40.0f;			// 移動方向変更の回転速度
		this->cMoveForwardY->speed = 6.0f;			// 移動速度

		// プレイヤーとの衝突判定
		this->onTriDescs.emplace_back(OnTriDesc("Player", Res::EfkType::Explosion2, Res::SEType::Bomb));
		// プレイヤーとの衝突判定
		this->onTriDescs.emplace_back(OnTriDesc("Boss", Res::EfkType::Explosion2, Res::SEType::Bomb));
		// プレイヤーの弾との衝突判定
		this->onTriDescs.emplace_back(OnTriDesc("Bullet", Res::EfkType::Hit, Res::SEType::Eliminate, Res::ScoreType::Missile));
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
		if (this->timeCounter += Sys::Timer::GetHitStopTime(); this->timeCounter >= this->InvalidCollDuration)
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

		this->rot += this->RotSpeed * Sys::Timer::GetDeltaTime();
		this->subMatrix
			= Matrix::CreateRotationZ(rot)
			* this->transform->matrix;
	}

	void EnemyMissile::Draw() const
	{
		MyDX::Dx12Wrapper::DrawMesh(this->transform->matrix,Res::MeshType::Missile,Res::MaterialType::Red);
		MyDX::Dx12Wrapper::DrawMesh(subMatrix,Res::MeshType::Tail,Res::MaterialType::Red);
	}

	void EnemyMissile::OnTriggerEnter(Base* _Other)
	{
		for (auto desc : this->onTriDescs)
		{
			// 衝突判定に登録されているゲームオブジェクトなら
			// エフェクト、効果音の再生、スコアの加算処理を行い、自身を無効化する。
			if (_Other->name == desc.otherName)
			{
				Effect::EfkMgr::PlayEffect(desc.efkType, this->transform->position, false);
				MyDX::Tool::Sound::PlaySE(desc.seType);
				if (desc.scoreType != Res::ScoreType::End)
				{
					Other::Score::AddEliminateNum(desc.scoreType);
				}
				this->SetActive(false);
				break;
			}
		}
	}
}