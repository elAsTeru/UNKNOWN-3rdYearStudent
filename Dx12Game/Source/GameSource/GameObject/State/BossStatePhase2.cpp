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
        struct LvDesc
        {
            uint8_t level;                      // レベル
            float fireInderval;                 // 射撃間隔
            float missileInterval;              // ミサイル間隔
            uint8_t shieldNum;                  // シールド数
            float shieldTrackingSpeed = 30.0f;  // シールドの所定位置を目指す速度
        };
        std::vector<LvDesc> levelDescs =
        {
            {1, 5.0f ,15.0f, 4},
            {2, 3.0f, 10.0f, 6},
            {3, 1.5f, 5.0f, 10}
        };
        for (auto lvDesc : levelDescs)
        {
            if (_Owner->GetLevel() == lvDesc.level)
            {
                this->fireInterval = lvDesc.fireInderval;
                this->missileInterval = lvDesc.missileInterval;
                _Owner->CreateRotShield(lvDesc.shieldNum, lvDesc.shieldTrackingSpeed);
                break;
            }
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
        if (this->fireTimeCounter += _DeltaTime; this->fireTimeCounter >= fireInterval)
        {
            fireTimeCounter -= fireInterval;
            _Owner->FireNormal(this->bulletNum);
        }
        if (this->missileTimeCounter += _DeltaTime; this->missileTimeCounter >= missileInterval)
        {
            missileTimeCounter -= missileInterval;
            _Owner->FireMissile();
        }

        // 4、体力が一定以下になったら遷移
        if (_Owner->GetHpPercentage() <= ActionHPPer.track6)
        {
            // 回転シールドが残っていたら消す処理
            _Owner->ClearRotShields();
            // 次のフェーズへ遷移
            return StateList::PHASE3;
        }

        return StateList::PHASE2;
    }
}