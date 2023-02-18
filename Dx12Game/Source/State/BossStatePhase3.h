#pragma once
#include "BossStateBase.h"

namespace State::Boss
{
    /// <summary>
    /// ボスが出現後0を目指して移動する処理
    /// </summary>
    class Phase3 :public Base
    {
    public:
        Phase3();
        ~Phase3();

        StateList Update(GameObject::Boss* _Owner, float _DeltaTime)override;
        void Enter(GameObject::Boss* _Owner, float _DeltaTime)override;

    private:
        const uint8_t MaxAtkMode;       // 攻撃モード数
        const uint8_t MaxMoveMode;      // 移動モード数
        const float WaitTimeShield;

        const float IntervalFire;       // 反射後に弾を撃つ間隔
        uint8_t bulletNum;              // 弾の数
        uint8_t fireNum;                // 反射後に弾を撃つ回数
        uint8_t leftFireNum;            // 残りの射撃回数
        float fireTimeCounter;          // 射撃用タイムカウンター

        uint8_t atkModeCounter;         // 攻撃モードカウンター
        uint8_t moveModeCounter;        // 移動モードカウンター
        float waitTimeCounter;

        float enemyActivateInterval;
        float activateTimeCounter;

        float moveSpeed;
    };
}