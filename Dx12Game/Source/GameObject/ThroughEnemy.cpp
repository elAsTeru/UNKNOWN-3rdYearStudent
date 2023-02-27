#include "ThroughEnemy.h"
#include "EfkMgr.h"

namespace GameObject
{
	ThroughEnemy::ThroughEnemy():
		Base(Tag::Enemy, "ThroughEnemy"),
		mode(Mode::Spawn),
		MaxSpawnScale(2),
		SpawnDuration(1),
		ExpanRunNum(4),
		ModelDir(MyMath::ToRadians(270.0f)),
		spawnScale(0),
		speed(7.5f),
		isInvincible(false)
	{
		// あたり判定の追加
		sphColl = AddComponent<Component::SphColl>();
		sphColl->radius = 1.0f;

		this->transform->scale = { 1.5 };
	}

	ThroughEnemy::~ThroughEnemy()
	{
	}

	void ThroughEnemy::Update()
	{
		if (mode == Mode::Spawn)
		{
			Spawn();
		}
		else if(mode == Mode::Normal)
		{
			this->timeCounter += Sys::Timer::GetHitStopTime();
			Move();

			// Z座標が-30以下になったら無効化
			if (this->transform->position.z <= -30 || this->transform->position.z >= 30)
			{
				this->SetActive(false); 
			}

			if (this->transform->position.x <= -50 || this->transform->position.x >= 50)
			{
				this->SetActive(false);
			}
		}

		transform->matrix
			= Matrix::CreateScale(this->transform->scale)
			* Matrix::CreateRotationZ(transform->rotation.z)
			* Matrix::CreateRotationY(transform->rotation.y + ModelDir)
			* Matrix::CreateTranslation(transform->position.x, transform->position.y, transform->position.z);

	}

	void ThroughEnemy::Draw() const
	{
		// 無敵か?状態で表示する物を変更
		if (this->isInvincible)
		{
			XMMATRIX matrix
				= Matrix::CreateScale(2, 2, 3)
				* Matrix::CreateRotationZ(this->timeCounter)
				* Matrix::CreateTranslation(0, 0, -1.5)
				* transform->matrix;
			MyDX::Dx12Wrapper::DrawBasicMesh({ matrix,Res::MeshType::Tail,Res::MaterialType::Purple });
		}
		else
		{
			MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix,Res::MeshType::Reverser,Res::MaterialType::Red });
		}
	}

	void ThroughEnemy::Init()
	{
		mode = Mode::Spawn;
		timeCounter = 0;
		sphColl->isTrigger = true;
		sphColl->isEnable = false;
		if(this->isInvincible)
		{this->sphColl->radius = 1.0f; }
		else
		{ this->sphColl->radius = 0.6f; }
	}

	void ThroughEnemy::Eliminate()
	{
		if(this->isInvincible)	// 無敵なら処理から出る
		{ return; }

		// 死亡エフェクト再生
		Effect::EfkMgr::PlayEffect(Res::EfkType::Explosion, this->transform->position, false);
		// スコアを加算する
		MyObj::Score::AddEliminateNum(Res::ScoreType::Thurough);
		MyObj::Sound::PlaySE(Res::SEType::Eliminate);
		// 自信を非アクティブにする
		SetActive(false);
	}

	void ThroughEnemy::Move()
	{
		// 向きを正規ベクトルにする
		Vector2 forward = MyMath::AngleToVecs2LH(transform->rotation.y);

		// 移動量分移動させる
		transform->position.x += forward.x * speed * Sys::Timer::GetHitStopTime();
		transform->position.z += forward.y * speed * Sys::Timer::GetHitStopTime();
	}

	void ThroughEnemy::Spawn()
	{
		// 時間を計測
		timeCounter += Sys::Timer::GetHitStopTime();

		float interval = SpawnDuration / ExpanRunNum;	// 間隔 = 実行時間 / 実行回数
		// 拡大率を設定
		{
			// 経過時間としてタイムカウンタをコピーしておく
			float elapsedTime = timeCounter;
			// 経過時間が間隔を越えていたら
			while (elapsedTime > interval)
			{
				//間隔の時間分引く
				elapsedTime -= interval;
			}
			// 拡大率を変更する
			spawnScale = { (elapsedTime / interval) * MaxSpawnScale };
		}

		// モードの行動を実行できるようにする
		if (timeCounter >= SpawnDuration)
		{
			mode = Mode::Normal;
			this->Shot();
			timeCounter = 0.0f;
			// あたり判定を有効化
			sphColl->isEnable = true;
		}
	}

	void ThroughEnemy::Shot()
	{
		// 無敵状態なら弾を撃たない
		if (this->isInvincible)
		{
			return;
		}

		// プレイヤーへの向きを取得
		Vector3 dir = GameObject::Mgr::Find("Player")->transform->position - this->transform->position;
		dir.Normalize();

		if (auto bullet = GameObject::Mgr::FindDeactiveObj("EnemyBullet"); bullet)
		{
			bullet->Init();
			bullet->transform->position = this->transform->position;
			bullet->transform->rotation.y = MyMath::Vec2ToAnglesLH(Vector2(dir.x, dir.z));
			bullet->SetActive(true);
		}
	}

	Vector3 ThroughEnemy::MoveForward(const Vector3 _Rotation, const float _Speed)
	{
		// 向きから単位ベクトルを取得
		//UnitVector:単位ベクトル
		{
		}
		// 速度から移動量を求めて返却する
		return {};
	}


	void ThroughEnemy::OnTriggerEnter(Base* _Other)
	{
		if (_Other->name == "Bullet")
		{
			if (!this->isInvincible)
			{
				this->Eliminate();
			}
			else
			{
				MyObj::Sound::PlaySE(Res::SEType::Reflect);
			}
		}
	}
}