#include "EnemyBullet.h"

#include "MeshMgr.h"
#include "EfkMgr.h"
#include "SphereCollider.h"
#include "CMoveForwardY.h"
#include "CFixPos.h"

namespace GameObject
{
	/// <summary>
	/// インスタンス
	/// </summary>
	/// <param name="_X">弾の飛ぶ方向Ⅹ</param>
	/// <param name="_Z">弾の飛ぶ方向Ｙ</param>
	EnemyBullet::EnemyBullet() :
		Base(Tag::Bullet, "EnemyBullet"),
		sphColl(AddComponent<Component::SphColl>()),
		cMoveForwardY(AddComponent<Component::CMoveForwardY>()),
		cFixPos(AddComponent<Component::CFixPos>())
	{
		this->sphColl->radius = 0.5f;
	}

	EnemyBullet::~EnemyBullet()
	{
	}

	void EnemyBullet::Init()
	{
		this->cMoveForwardY->speed = 10.0f;
		this->cFixPos->isEnable = false;
		this->transform->scale = { 1.5f };
	}

	void EnemyBullet::Update()
	{
		if (this->GetActive())
		{
			this->cMoveForwardY->Update();

			if (this->cFixPos->Update())
			{
				if (this->cFixPos->isEnable == true)
				{
					SetActive(false);
					return;
				}
			}
			else
			{
				this->cFixPos->isEnable = true;
			}
	

			// 描画情報設定
			float modelDir = MyMath::ToRadians(270);
			transform->matrix
				= Matrix::CreateScale(this->transform->scale)
				* Matrix::CreateRotationY(transform->rotation.y + modelDir)
				* Matrix::CreateTranslation(transform->position.x, transform->position.y, transform->position.z);
		}
	}

	void EnemyBullet::Draw() const
	{
		MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix,Res::MeshType::GeoBox,Res::MaterialType::Purple });
	}

	void EnemyBullet::OnTriggerEnter(Base* _Other)
	{
		if (_Other->tag == Tag::Player)
		{
			Effect::EfkMgr::PlayEffect(Res::EfkType::Hit, this->transform->position, false);
		}
	}
}