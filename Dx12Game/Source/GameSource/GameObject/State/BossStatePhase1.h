#pragma once
#include "BossStateBase.h"

namespace State::Boss
{
    /// <summary>
    /// ボスが出現後0を目指して移動する処理
    /// </summary>
    class Phase1 :public Base
    {
    public:
        Phase1();
        ~Phase1();

        StateList Update(GameObject::Boss* _Owner, float _DeltaTime)override;
        void Enter(GameObject::Boss* _Owner, float _DeltaTime)override;

    private:
        const float MoveSpeed;      // 移動速度
    };
}