#include "BossStatePhase3.h"

namespace State::Boss
{
    Phase3::Phase3() :
        MaxAtkMode(2),
        MaxMoveMode(2),
        WaitTimeShield(2.0f),
        IntervalFire(0.2f)
    {
    }

    Phase3::~Phase3()
    {
    }

    void Phase3::Enter(GameObject::Boss* _Owner, float _DeltaTime)
    {
        // 変数初期化
        this->atkModeCounter = 0;
        this->moveModeCounter = 0;
        this->waitTimeCounter = 0.0f;
        this->activateTimeCounter = 0.0f;
        this->bulletNum = 10;
        this->leftFireNum = 0;
        this->fireTimeCounter = 0.0f;
        // レベル設定
        struct LvDesc
        {
            uint8_t level;                  // レベル
            float activateEnemyInterval;    // 敵を出す間隔、-1 : 敵を出さない
            float moveSpeed;                // 移動速度
            float fireNum;                  // 一回の攻撃で弾を出す回数
        };
        std::vector<LvDesc> levelDescs =
        {
            {1, -1.0f, 10.0f, 1},
            {2, 5.0f, 12.5f, 3},
            {3, 3.0f, 15.0f, 5}
        };
        for (auto lvDesc : levelDescs)
        {
            if (_Owner->GetLevel() == lvDesc.level)
            {
                this->activateEnemyInterval = lvDesc.activateEnemyInterval;
                this->moveSpeed = lvDesc.moveSpeed;
                _Owner->SetSpeed(this->moveSpeed);              // 移動速度設定
                this->fireNum = lvDesc.fireNum;
                break;
            }
        }
        _Owner->CreateTrackShield(6, this->moveSpeed * 1.3f);   // 追跡シールドの作成

        // プレイヤーと反対側に移動を開始するために反対側を向く
        Vector3 playerPos = GameObject::Mgr::Find("Player")->transform->position;
        Vector3 dir = _Owner->transform->position - playerPos;
        _Owner->transform->rotation.y = MyMath::Vec2ToAnglesLH(Vector2(dir.x, dir.z));

        // 無敵化
        _Owner->SetInvisible(true);
    }

    // 1、一定時間待機(バリアが指定位置に到着するまでの時間、元々は３秒停止していた)
    // 2、移動処理、壁にヒットした際に、攻撃処理と一定間隔で移動モード変更
    // 3、一定間隔で追尾敵出現
    StateList Phase3::Update(GameObject::Boss* _Owner, float _DeltaTime)
    {
        // 1、一定時間待機
        if (this->waitTimeCounter <= this->WaitTimeShield)
        {
            this->waitTimeCounter += _DeltaTime;
            return StateList::PHASE3;
        }

        // 弾攻撃→レーザー→移動方法変更→ループ
        // 開幕のみプレイヤーと反対側に移動
        // 反射追跡→追跡→ループ

        // 2、移動処理
        _Owner->MoveForward();

        if (this->moveModeCounter == 1)
        {
            _Owner->RotPlayerPos();
        }

        // 壁に衝突していた場合
        if (_Owner->IsOutsideStage())
        {
            _Owner->ReflectionMoveDir();

            // 攻撃処理
            switch (this->atkModeCounter)
            {
            case 0:
                this->leftFireNum = fireNum;    // 攻撃回数を入れる
                ++this->atkModeCounter;         // 攻撃モードを移行
                break;
            case 1:
                if (_Owner->PutLaserPoint())
                {
                    --this->atkModeCounter; // 攻撃モードを移行

                    // 移動モードを移行
                    if (++this->moveModeCounter; this->moveModeCounter >= this->MaxMoveMode)
                    {
                        this->moveModeCounter = 0;
                    }
                }
            }
        }

        // 弾幕処理
        if (this->leftFireNum > 0)
        {
            // 計測時間が射撃間隔時間を超えていたら、弾幕をはる
            if (this->fireTimeCounter >= this->IntervalFire)
            {
                _Owner->FireNormal(this->bulletNum);
                --this->leftFireNum;
                this->fireTimeCounter = 0.0f;
            }
            else
            {
                this->fireTimeCounter += Sys::Timer::GetHitStopTime();
            }
        }

        // 3、レベル2以上なら敵を一定間隔で出現させる
        if (this->activateEnemyInterval > 0.0f && this->activateTimeCounter >= this->activateEnemyInterval)
        {
            if (auto temp = GameObject::Mgr::FindDeactiveObj("SlowTracker"); temp)
            {
                temp->Init();
                temp->transform->position = _Owner->transform->position;
                temp->SetActive(true);
            }

            this->activateTimeCounter = 0.0f;
        }
        this->activateTimeCounter += Sys::Timer::GetHitStopTime();

        // レーザーを張った場合広い方へ避ける
        // シールドがコアを追跡
        _Owner->TrackingShields();


        // シールドが減る表現
        if (_Owner->GetHpPercentage() <= ActionHPPer.track1)
        {
            _Owner->NumOrLessTrackingShield(1);
        }
        else if (_Owner->GetHpPercentage() <= ActionHPPer.track2)
        {
            _Owner->NumOrLessTrackingShield(2);
        }
        else if (_Owner->GetHpPercentage() <= ActionHPPer.track3)
        {
            _Owner->NumOrLessTrackingShield(3);
        }
        else if (_Owner->GetHpPercentage() <= ActionHPPer.track4)
        {
            _Owner->NumOrLessTrackingShield(4);
        }
        else if (_Owner->GetHpPercentage() <= ActionHPPer.track5)
        {
            _Owner->NumOrLessTrackingShield(5);
        }

        // シールドが0になったら次の状態に遷移
        if (_Owner->GetHpPercentage() <= ActionHPPer.cube)
        {
            _Owner->ClearTrackShields();    // 処理が完成したら不要な処理...シールドがすべてなくなったら遷移するため
            _Owner->ClearLaser();
            return StateList::PHASE4;
        }

        return StateList::PHASE3;
    }
}