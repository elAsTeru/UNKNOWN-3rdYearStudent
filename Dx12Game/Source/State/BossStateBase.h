#pragma once
#include "Boss.h"

namespace State
{
    namespace Boss
    {
        struct ActionHPPercentage
        {
            float sphere;   // 球の状態のHP割合
            float rot;      // 回転状態になるときのHP割合
            float track6;   // 追跡状態になるときのHP割合
            float track5;   // 追跡時のシールドが１つなくなるときのHP割合
            float track4;   // 追跡時のシールドが２つなくなるときのHP割合
            float track3;   // 追跡時のシールドが３つなくなるときのHP割合
            float track2;   // 追跡時のシールドが４つなくなるときのHP割合
            float track1;   // 追跡時のシールドが５つなくなるときのHP割合
            float cube;     // 追跡時のシールドがすべてなくなり行動が変わるときのHP割合
        };
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
            Base():
                ActionHPPer({ 100,95,50,40,30,25,20,15,10 })
            {}
            virtual ~Base() {}

            // Private Variable

            const ActionHPPercentage ActionHPPer;

            // Private Method

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