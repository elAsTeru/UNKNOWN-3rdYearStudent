#include "BossStatePhase2.h"

namespace State::Boss
{
    Phase2::Phase2():
        WaitTimeShield(2.0f)
    {
    }

    Phase2::~Phase2()
    {
    }

    void Phase2::Enter(GameObject::Boss* _Owner, float _DeltaTime)
    {
        // 変数初期化
        this->fireTimeCounter = 0.0f;
        this->missileTimeCounter = 0.0f;
        this->waitTimeCounter = 0.0f;
        this->bulletNum = 10;
        // レベル設定
        switch (_Owner->GetLevel())
        {
        case 1:
            this->intervalFire = 5.0f;
            this->intervalMissile = 15.0f;
            // バリア発生
            _Owner->CreateRotShield(4, 30.0f);
            break;
        case 2:
            this->intervalFire = 3.0f;
            this->intervalMissile = 10.0f;
            // バリア発生
            _Owner->CreateRotShield(6, 30.0f);
            break;
        case 3:
            this->intervalFire = 1.5f;
            this->intervalMissile = 5.0f;
            // バリア発生
            _Owner->CreateRotShield(10, 30.0f);
            break;
        }
    }

    // 1、一定時間待機(バリアが指定位置に到着するまでの時間、元々は３秒停止していた)
    // 2、回転処理
    // 3、攻撃処理()
    // 4、体力一定以下で遷移
    StateList Phase2::Update(GameObject::Boss* _Owner, float _DeltaTime)
    {
        // 1、一定時間待機
        if (this->waitTimeCounter <= this->WaitTimeShield)
        {
            this->waitTimeCounter += _DeltaTime;
            return StateList::PHASE2;
        }

        // 2、回転処理
        _Owner->RotShields(300.0f);

        // 3、攻撃処理
        if (this->fireTimeCounter += _DeltaTime; this->fireTimeCounter >= intervalFire)
        {
            fireTimeCounter -= intervalFire;
            _Owner->FireNormal(this->bulletNum);
        }
        if (this->missileTimeCounter += _DeltaTime; this->missileTimeCounter >= intervalMissile)
        {
            missileTimeCounter -= intervalMissile;
            _Owner->FireMissile();
        }

        // 4、体力が一定以下になったら遷移
        if (_Owner->GetHpPercentage() <= 50)
        {
            // 回転シールドが残っていたら消す処理
            _Owner->ClearRotShields();
            // 次のフェーズへ遷移
            return StateList::PHASE3;
        }

        return StateList::PHASE2;
    }
}