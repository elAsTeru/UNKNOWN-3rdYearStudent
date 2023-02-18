﻿#include "Shield.h"

#include <cmath>
#include "SphereCollider.h"
#include "CMoveAimPos.h"
#include "CRotYAimPos.h"
#include "Sound.h"

namespace GameObject
{
	Shield::Shield() :
		Base(Tag::Enemy, "Shield"),
		sphColl(AddComponent<Component::SphColl>()),
		cMoveAimPos(AddComponent<Component::CMoveAimPos>()),
		cRotYAimPos(AddComponent<Component::CRotYAimPos>()),
		DurationAnim(1.0f)
	{
		cRotYAimPos->rotSpeed = 500;
	}
	Shield::~Shield()
	{
	}
	void Shield::Update()
	{
		this->forward = this->cMoveAimPos->Update();
		this->cRotYAimPos->aimPos = this->forward + this->transform->position;
		this->cRotYAimPos->Update();

		float modelDir = MyMath::ToRadians(45);

		if (this->isDraw)
		{
			// 拡大後表示
			if (this->isCrack)
			{
				if (this->timeCounter <= this->DurationAnim)
				{
					timeCounter += MySys::Timer::GetHitStopTime();
					this->scaleMag = 1.0f + MyMath::CircularMotion(0.0f, 0.5f, timeCounter, 10.0f);
				}
				else
				{
					this->isDraw = false;
				}
			}
			// 表示後縮小
			else
			{
				if (this->timeCounter <= this->DurationAnim)
				{
					timeCounter += MySys::Timer::GetHitStopTime();
					this->scaleMag = 1.0f + 0.3f - MyMath::CircularMotion(0.0f, 0.5f, timeCounter, 10.0f);
				}
				else
				{
					this->sphColl->isEnable = true;
				}
			}

			if (this->timeCounter <= this->DurationAnim)
			{
				
				this->transform->matrix
					= Matrix::CreateScale(this->transform->scale * this->scaleMag)
					* Matrix::CreateRotationY(this->transform->rotation.y + modelDir)
					* Matrix::CreateTranslation(this->transform->position);
				MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix,MyRes::MeshType::ShieldCrack ,1 });
			}
			else
			{
				// 描画
				this->transform->matrix
					= Matrix::CreateScale(this->transform->scale)
					* Matrix::CreateRotationY(this->transform->rotation.y + modelDir)
					* Matrix::CreateTranslation(this->transform->position);
				MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix,MyRes::MeshType::Tracker ,11 });


				Vector3 p1 = this->transform->position;
				Vector3 p2 = GameObject::Mgr::FindActiveObj("Boss")->transform->position;
				// 2点間の中点
				Vector3 midPos = (p1 + p2) / 2;
				// 2点間の角度
				float dir = std::atan2(p2.z - p1.z, p2.x - p1.x);
				// 2点間の距離
				Vector3 dist3D = { std::fabs(p1.x - p2.x),0,std::fabs(p1.z - p2.z) };
				// 2点間の距離を大きさに
				float vecSize = dist3D.Length();

				// レーザーの範囲に仮置きで板を描画
				XMMATRIX matrix
					= Matrix::CreateScale(vecSize / 2.0f, 1.0f, 0.1f)
					* Matrix::CreateRotationY(-dir)
					* Matrix::CreateTranslation(midPos);
				MyDX::Dx12Wrapper::DrawBasicMesh({ matrix, MyRes::MeshType::Board, 7 });
			}
		}
	}

	void Shield::Init()
	{
		this->transform->position = Vector3(0.0f);
		this->transform->rotation = Vector3(0.0f);
		this->transform->scale = Vector3(1.8f);
		this->timeCounter = this->DurationAnim;
		this->isCrack = false;
		this->isDraw = true;
	}

	void Shield::SetTargetPos(const Vector3 _Pos)
	{
		this->cMoveAimPos->aimPos = _Pos;
	}

	void Shield::SetSpeed(const float _Speed)
	{
		this->cMoveAimPos->speed = _Speed;
	}


	void Shield::Crack()
	{
		this->isCrack = true;
		this->sphColl->isEnable = false;
		this->timeCounter = 0.0f;
	}

	void Shield::Repair()
	{
		this->isCrack = false;
		this->isDraw = true;
		this->timeCounter = 0.0f;
	}

	void Shield::OnTriggerEnter(Base* _Other)
	{
		if (_Other->name == "Bullet")
		{
			MyObj::Sound::Play(15, false, true);
		}
	}
}