#include "Boss.h"
#include <cmath>
#include "Dx12Wrapper.h"

#include "CSphColl.h"
#include "CRestrictRect.h"
#include "Shield.h"
#include "CMoveForwardY.h"
#include "CRotYAimPos.h"
#include "CMoveAimPos.h"
#include "CTransformRingBuff.h"
#include "Resource/MaterialType.h"

#include "Laser.h"

#include "State/BossStateBase.h"

namespace GameObject
{
	Boss::Boss():
		Base(Tag::Enemy, "Boss"),
		FirstHp(200),
		ScaleRotShield(1.5f),
		DistMagnification(3.5f),
		ScaleTrackingShield(2.2f),
		DurationShieldCrack(5.0f),
		DurationDamageColor(0.1f),
		sphColl(AddComponent<Component::CSphColl>()),
		cRestRect(AddComponent<Component::CRestrictRect>()),
		cMoveForwardY(AddComponent<Component::CMoveForwardY>()),
		cRotYAimPos(AddComponent<Component::CRotYAimPos>()),
		cMoveAimPos(AddComponent<Component::CMoveAimPos>()),
		cRingBuff(AddComponent<Component::CTransformRingBuff>())
	{
		this->transform->scale = { 2.0f };
		this->sphColl->radius = 1.5f;

		this->statePools.emplace_back(new State::Boss::Phase1);
		this->statePools.emplace_back(new State::Boss::Phase2);
		this->statePools.emplace_back(new State::Boss::Phase3);
		this->statePools.emplace_back(new State::Boss::Phase4);
		this->statePools.emplace_back(new State::Boss::Eliminate);
	}

	Boss::~Boss()
	{}

	void Boss::Init()
	{
		// 変数の初期化
		this->speed = 0.0f;
		this->rotCore = 0.0f;
		this->rotShieldRot = 0.0f;
		this->nowState = State::Boss::StateList();
		this->nextState = this->nowState;
		this->matType = Res::MaterialType::Blue;
		this->damageTimeCounter = 0.0f;
		this->hp = this->FirstHp;
		this->isInvisible = false;

		this->waitTime = 0.0f;

		// ターゲットを設定
		aimPos = GameObject::Mgr::Find("Player");

		ClearRotShields();
		ClearTrackShields();

		// コンポーネントの初期化
		this->cRotYAimPos->rotSpeed = 30.0f;
		this->cRotYAimPos->isRunBeforeFix = false;

		this->cRingBuff->Init();
		this->cRingBuff->SetBuffSize(72);
		this->cRingBuff->interval = 0.02f;
	}

	void Boss::Update()
	{
		//// 通常処理
		// ステート外部からステート変更があったか？
		if (this->nowState != this->nextState)
		{
			this->statePools[static_cast<uint8_t>(this->nextState)]->Enter(this, Sys::Timer::GetDeltaTime());
			this->nowState = this->nextState;
			return;
		}
		// ステート実行
		this->nextState = statePools[static_cast<uint8_t>(this->nowState)]->Update(this, Sys::Timer::GetDeltaTime());
		// ステート内部からステート変更があったか?
		if (this->nowState != this->nextState)
		{
			this->statePools[static_cast<uint8_t>(this->nextState)]->Enter(this, Sys::Timer::GetDeltaTime());
			this->nowState = this->nextState;
		}

		//// 停止処理
		if (this->waitTime > 0.0f)
		{
			this->waitTime -= Sys::Timer::GetHitStopTime();
		}

		//// 追加処理
		this->cRingBuff->Update(Sys::Timer::GetHitStopTime());	// リングバッファの更新
		// 追跡シールドが有効時、シールドがひび割れ状態なら
		if (this->trackingShields.size() && this->trackingShields[0]->IsCrack())
		{
			// 継続時間を経過していたら修復する
			if (this->crackTimeCounter += Sys::Timer::GetHitStopTime(); this->crackTimeCounter >= this->DurationShieldCrack)
			{
				this->trackingShields[0]->Repair();
				this->crackTimeCounter = 0.0f;
				this->isInvisible = true;
			}
		}

		// ダメージカラーから通常カラーに戻す
		if (this->matType == Res::MaterialType::White)
		{
			if (this->damageTimeCounter += Sys::Timer::GetHitStopTime(); this->damageTimeCounter >= this->DurationDamageColor)
			{
				float percentage = this->GetHpPercentage();
				float half = 50.0f;		// 半分
				float quarter = 25.0f;	// ４分の１
				// 体力の割合によって色を変更する
				if (percentage > half)
				{
					this->matType = Res::MaterialType::Blue;
				}
				else if (percentage > quarter)
				{
					this->matType = Res::MaterialType::Yellow;
				}
				else
				{
					this->matType = Res::MaterialType::Red;
				}
				this->damageTimeCounter = 0;
			}
		}


		// コア描画
		this->rotCore += 2 * Sys::Timer::GetHitStopTime();
		this->transform->matrix
			= Matrix::CreateScale(this->transform->scale)
			* Matrix::CreateRotationX(this->rotCore)
			* Matrix::CreateRotationY(this->rotCore)
			* Matrix::CreateRotationZ(this->rotCore)
			* Matrix::CreateTranslation(this->transform->position);
	}

	void Boss::Draw() const
	{
		if (this->nowState != State::Boss::StateList::PHASE1)
		{
			MyDX::Dx12Wrapper::DrawMesh(transform->matrix,Res::MeshType::GeoBox,this->matType);
		}
		else
		{
			MyDX::Dx12Wrapper::DrawMesh(transform->matrix,Res::MeshType::Sphere,this->matType);
		}
	}

	void Boss::ClearLaser()
	{
		if (laser)
		{
			this->laser->Init();
			this->laser->SetActive(false);
			this->laser = nullptr;
		}
	}

	void Boss::NumOrLessTrackingShield(uint8_t _Num)
	{
		for (uint8_t i = 0;trackingShields.size() > _Num; ++i)
		{
			this->trackingShields.back()->SetActive(false);
			this->trackingShields.pop_back();
		}
	}

	bool Boss::Move(const Vector3 _Target,const float _Speed)
	{
		// 停止状態でなければ
		if (waitTime <= 0)
		{
			// もし目標座標とほぼ同じなら、目標座標にしてtrueを返却
			if (this->transform->position == _Target)
			{
				return true;
			}

			// 目標座標を設定する
			this->cMoveAimPos->aimPos = _Target;
			// 移動速度を設定する
			this->cMoveAimPos->speed = _Speed;

			// 移動
			this->cMoveAimPos->Update();
		}

		return false;
	}

	const uint16_t Boss::GetHpPercentage()
	{
		return static_cast<uint16_t>(this->hp / static_cast<float>(this->FirstHp) * 100);
	}

	void Boss::CreateRotShield(const uint8_t _Num, const float _Speed)
	{
		Vector3 spownPos(30.0f, 0.0f, 30.0f);	// 出現位置
		Vector3 targetPos(3.0f, 0.0f, 3.0f);	// 目標座標

		for (uint8_t i = 0; i < _Num; ++i)
		{
			Matrix yRotMat = Matrix::CreateRotationY(MyMath::ToRadians(360.0f / _Num * i));	// シールドの目標位置を円形に設定する
			
			auto shield = static_cast<GameObject::Shield*>(GameObject::Mgr::FindDeactiveObj("Shield"));
			shield->Init();
			shield->transform->scale = Vector3(this->ScaleRotShield);
			shield->GetComponent<Component::CSphColl>()->radius = this->ScaleRotShield;
			shield->transform->position = Vector3::Transform(spownPos, yRotMat);	// 出現位置
			shield->SetTargetPos(Vector3::Transform(targetPos, yRotMat));			// 目標位置
			shield->SetSpeed(_Speed);
			shield->SetActive(true);
			this->rotShields.emplace_back(shield);
		}
	}

	void Boss::RotShields(const float& _Speed)
	{
		Vector3 targetPos = this->transform->position + Vector3(3, 0, 3);	// 目標座標
		this->rotShieldRot += Sys::Timer::GetHitStopTime() * _Speed;		// 移動速度にデルタタイムを乗算

		for (uint8_t i = 0; i < this->rotShields.size(); ++i)
		{
			// シールドの設置位置の回転行列
			Matrix yRotMat = Matrix::CreateRotationY(MyMath::ToRadians(360.0f / this->rotShields.size() * i + this->rotShieldRot));
			// 回転処理
			this->rotShields[i]->SetTargetPos(Vector3::Transform(targetPos, yRotMat));
		}
	}

	void Boss::CreateTrackShield(const uint8_t _Num, const float _Speed)
	{
		Vector3 spownPos(30.0f, 0.0f, 30.0f);	// 出現位置
		Vector3 targetPos(3.0f, 0.0f, 3.0f);	// 目標座標
		float scaleDiff = 0.2f;					// シールド拡大率の差

		for (uint8_t i = 0; i < _Num; ++i)
		{
			// シールドの設置位置の回転行列
			Matrix yRotMat = Matrix::CreateRotationY(MyMath::ToRadians(360.0f / _Num * i)); // シールドの目標位置を円形に設定する

			auto shield = static_cast<GameObject::Shield*>(GameObject::Mgr::FindDeactiveObj("Shield"));
			shield->Init();
			shield->transform->scale = Vector3(this->ScaleTrackingShield - scaleDiff * i);
			shield->GetComponent<Component::CSphColl>()->radius = this->ScaleTrackingShield - scaleDiff * i;
			shield->transform->position = Vector3::Transform(spownPos, yRotMat);	// 出現位置
			shield->SetTargetPos(Vector3::Transform(targetPos, yRotMat));			// 目標位置
			shield->SetSpeed(_Speed);
			shield->SetActive(true);
			this->trackingShields.emplace_back(shield);
		}
	}

	void Boss::FireNormal(const uint8_t _BulletNum)
	{
		// 弾を必要分探索＆向きを渡し有効化
		for (uint8_t i = 0; i < _BulletNum; ++i)
		{
			if (auto bullet = GameObject::Mgr::FindDeactiveObj("EnemyBullet"); bullet)
			{
				bullet->Init();
				bullet->transform->position = this->transform->position;
				bullet->transform->rotation.y = MyMath::ToRadians(360.0f / _BulletNum * i + this->rotShieldRot);
				bullet->SetActive(true);
			}
		}
	}

	void Boss::FireMissile()
	{
		// ミサイル有効化
		if (auto missile = GameObject::Mgr::FindDeactiveObj("EnemyMissile"); missile)
		{
			missile->Init();
			missile->transform->position = this->transform->position;
			Vector3& playerPos = GameObject::Mgr::Find("Player")->transform->position;
			Vector2 missileDir(this->transform->position.x - playerPos.x, this->transform->position.z - playerPos.z);
			missile->transform->rotation.y = MyMath::Vec2ToAnglesLH(missileDir);
			missile->SetActive(true);
		}
	}

	void Boss::ClearRotShields()
	{
		for (auto shield : this->rotShields)
		{
			shield->SetActive(false);
		}
		this->rotShields.clear();
	}

	void Boss::ClearTrackShields()
	{
		for (auto shield : this->trackingShields)
		{
			shield->SetActive(false);
		}
		this->trackingShields.clear();
	}

	void Boss::TrackingShields()
	{
		// 最初のシールドのみ本体を追尾し、
		// ほかは1つ前のシールドを追尾する
		
		// 停止状態でなければ
		if (waitTime <= 0)
		{
			for (uint16_t i = 0; i < this->trackingShields.size(); ++i)
			{
				if (!i)
				{
					this->trackingShields[i]->SetTargetPos(this->transform->position);
				}
				else
				{
					//// 全て頭を追尾
					//Vector3 targetPos = this->trackingShields[0]->transform->position - (this->trackingShields[0]->GetForward() * (DistMagnification * i));
					//this->trackingShields[i]->SetTargetPos(targetPos);

					Component::CTransform target = this->cRingBuff->GetBuff(i * 12);
					this->trackingShields[i]->SetTargetPos(target.position);
				}
			}
		}
	}

	void Boss::ReflectionMoveDir()
	{
		// 現在位置から目標へ向かうtargetDirを作成
		Vector3 targetDir = aimPos->transform->position - this->transform->position;
		targetDir.Normalize();
		// targetDirの方向を向く
		this->transform->rotation.y = MyMath::Vec2ToAnglesLH(Vector2(targetDir.x, targetDir.z));
	}

	void Boss::RotPlayerPos()
	{
		this->cRotYAimPos->aimPos = aimPos->transform->position;
		this->cRotYAimPos->Update();
	}

	void Boss::MoveForward()
	{
		// 停止状態でなければ
		if (waitTime <= 0)
		{
			// 向いている方向に進む処理
			this->cMoveForwardY->Update();
		}
	}

	bool Boss::IsOutsideStage()
	{
		// 壁と当たったか？
		if (this->cRestRect->Update())
		{
			return true;
		}
		return false;
	}

	void Boss::SetSpeed(const float _Speed)
	{
		this->speed = _Speed;
		this->cMoveForwardY->speed = this->speed;
	}

	bool Boss::PutLaserPoint()
	{
		// laserがnullptr
		if (!laser)
		{
			// laserを取得して初期化
			laser = static_cast<GameObject::Laser*>(GameObject::Mgr::FindDeactiveObj("Laser"));
			laser->Init();
			laser->SetParentPos(&this->transform->position);
			laser->SetActive(true);
		}
		
		// 衝突した壁の位置をレーザーに渡して、埋まったら
		if (laser->SetPos(this->transform->position))
		{
			// 上書きできないようにnullptrを入れる
			laser = nullptr;
			return true;
		}
		
		return false;
	}

	void Boss::OnTriggerEnter(Base* _Other)
	{
		if (_Other->name == "LaserColl")
		{
			// シールドを無効化＆自身の当たり判定を有効化＆一定時間停止
			if (this->trackingShields.size())
			{
				this->trackingShields[0]->Crack();
				this->isInvisible = false;
				this->crackTimeCounter = 0.0f;
				this->waitTime = this->DurationShieldCrack;
				this->ClearLaser();
			}
		}

		// 無敵状態なら終了
		if (this->isInvisible)
		{
			return;
		}

		if (_Other->name == "Bullet")
		{
			// HPの減少処理
			this->Damage();
		}
		else if (_Other->name == "EnemyMissile")
		{
			if (this->rotShields.size())
			{
				this->rotShields.back()->SetActive(false);
				this->rotShields.pop_back();
			}
		}
	}

	void Boss::Damage()
	{
		MyDX::Tool::Sound::PlaySE(Res::SEType::Damage);
		// HPが0より多ければ
		if (this->hp > 0)
		{
			// 色を変更
			this->matType = Res::MaterialType::White;
			// HPを減らす
			--this->hp;
		}
	}
}