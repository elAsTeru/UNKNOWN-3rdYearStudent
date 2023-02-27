#include "AvoidTracker.h"
#include "EfkMgr.h"

#include <cmath>

#include "CTracking.h"
#include "CFixPos.h"
#include "SphereCollider.h"

#include "Rader.h"

#include "Logger.h"

namespace GameObject
{
	AvoidTracker::AvoidTracker() :
		Base(Tag::Enemy, "AvoidTracker"),
		Speed(4.0f),
		AvoidSpeed(40.0f)
	{
		this->transform->scale = { 1.5f };

		this->sphColl = AddComponent<Component::SphColl>();
		this->sphColl->radius = 1.5f;

		this->cFixPos = AddComponent<Component::CFixPos>();
		this->cTracking = AddComponent<Component::CTracking>();
		this->cTracking->Init(GameObject::Mgr::Find("Player"), this->Speed);

		this->raderObj = new Rader();
		raderObj->parentObj = this;
	}

	AvoidTracker::~AvoidTracker()
	{
		delete raderObj;
		raderObj = nullptr;
	}

	void AvoidTracker::Update()
	{
		this->raderObj->Update();

		// 追尾処理 // 3次元単位ベクトル
		this->forceNorVec3 = this->cTracking->Update();
		this->raderObj->SetForce(this->forceNorVec3);

		// 一度フィールド内に入ったら外に出れないようにする
		if (this->cFixPos->Update() && !this->cFixPos->isEnable)
		{
			this->cFixPos->isEnable = true;
		}

		float modelDir = MyMath::ToRadians(45);
		this->transform->matrix
			= Matrix::CreateScale(this->transform->scale)
			* Matrix::CreateRotationY(transform->rotation.y + modelDir)
			* Matrix::CreateTranslation(transform->position.x, 0, transform->position.z);
	}

	void AvoidTracker::Draw() const
	{
		MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix,Res::MeshType::GeoTriangle,Res::MaterialType::Red });
	}

	void AvoidTracker::Init()
	{
		this->sphColl->isTrigger = false;
		this->raderObj->SetActive(true);
	}

	void AvoidTracker::Avoid(int _Dir)
	{
		Vector3 vec3Avoid;
		// 右に避ける
		if (_Dir == 1)
		{
			// 移動方向の単位ベクトルを270度傾けて移動させる
			vec3Avoid = { this->forceNorVec3.z,0,-this->forceNorVec3.x };
		}
		// 左に避ける
		else
		{
			// 移動方向の単位ベクトルを90度傾けて移動させる
			vec3Avoid = { -this->forceNorVec3.z,0,this->forceNorVec3.x };
		}

		this->transform->position += vec3Avoid * Sys::Timer::GetHitStopTime() * this->AvoidSpeed;
	}

	void AvoidTracker::OnTriggerEnter(Base* _Other)
	{
		if (_Other->name == "Bullet")
		{
			// 死亡エフェクト再生
			Effect::EfkMgr::PlayEffect(Res::EfkType::Explosion, this->transform->position, false);
			// スコアを加算する
			MyObj::Score::AddEliminateNum(Res::ScoreType::AvoidTracker);
			MyObj::Sound::PlaySE(Res::SEType::Eliminate);
			// 自信を非アクティブにする
			this->SetActive(false);
		}
	}
}