#include "BossStateEliminate.h"
#include "EffekseerMgr.h"
#include "InputMgr.h"

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
        Effect::EffekseerMgr::SetPlaySpeed(this->elimiEfkHandle, MySys::Timer::GetHitStopTime() * 50.0f);
        // 一定時間経過してから無効化する
        if (this->timer += MySys::Timer::GetHitStopTime(); this->timer >= this->Delay)
        {
            Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::Explosion2, _Owner->transform->position, false);
            // スコアを加算
            MyObj::Score::AddEliminateNum(MyRes::ScoreType::Boss);
            MyObj::Sound::Play(4, false, true);	// 死亡SE
            Input::Mgr::GetPad()->SetVibration(0, 0, 0);
            // 自信を非アクティブにする
            _Owner->SetActive(false);
        }
        return StateList::ELIMINATE;
    }

    void Eliminate::Enter(GameObject::Boss* _Owner, float _DeltaTime)
    {
        MySys::Timer::RunHitStop();
        // 無敵化
        _Owner->SetInvisible(true);
        this->timer = 0.0f;
        // 爆破エフェクト
        this->elimiEfkHandle = Effect::EffekseerMgr::PlayEffect(MyRes::EfkType::PlayerEliminate, _Owner->transform->position, false);
        MyObj::Sound::Play(6, false, true);	// 敵死亡SE
        Input::Mgr::GetPad()->SetVibration(0, 0.8f, 0.8f);
    }
}