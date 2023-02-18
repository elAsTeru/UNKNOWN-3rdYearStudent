#include "AvoidTracker.h"
#include "EffekseerMgr.h"

#include <cmath>

#include "CTracking.h"
#include "CFixPos.h"
#include "SphereCollider.h"

#include "Rader.h"

#include "Logger.h"

namespace GameObject
{
	AvoidTracker::AvoidTracker():
		Base(Tag::Enemy, "AvoidTracker"),
		mode(Mode::Spawn),
		MaxSpawnScale(2),
		SpawnDuration(1),
		ExpanRunNum(4),
		spawnScale(0),

		speed(0.0f),
		targetObject(nullptr)
	{
		this->transform->scale = { 1.5f };

		this->sphColl = AddComponent<Component::SphColl>();
		this->sphColl->radius = 1.5f;

		this->cFixPos = AddComponent<Component::CFixPos>();
		this->cTracking = AddComponent<Component::CTracking>();
		this->cTracking->Init(GameObject::Mgr::Find("Player"), 4);

		this->rader = new Rader();
		rader->parentObj = this;
	}
	AvoidTracker::~AvoidTracker()
	{
		delete rader;
		rader = nullptr;
	}
	void AvoidTracker::Update()
	{
		//sphColl->DebugUpdate();

		rader->Update();

		if (this->mode == Mode::Spawn)
		{
			Spawn();
		}
		else if (this->mode == Mode::Normal)
		{
			// 追尾処理 // 3次元単位ベクトル
			this->forceNorVec3 = this->cTracking->Update();
			rader->SetForce(this->forceNorVec3);
		}
		else if (this->mode == Mode::Dead)
		{

		}

		// フィールド外にいる場合位置を補正する
		//this->cFixPos->Update();

		float modelDir = MyMath::ToRadians(45);
		this->transform->matrix
			= Matrix::CreateScale(this->transform->scale)
			* Matrix::CreateRotationY(transform->rotation.y + modelDir)
			* Matrix::CreateTranslation(transform->position.x, 0, transform->position.z);

		MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix,MyRes::MeshType::GeoTriangle,7 });

		//if (this->mode == Mode::Spawn)
		//{
		//	XMMATRIX matrix
		//		= Matrix::CreateScale(spawnScale)
		//		* transform->matrix;

		//	MyDX::Dx12Wrapper::DrawTransMesh({ matrix,MyRes::MeshType::GeoTriangle,2 });
		//}
	}

	void AvoidTracker::Init()
	{
		this->mode = Mode::Spawn;
		this->timeCounter = 0;
		this->sphColl->isTrigger = false;
		//this->sphColl->isEnable = false;
		this->rader->SetActive(true);
	}

	void AvoidTracker::Eliminate()
	{
		// 死亡エフェクト再生
		Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Explosion, this->transform->position, false);
		// スコアを加算する
		MyObj::Score::AddEliminateNum(MyRes::ScoreType::AvoidTracker);
		MyObj::Sound::Play(6, false, true);	// 敵死亡SE
		// 自信を非アクティブにする
		this->SetActive(false);
	}

	void AvoidTracker::Spawn()
	{
		// 時間を計測
		this->timeCounter += MySys::Timer::GetHitStopTime();

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

	void AvoidTracker::Avoid(int _Dir)
	{
		Vector3 vec3Avoid;
		// 右に避ける
		if (_Dir == 1)
		{
			// 移動方向の単位ベクトルを270度傾けて移動させる
			vec3Avoid = { this->forceNorVec3.z,0,-this->forceNorVec3.x };
		}
		// 左に避ける
		else
		{
			// 移動方向の単位ベクトルを90度傾けて移動させる
			vec3Avoid = { -this->forceNorVec3.z,0,this->forceNorVec3.x };
		}

		this->transform->position += vec3Avoid * MySys::Timer::GetHitStopTime() * 40;
	}

	void AvoidTracker::OnTriggerEnter(Base* _Other)
	{
		if (_Other->name == "Bullet")
		{
			this->Eliminate();
		}
	}
}