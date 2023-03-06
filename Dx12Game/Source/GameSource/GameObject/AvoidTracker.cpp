#include "AvoidTracker.h"
#include <cmath>
#include "Dx12Wrapper.h"
#include "Tool/EfkMgr.h"

#include "CTracking.h"
#include "CRestrictRect.h"
#include "CSphColl.h"

#include "Rader.h"

namespace GameObject
{
	AvoidTracker::AvoidTracker() :
		Base(Tag::Enemy, "AvoidTracker"),
		sphColl(AddComponent<Component::CSphColl>()),
		cRestRect(AddComponent<Component::CRestrictRect>()),
		cTracking(AddComponent<Component::CTracking>()),
		Speed(4.0f),
		AvoidSpeed(40.0f)
	{
		this->transform->scale = Vector3(1.5f);
		this->sphColl->radius = 1.5f;

		// 追跡するオブジェクトの名前を設定
		this->cTracking->Init(GameObject::Mgr::Find("Player"), this->Speed);
	}

	void AvoidTracker::Init()
	{
		this->cRestRect->isEnable = false;
		this->sphColl->isTrigger = false;
		// 弾を避けるためのオブジェクトを使用する。thisを親として設定する。
		this->raderObj = static_cast<GameObject::Rader*>(GameObject::Mgr::FindDeactiveObj("Rader"));
		this->raderObj->parentObj = this;
		this->raderObj->SetActive(true);
	}

	void AvoidTracker::Update()
	{
		// 追跡処理と移動方向をレーダーオブジェクトに渡す。
		this->forceNorVec3 = this->cTracking->Update();
		this->raderObj->SetDir(this->forceNorVec3);

		// 一度フィールド内に入ったら外に出れないようにする
		if (this->cRestRect->Update() && !this->cRestRect->isEnable)
		{
			this->cRestRect->isEnable = true;
		}

		// 描画情報を設定
		float modelDir = MyMath::ToRadians(45.0f);
		this->transform->matrix
			= Matrix::CreateScale(this->transform->scale)
			* Matrix::CreateRotationY(this->transform->rotation.y + modelDir)
			* Matrix::CreateTranslation(this->transform->position.x, 0, this->transform->position.z);
	}

	void AvoidTracker::Draw() const
	{
		MyDX::Dx12Wrapper::DrawMesh(this->transform->matrix,Res::MeshType::GeoTriangle,Res::MaterialType::Red);
	}

	void AvoidTracker::Avoid(bool _IsLeft)
	{
		Vector3 vec3Avoid;

		// 左に避ける為に、移動方向のベクトルYをを90度回転させる
		if (_IsLeft == true)
		{
			// 移動方向の単位ベクトルを90度傾けて移動させる
			vec3Avoid = { -this->forceNorVec3.z, 0, this->forceNorVec3.x };
		}
		// 右に避ける為に、移動方向のベクトルYを270度回転させる
		else
		{
			// 移動方向の単位ベクトルを270度傾けて移動させる
			vec3Avoid = { this->forceNorVec3.z, 0, -this->forceNorVec3.x };
		}

		// 移動させる
		this->transform->position += vec3Avoid * Sys::Timer::GetHitStopTime() * this->AvoidSpeed;
	}

	void AvoidTracker::OnTriggerEnter(Base* _Other)
	{
		if (_Other->name == "Bullet")
		{
			// レーダーを無効化
			this->raderObj->SetActive(false);
			// スコアを加算する
			Other::Score::AddEliminateNum(Res::ScoreType::AvoidTracker);
			// 死亡エフェクト、効果音再生
			Effect::EfkMgr::PlayEffect(Res::EfkType::Explosion, this->transform->position, false);
			MyDX::Tool::Sound::PlaySE(Res::SEType::Eliminate);
			// 自信を非アクティブにする
			this->SetActive(false);
		}
	}
}