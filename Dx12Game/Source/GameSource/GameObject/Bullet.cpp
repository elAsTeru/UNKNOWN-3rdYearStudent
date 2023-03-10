#include "Bullet.h"
#include "Dx12Wrapper.h"

#include "MeshMgr.h"
#include "CSphColl.h"
#include "Tool/EfkMgr.h"
#include "ThroughEnemy.h"
#include "CSphColl.h"
#include "CMoveForwardY.h"
#include "CRestrictRect.h"

namespace GameObject
{
	Bullet::Bullet() :
		Base(Tag::Bullet, "Bullet"),
		sphColl(AddComponent<Component::CSphColl>()),
		cMoveForwardY(AddComponent<Component::CMoveForwardY>()),
		cRestRect(AddComponent<Component::CRestrictRect>())
	{
		this->sphColl->radius = 0.3f;
	}

	Bullet::~Bullet()
	{
	}

	void Bullet::Init()
	{
		this->sphColl->isEnable = true;
		this->cMoveForwardY->speed = 40.0f;
		this->transform->scale = { 1.5f };
	}

	void Bullet::Update()
	{
		// 移動処理

		// 移動制限処理

		// 描画情報設定

		this->cMoveForwardY->Update();

		// ステージの枠に当たったら、エフェクトを表示後、自身を無効化する
		if (this->cRestRect->Update())
		{
			Effect::EfkMgr::PlayEffect(Res::EfkType::Hit, this->transform->position, false);
			SetActive(false);
			return;
		}

		// 描画情報設定
		float modelDir = MyMath::ToRadians(270);
		this->transform->matrix
			= Matrix::CreateScale(this->transform->scale)
			* Matrix::CreateRotationY(transform->rotation.y + modelDir)
			* Matrix::CreateTranslation(transform->position.x, transform->position.y, transform->position.z);
	}

	void Bullet::Draw() const
	{
		MyDX::Dx12Wrapper::DrawMesh(transform->matrix,Res::MeshType::Bullet,Res::MaterialType::Orange);
	}

	void Bullet::OnTriggerEnter(Base* _Other)
	{
		// 当たったのがシールドなら弾がはじかれたよう飛ぶ
		if (_Other->name == "Shield" || (_Other->name == "ThroughEnemy" && static_cast<GameObject::ThroughEnemy*>(_Other)->isInvincible ))
		{
			float revDir = 180.0f;	// 跳ね返る弾の基準角度
			uint8_t rangeDir = 50;	// 跳ね返る弾の拡散角度

			float digDir = MyMath::ToDegrees(this->transform->rotation.y) - revDir + (std::rand() % rangeDir) - (std::rand() % rangeDir);
			this->transform->rotation.y = MyMath::ToRadians(digDir);
			this->sphColl->isEnable = false;
		}
		else if (_Other->tag == Tag::Enemy && _Other->name != "Rader")
		{
			Effect::EfkMgr::PlayEffect(Res::EfkType::Hit, this->transform->position, false);
			// 自分を非アクティブに
			SetActive(false);
		}
	}
}