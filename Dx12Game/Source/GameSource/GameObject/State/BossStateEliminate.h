#pragma once
#include "BossStateBase.h"

namespace State::Boss
{
    /// <summary>
    /// ボスが出現後0を目指して移動する処理
    /// </summary>
    class Eliminate :public Base
    {
    public:
        Eliminate();
        ~Eliminate();

        StateList Update(GameObject::Boss* _Owner, float _DeltaTime)override;
        void Enter(GameObject::Boss* _Owner, float _DeltaTime)override;

    private:
        const float Delay;
        float timer;
        uint32_t elimiEfkHandle;	// やられエフェクトハンドル
    };
}