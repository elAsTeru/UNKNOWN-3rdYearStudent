#pragma once
#include "Boss.h"

namespace State
{
    namespace Boss
    {
        // 100 球
        // 95 回転
        // 50 追跡6
        // 40 追跡5
        // 30 追跡4
        // 25 追跡3
        // 20 追跡2
        // 15 追跡1
        // 10 キューブ

        enum class StateList
        {
            PHASE1,
            PHASE2,
            PHASE3,
            PHASE4,
            ELIMINATE,  // 終了

            SUM         // ステート総数
        };

        class Base
        {
        public:
            Base() {}
            virtual ~Base() {}

            virtual void Enter(GameObject::Boss* _Owner, float _DeltaTime) {};
            virtual StateList Update(GameObject::Boss* _Owner, float _DeltaTime) = 0;
        };
    }
}

#include "BossStatePhase1.h"
#include "BossStatePhase2.h"
#include "BossStatePhase3.h"
#include "BossStateEliminate.h"
#include "BossStatePhase4.h"