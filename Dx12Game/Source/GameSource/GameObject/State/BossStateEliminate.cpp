#include "BossStateEliminate.h"
#include "Tool/EfkMgr.h"
#include "Tool/InputMgr.h"

namespace State::Boss
{
    Eliminate::Eliminate():
        Delay(1.0f)
    {
    }

    Eliminate::~Eliminate()
    {
    }

    StateList Eliminate::Update(GameObject::Boss* _Owner, float _DeltaTime)
    {
        Effect::EfkMgr::SetPlaySpeed(this->elimiEfkHandle, Sys::Timer::GetHitStopTime() * 50.0f);
        // 一定時間経過してから無効化する
        if (this->timer += Sys::Timer::GetHitStopTime(); this->timer >= this->Delay)
        {
            Effect::EfkMgr::PlayEffect(Res::EfkType::Explosion2, _Owner->transform->position, false);
            // スコアを加算
            Other::Score::AddEliminateNum(Res::ScoreType::Boss);
            MyDX::Tool::Sound::PlaySE(Res::SEType::Died);
            Input::Mgr::GetPad()->SetVibration(0, 0, 0);
            // 自信を非アクティブにする
            _Owner->SetActive(false);
        }
        return StateList::ELIMINATE;
    }

    void Eliminate::Enter(GameObject::Boss* _Owner, float _DeltaTime)
    {
        Sys::Timer::RunHitStop();
        // 無敵化
        _Owner->SetInvisible(true);
        this->timer = 0.0f;
        // 爆破エフェクト
        this->elimiEfkHandle = Effect::EfkMgr::PlayEffect(Res::EfkType::PlayerEliminate, _Owner->transform->position, false);
        MyDX::Tool::Sound::PlaySE(Res::SEType::Eliminate);
        Input::Mgr::GetPad()->SetVibration(0, 0.8f, 0.8f);
    }
}