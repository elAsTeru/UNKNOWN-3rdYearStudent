#include "Rander.h"
#include <cmath>
#include "Dx12Wrapper.h"
#include "CSphColl.h"
#include "CRestrictRect.h"

namespace GameObject
{
	Rander::Rander():
		Base(Tag::Enemy, "Rander"),
		mode(Mode::Spawn),
		sphColl(AddComponent<Component::CSphColl>()),
		subTrans(AddComponent<Component::CTransform>()),
		cRestRect(AddComponent<Component::CRestrictRect>()),
		MaxSpawnScale(2),
		SpawnDuration(1),
		ExpanRunNum(4),
		spawnScale(0),

		MoveSpeed(15.0f),
		IdolDuration(1.0f),
		ActionTime(0.5f),
		timeCounter(0.0f)
	{
		sphColl->radius = 1.0f;
	}

	Rander::~Rander()
	{
	}

	void Rander::Init()
	{
		mode = Mode::Spawn;
		timeCounter = 0;
		sphColl->isTrigger = false;
		sphColl->isEnable = false;
	}

	void Rander::Update()
	{
		this->cRestRect->Update();

		if (mode == Mode::Spawn)
		{
			Spawn();
		}
		else if(mode == Mode::Normal)
		{
			Move();
		}
		
		// 通常枠メッシュ
		transform->matrix
			= Matrix::CreateTranslation(transform->position.x, transform->position.y, transform->position.z);
		// 通常矢印メッシュ
		subTrans->matrix
			= Matrix::CreateRotationY(MyMath::ToRadians(subTrans->rotation.y))
			* transform->matrix;
	}

	void Rander::Draw() const
	{
		MyDX::Dx12Wrapper::DrawMesh(transform->matrix, Res::MeshType::Rander, Res::MaterialType::Red);
		MyDX::Dx12Wrapper::DrawMesh(subTrans->matrix, Res::MeshType::Arrow , Res::MaterialType::Purple);
	}

	void Rander::Eliminate()
	{
		// スコアを加算する
		Other::Score::AddEliminateNum(Res::ScoreType::Rander);
		MyDX::Tool::Sound::PlaySE(Res::SEType::Eliminate);
		// 自信を非アクティブにする
		SetActive(false);
	}

	const float Rander::CreateRandomDir(int _SplitNum)
	{
		// 360を分割する
		float splitSize = static_cast<float>(360.0f / _SplitNum);
		// 分割数にあったランダムな数値を取得
		int random = std::rand() % _SplitNum;
		// ランダム数値と分割された1つのサイズからラジアン角を求める
		float radDir = (random * splitSize);

		return radDir;
	}

	void Rander::Move()
	{
		timeCounter += Sys::Timer::GetDeltaTime();

		// 行動待機
		if (!isAction)
		{
			// 矢印を回転する
			subTrans->rotation.y += 1000.0f * Sys::Timer::GetDeltaTime();

			// 経過時間が次に動けるまでの時間を過ぎていたら
			if (timeCounter >= IdolDuration)
			{
				isAction = true;
				timeCounter = 0.0f;
				// 進む方向を決定する
				transform->rotation.y = subTrans->rotation.y = CreateRandomDir(4);
			}
		}
		// 行動
		else
		{
			// 向きを正規ベクトルに変換
			double x = std::sin(MyMath::ToRadians(transform->rotation.y));
			double z = std::cos(MyMath::ToRadians(transform->rotation.y));
			float moveValue = timeCounter / ActionTime * MoveSpeed;

			if (timeCounter > ActionTime)
			{
				timeCounter = ActionTime;
				moveValue = timeCounter / ActionTime * MoveSpeed;
			}

			transform->position.x -= static_cast<float>(x * moveValue * Sys::Timer::GetDeltaTime());
			transform->position.z -= static_cast<float>(z * moveValue * Sys::Timer::GetDeltaTime());

			if (timeCounter >= ActionTime)
			{
				isAction = false;
				timeCounter = 0;
			}
		}
	}

	void Rander::Spawn()
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
			spawnScale = {(elapsedTime/interval) * MaxSpawnScale};
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

	void Rander::OnTriggerEnter(Base* _Other)
	{
		if (_Other->tag == Tag::Bullet)
		{ this->Eliminate(); }
	}
}