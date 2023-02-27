#include "SlowTracker.h"
#include "EfkMgr.h"

#include <cmath>

#include "CTracking.h"
#include "CFixPos.h"

namespace GameObject
{
	SlowTracker::SlowTracker():
		Base(Tag::Enemy, "SlowTracker"),
		mode(Mode::Spawn),
		MaxSpawnScale(2),
		SpawnDuration(1),
		ExpanRunNum(4),
		spawnScale(0),
		speed(0.0f),
		targetObject(nullptr),
		sphColl(AddComponent<Component::SphColl>()),
		cFixPos(AddComponent<Component::CFixPos>()),
		cTracking(AddComponent<Component::CTracking>())
	{
		this->sphColl->radius = 1.0f;
		this->cTracking->Init(GameObject::Mgr::Find("Player"), 4);
	}
	SlowTracker::~SlowTracker()
	{
	}
	void SlowTracker::Update()
	{
		if (this->mode == Mode::Spawn)
		{
			Spawn();
		}
		else if (this->mode == Mode::Normal)
		{
			// 追尾処理
			this->cTracking->Update();
		}
		else if (this->mode == Mode::Dead)
		{

		}

		// フィールド外にいる場合位置を補正する
		//this->cFixPos->Update();

		float modelDir = MyMath::ToRadians(45);
		this->transform->matrix
			= Matrix::CreateRotationY(transform->rotation.y + modelDir)
			* Matrix::CreateTranslation(transform->position.x, 0, transform->position.z);

	}

	void SlowTracker::Draw() const
	{
		MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix,Res::MeshType::Tracker,Res::MaterialType::Red });
	}

	void SlowTracker::Init()
	{
		this->mode = Mode::Spawn;
		this->timeCounter = 0;
		this->sphColl->isTrigger = false;
		this->sphColl->isEnable = false;
	}

	void SlowTracker::Eliminate()
	{
		// 死亡エフェクト再生
		Effect::EfkMgr::PlayEffect(Res::EfkType::Explosion, this->transform->position, false);
		// スコアを加算する
		MyObj::Score::AddEliminateNum(Res::ScoreType::SlowTracker);
		MyObj::Sound::PlaySE(Res::SEType::Eliminate);
		// 自信を非アクティブにする
		this->SetActive(false);
	}

	void SlowTracker::Spawn()
	{
		// 時間を計測
		this->timeCounter += Sys::Timer::GetHitStopTime();

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
			this->spawnScale = { (elapsedTime / interval) * this->MaxSpawnScale };
		}

		// モードの行動を実行できるようにする
		if (timeCounter >= SpawnDuration)
		{
			this->mode = Mode::Normal;
			this->timeCounter = 0.0f;
			// あたり判定を有効化
			this->sphColl->isEnable = true;
			this->sphColl->isTrigger = false;
		}
	}

	void SlowTracker::OnTriggerEnter(Base* _Other)
	{
		if (_Other->name == "Bullet")
		{
			this->Eliminate();
		}
	}
}