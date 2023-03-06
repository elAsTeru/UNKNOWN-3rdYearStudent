#pragma once
#include "BossStateBase.h"

namespace State::Boss
{
    /// <summary>
    /// ボスが出現後0を目指して移動する処理
    /// </summary>
    class Phase2 :public Base
    {
    public:
        Phase2();
        ~Phase2();

        StateList Update(GameObject::Boss* _Owner, float _DeltaTime)override;
        void Enter(GameObject::Boss* _Owner, float _DeltaTime)override;

    private:
        const float WaitTimeShield;     // シールドを待つ時間(ボスの動きを遅延させる)

        uint8_t bulletNum;              // 弾の数

        float fireInterval;             // 弾幕間隔
        float missileInterval;          // ミサイル間隔

        float fireTimeCounter;          // 弾幕用の計測タイマー
        float missileTimeCounter;       // ミサイル用の計測タイマー
        float waitTimeCounter;              // シールドを持つ計測タイマー
    };
}