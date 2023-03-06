#include "EnemyBullet.h"
#include "Dx12Wrapper.h"
#include "MeshMgr.h"
#include "Tool/EfkMgr.h"
#include "CSphColl.h"
#include "CMoveForwardY.h"
#include "CRestrictRect.h"

namespace GameObject
{
	/// <summary>
	/// インスタンス
	/// </summary>
	/// <param name="_X">弾の飛ぶ方向Ⅹ</param>
	/// <param name="_Z">弾の飛ぶ方向Ｙ</param>
	EnemyBullet::EnemyBullet() :
		Base(Tag::Bullet, "EnemyBullet"),
		sphColl(AddComponent<Component::CSphColl>()),
		cMoveForwardY(AddComponent<Component::CMoveForwardY>()),
		cRestRect(AddComponent<Component::CRestrictRect>()),
		RotSpeed(10.0f)
	{
		this->sphColl->radius = 0.5f;
		this->transform->scale = Vector3(1.5f);
		this->cMoveForwardY->speed = 10.0f;		// 移動速度
	}

	EnemyBullet::~EnemyBullet()
	{
	}

	void EnemyBullet::Init()
	{
		this->cRestRect->isEnable = false;
		this->rot = 0.0f;
	}

	void EnemyBullet::Update()
	{
		this->cMoveForwardY->Update();
		
		// フィールド外に出ていないか？ → true : 移動制限がかかっていれば自身を無効化 / false : 移動制限を有効化
		if (this->cRestRect->Update())
		{
			if (this->cRestRect->isEnable == true)
			{
				SetActive(false);
				return;
			}
		}
		else
		{
			this->cRestRect->isEnable = true;
		}
	

		// 弾回転処理
		this->rot += this->RotSpeed * Sys::Timer::GetHitStopTime();
		// 描画情報設定
		this->transform->matrix
			= Matrix::CreateScale(this->transform->scale)
			* Matrix::CreateRotationY(this->transform->rotation.y + this->rot)
			* Matrix::CreateRotationX(this->rot)
			* Matrix::CreateRotationZ(this->rot)
			* Matrix::CreateTranslation(transform->position.x, transform->position.y, transform->position.z);
	}

	void EnemyBullet::Draw() const
	{
		MyDX::Dx12Wrapper::DrawMesh(this->transform->matrix,Res::MeshType::GeoBox,Res::MaterialType::Purple);
	}

	void EnemyBullet::OnTriggerEnter(Base* _Other)
	{
		if (_Other->tag == Tag::Player)
		{
			Effect::EfkMgr::PlayEffect(Res::EfkType::Hit, this->transform->position, false);
		}
	}
}