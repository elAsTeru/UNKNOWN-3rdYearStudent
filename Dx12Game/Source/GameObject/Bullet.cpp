#include "Bullet.h"


#include "MeshMgr.h"
#include "SphereCollider.h"
#include "EffekseerMgr.h"
#include "ThroughEnemy.h"

namespace GameObject
{
	/// <summary>
	/// インスタンス
	/// </summary>
	/// <param name="_X">弾の飛ぶ方向Ⅹ</param>
	/// <param name="_Z">弾の飛ぶ方向Ｙ</param>
	Bullet::Bullet() :
		Base(Tag::Bullet, "Bullet")
	{
		// 球状のあたり判定を追加
		this->sphColl = AddComponent<Component::SphColl>();
		this->sphColl->radius = 0.3f;
	}

	Bullet::~Bullet()
	{
	}

	void Bullet::Init()
	{
		this->speed = 0.0f;
		this->sphColl->isEnable = true;
	}

	void Bullet::Update()
	{
		float speed = static_cast<float>(40.0f * MySys::Timer::GetHitStopTime());

		// 向きから移動方向を取得する
		Vector2 vec2 = MyMath::AngleToVecs2LH(transform->rotation.y);
		transform->position.x += vec2.x * speed;
		transform->position.z += vec2.y * speed;

		// ステージの枠に当たったら、エフェクトを表示後、自身を無効化する
		if ((transform->position.x < -MAX_RIGHT - 0.5f || transform->position.x > MAX_RIGHT + 0.5f)
			|| (transform->position.z < -MAX_DEPTH - 0.5f || transform->position.z > MAX_DEPTH + 0.5f))
		{
			Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Hit, this->transform->position, false);
			SetActive(false);
			return;
		}

		// モデルの向き調整分
		float modelDir = MyMath::ToRadians(270);
		transform->matrix
			= Matrix::CreateScale(1.5f, 1.5f, 1.5f)
			* Matrix::CreateRotationY(transform->rotation.y + modelDir)
			* Matrix::CreateTranslation(transform->position.x, transform->position.y, transform->position.z);

		MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix,MyRes::MeshType::Bullet,2 });
	}

	void Bullet::OnTriggerEnter(Base* _Other)
	{
		// 当たったのがシールドなら弾がはじかれたよう飛ぶ
		if (_Other->name == "Shield" || (_Other->name == "ThroughEnemy" && static_cast<GameObject::ThroughEnemy*>(_Other)->isInvincible ))
		{
			float digDir = MyMath::ToDegrees(this->transform->rotation.y) - 180.0f + (std::rand() % 50) - (std::rand() % 50);
			this->transform->rotation.y = MyMath::ToRadians(digDir);
			this->sphColl->isEnable = false;
		}
		else if (_Other->tag == Tag::Enemy)
		{
			// Raderは判定しない
			if (_Other->name == "Rader")
			{ return; }

			Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Hit, this->transform->position, false);
			// 自分を非アクティブに
			SetActive(false);
		}
	}
}