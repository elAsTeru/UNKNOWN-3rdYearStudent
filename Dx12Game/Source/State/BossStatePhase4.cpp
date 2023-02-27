#include "BossStatePhase4.h"

namespace State::Boss
{
    Phase4::Phase4() :
        MaxAtkMode(2),
        MaxMoveMode(2),
        IntervalFire(0.2f)
    {
    }

    Phase4::~Phase4()
    {
    }

    void Phase4::Enter(GameObject::Boss* _Owner, float _DeltaTime)
    {
        // 変数初期化
        this->atkModeCounter = 0;
        this->moveModeCounter = 0;
        this->bulletNum = 10;
        this->leftFireNum = 0;
        this->fireTimeCounter = 0.0f;
        // レベル設定
        switch (_Owner->GetLevel())
        {
        case 1:
            this->moveSpeed = 30.0f;        // 移動速度を設定
            this->fireNum = 1;              // 反射後に撃つ回数
            break;
        case 2:
            this->moveSpeed = 27.5f;
            this->fireNum = 2;
            break;
        case 3:
            this->moveSpeed = 25.0f;
            this->fireNum = 3;
            break;
        }
        _Owner->SetSpeed(this->moveSpeed);  // 移動速度設定


        // プレイヤーと反対側に移動を開始するために反対側を向く
        Vector3 playerPos = GameObject::Mgr::Find("Player")->transform->position;
        Vector3 dir = _Owner->transform->position - playerPos;
        _Owner->transform->rotation.y = MyMath::Vec2ToAnglesLH(Vector2(dir.x, dir.z));

        // 無敵解除
        _Owner->SetInvisible(false);
        _Owner->ClearWaitTime();
    }

    StateList Phase4::Update(GameObject::Boss* _Owner, float _DeltaTime)
    {
        // 移動処理
        _Owner->MoveForward();
        // 壁に衝突していた場合
        if (_Owner->IsOutsideStage())
        {
            _Owner->ReflectionMoveDir();

            // 攻撃処理
            this->leftFireNum = fireNum;    // 攻撃回数を入れる
            switch (this->atkModeCounter)
            {
            case 0:
                ++this->atkModeCounter;     // 攻撃モードを移行
                break;
            case 1:
                _Owner->FireMissile();
                --this->atkModeCounter;     // 攻撃モードを移行

                // 移動モードを移行
                if (++this->moveModeCounter; this->moveModeCounter >= this->MaxMoveMode)
                {
                    this->moveModeCounter = 0;
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

        if (_Owner->GetHpPercentage() <= 0)
        {
            return StateList::ELIMINATE;
        }

        return StateList::PHASE4;
    }
}