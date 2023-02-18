#include "EnemyBullet.h"

#include "MeshMgr.h"
#include "SphereCollider.h"
#include "EffekseerMgr.h"

namespace GameObject
{
	/// <summary>
	/// インスタンス
	/// </summary>
	/// <param name="_X">弾の飛ぶ方向Ⅹ</param>
	/// <param name="_Z">弾の飛ぶ方向Ｙ</param>
	EnemyBullet::EnemyBullet() :
		Base(Tag::Bullet, "EnemyBullet"),
		speed(0.0f)
	{
		// 球状のあたり判定を追加
		this->sphColl = AddComponent<Component::SphColl>();
		this->sphColl->radius = 0.5f;
	}

	EnemyBullet::~EnemyBullet()
	{
	}

	void EnemyBullet::Init()
	{
		this->timeCounter = 0.0f;
	}

	void EnemyBullet::Update()
	{
		if (this->GetActive())
		{
			float speed = static_cast<float>(10.0f * MySys::Timer::GetHitStopTime());

			// 向きから移動方向を取得する
			Vector2 vec2 = MyMath::AngleToVecs2LH(transform->rotation.y);
			transform->position.x += vec2.x * speed;
			transform->position.z += vec2.y * speed;

			if ((transform->position.x < -MAX_RIGHT - 10 || transform->position.x > MAX_RIGHT + 10)
				|| (transform->position.z < -MAX_DEPTH - 10 || transform->position.z > MAX_DEPTH + 10))
			{
				SetActive(false);
				return;
			}

			// モデルの向き調整分
			float modelDir = MyMath::ToRadians(270);
			transform->matrix
				= Matrix::CreateScale(1.5f, 1.5f, 1.5f)
				* Matrix::CreateRotationY(transform->rotation.y + modelDir)
				* Matrix::CreateTranslation(transform->position.x, transform->position.y, transform->position.z);

			MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix,MyRes::MeshType::GeoBox,11 });
		}
	}

	void EnemyBullet::OnTriggerEnter(Base* _Other)
	{
		if (_Other->tag == Tag::Player)
		{
			Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Hit, this->transform->position, false);
		}
	}
}