#include "BossStatePhase1.h"
#include "EfkMgr.h"

namespace State::Boss
{
    Phase1::Phase1():
        MoveSpeed(30.0f)
    {
    }

    Phase1::~Phase1()
    {
    }

    void Phase1::Enter(GameObject::Boss* _Owner, float _DeltaTime)
    {
        // ボスの初期化処理

    }

    // 1、画面中央に移動
    // 2、体力一定以下で遷移
    StateList Phase1::Update(GameObject::Boss* _Owner, float _DeltaTime)
    {
        // 中央を目指す処理、目標座標に達してなかったら終了
        if (!_Owner->Move(Vector3::Zero, this->MoveSpeed))
        {
            return StateList::PHASE1;
        }

        // 体力が一定以下になったら遷移
        if (_Owner->GetHpPercentage() <= ActionHPPer.rot)
        {
            Effect::EfkMgr::PlayEffect(Res::EfkType::Explosion2, _Owner->transform->position, false);
            return StateList::PHASE2;
        }

        return StateList::PHASE1;
    }
}