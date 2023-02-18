#include "Tracker.h"

#include <cmath>

namespace GameObject
{
	Tracker::Tracker():
		Base(Tag::Enemy, "Tracker"),
		mode(Mode::Spawn),
		MaxSpawnScale(2),
		SpawnDuration(1),
		ExpanRunNum(4),
		spawnScale(0),

		speed(0.0f),
		targetObject(nullptr)
	{
		transform->position = DirectX::SimpleMath::Vector3(std::rand()%100 * 0.1f + 10, 0.0f, std::rand()%100 *0.1f);
		sphColl = AddComponent<Component::SphColl>();
		sphColl->radius = 0.6f;
	}
	Tracker::~Tracker()
	{
	}
	void Tracker::Update()
	{
		if (mode == Mode::Spawn)
		{
			FixPosFromStage();
			Spawn();
		}
		else if (mode == Mode::Normal)
		{
			// ターゲットがいれば
			if (targetObject)
			{
				FixPosFromStage();
				//TrackingMove();
				Vector3 targetPos = targetObject->transform->position;
				Vector3 dist = targetPos - transform->position;
				if (!(dist.x == 0 && dist.z == 0))
				{
					// 移動量の正規化
					Vector3 nor;
					nor.Normalize(dist);
					// 移動処理
					transform->position += nor * MySys::Timer::GetDeltaTime() * 8;
				}
				// 移動方向の角度を取得
				transform->rotation.y = static_cast<float>(MyMath::Vec2ToAnglesLH(Vector2(dist.x, dist.z)));
			}
		}
		else if (mode == Mode::Dead)
		{
		}

		float modelDir = MyMath::ToRadians(45);
		transform->matrix
			= Matrix::CreateScale(0.5f)
			* Matrix::CreateRotationY(transform->rotation.y + modelDir)
			* Matrix::CreateTranslation(transform->position.x, 0, transform->position.z);

		MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix,MyRes::MeshType::Tracker,2 });

		//if (mode == Mode::Spawn)
		//{
		//	XMMATRIX matrix
		//		= Matrix::CreateScale(spawnScale)
		//		* transform->matrix;

		//	MyDX::Dx12Wrapper::DrawTransMesh({ matrix,MyRes::MeshType::Tracker,2 });
		//}
	}

	void Tracker::Init()
	{
		mode = Mode::Spawn;
		timeCounter = 0;
		sphColl->isTrigger = false;
		sphColl->isEnable = false;
	}

	void Tracker::Eliminate()
	{
		// スコアを加算する
		MyObj::Score::AddEliminateNum(MyRes::ScoreType::Tracker);
		MyObj::Sound::Play(6, false, true);	// 敵死亡SE
		// 自信を非アクティブにする
		SetActive(false);
	}

	void Tracker::Spawn()
	{
		// 時間を計測
		timeCounter += MySys::Timer::GetDeltaTime();

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
			sphColl->isTrigger = false;
		}
	}

	void Tracker::OnTriggerEnter(Base* _Other)
	{
	}
	void Tracker::FixPosFromStage()
	{
		// 画面外に出ていたら戻す
// 右
		if (transform->position.x >= MAX_RIGHT)
		{
			transform->position.x = static_cast<float>(MAX_RIGHT - 0.1f);
		}
		// 左
		else if (transform->position.x <= -MAX_RIGHT)
		{
			transform->position.x = static_cast<float>(-MAX_RIGHT + 0.1f);
		}

		// 上
		if (transform->position.z >= MAX_DEPTH)
		{
			transform->position.z = static_cast<float> (MAX_DEPTH - 0.1f);
		}
		// 下
		else if (transform->position.z <= -MAX_DEPTH)
		{
			transform->position.z = static_cast<float>(-MAX_DEPTH + 0.1);
		}
	}
}