#include "Reverser.h"
#include "Dx12Wrapper.h"
#include "CRestrictRect.h"

namespace GameObject
{
	Reverser::Reverser():
		Base(Tag::Enemy, "Reverser"),
		cRestRect(AddComponent<Component::CRestrictRect>()),
		mode(Mode::Spawn),
		MaxSpawnScale(2),
		SpawnDuration(1),
		ExpanRunNum(4),
		spawnScale(0),
		speed(10.0f),
		isReverse(false)
	{
		// あたり判定の追加
		sphColl = AddComponent<Component::CSphColl>();
		sphColl->radius = 0.8f;

		// 追加の座標系
		subTrans = AddComponent<Component::CTransform>();
	}

	Reverser::~Reverser()
	{
	}

	void Reverser::Update()
	{
		// 横移動モード時の処理
		if (mode == Mode::Spawn)
		{
			Spawn();
		}
		else if(mode == Mode::Normal)
		{
			Move();
		}
		// 反転動作をする場合
		if (isReverse)
		{
			Reverse();
		}

		// モデルの向き調整分
		float modelDir = MyMath::ToRadians(270);
		transform->matrix
			= Matrix::CreateRotationZ(transform->rotation.z)
			* Matrix::CreateRotationY(transform->rotation.y + modelDir)
			* Matrix::CreateTranslation(transform->position.x, transform->position.y, transform->position.z);

		subTrans->matrix
			= Matrix::CreateRotationZ(subTrans->rotation.z += 0.05f)
			* transform->matrix;

	}

	void Reverser::Draw() const
	{
		MyDX::Dx12Wrapper::DrawMesh(transform->matrix,Res::MeshType::Reverser,Res::MaterialType::Red);
		MyDX::Dx12Wrapper::DrawMesh(subTrans->matrix,Res::MeshType::Tail ,Res::MaterialType::Red);
	}

	void Reverser::Init()
	{
		mode = Mode::Spawn;
		timeCounter = 0;
		sphColl->isTrigger = true;
		sphColl->isEnable = false;
	}

	void Reverser::Eliminate()
	{
		// スコアを加算する
		Other::Score::AddEliminateNum(Res::ScoreType::Reverser);
		MyDX::Tool::Sound::PlaySE(Res::SEType::Eliminate);
		// 自信を非アクティブにする
		SetActive(false);
	}

	void Reverser::Move()
	{
		// 向きを正規ベクトルにする
		Vector2 norVec2 = MyMath::AngleToVecs2LH(transform->rotation.y);
		// 移動量分移動させる
		transform->position.x += norVec2.x * speed * Sys::Timer::GetDeltaTime();
		transform->position.z += norVec2.y * speed * Sys::Timer::GetDeltaTime();

		if (this->cRestRect->Update())
		{
			this->transform->rotation.y += MyMath::ToRadians(180.0f);
			isReverse = true;
		}
	}

	void Reverser::Reverse()
	{
		// 回転させる
		transform->rotation.z += 0.1f;
		// 360度回転したら終了
		if (transform->rotation.z >= (180 * 3.14159265 / 180))
		{
			isReverse = false;
			transform->rotation.z = 0;
		}
	}

	void Reverser::Spawn()
	{
		// 時間を計測
		timeCounter += Sys::Timer::GetDeltaTime();

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
			timeCounter = 0.0f;
			// あたり判定を有効化
			sphColl->isEnable = true;
		}
	}

	Vector3 Reverser::MoveForward(const Vector3 _Rotation, const float _Speed)
	{
		// 向きから単位ベクトルを取得
		//UnitVector:単位ベクトル
		{
		}
		// 速度から移動量を求めて返却する
		return {};
	}


	void Reverser::OnTriggerEnter(Base* _Other)
	{
		if (_Other->tag == Tag::Bullet)
		{ this->Eliminate(); }
	}
}