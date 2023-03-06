#include "ScoreUI.h"
#include "Dx12Wrapper.h"
#include "Tool/InputMgr.h"
#include "Tool/DXTK12Font.h"

static const XMVECTOR COLOR_ORANGE = { 1, 0.65f, 0, 1.0f };

namespace GameObject
{
	ScoreUI::ScoreUI() :
		Base(Tag::UI, "ScoreUI"),
		LifeTime(3),
		SlideDuration(0.5f)
	{
	}

	ScoreUI::~ScoreUI()
	{}

	void ScoreUI::Init()
	{
		this->score = L"";
		this->timeCounter = 0;
		this->drawDescs.clear();
	}

	void ScoreUI::Update()
	{
		// オブザーバーデザインUIの時間経過と不要データを削除
		for (auto it = this->drawDescs.begin(); it != this->drawDescs.end();)
		{
			it->remaining -= Sys::Timer::GetDeltaTime();
			it->color = COLOR_ORANGE - (COLOR_ORANGE * it->remaining / this->LifeTime);

			// 持続時間の経過 または 新しいデータが入ってきたら
			if ((it->remaining <= 0) || (drawDescs.size() > 10))
			{
				it = drawDescs.erase(it);
			}
			else
			{ ++it;}
		}

		// 残ったデータに座標を与える
		for (int i = 0; i < this->drawDescs.size(); ++i)
		{
			this->drawDescs[i].pos.x = 1600;
			float targetPosY = 120 + (40.0f * (this->drawDescs.size() - i));	// 加算されたスコアが表示されるべき高さ
			if (this->timeCounter += Sys::Timer::GetDeltaTime(); this->timeCounter < SlideDuration)
			{
				this->drawDescs[i].pos.y = std::lerp(this->drawDescs[i].pos.y, targetPosY, timeCounter);
			}
			else
			{
				this->drawDescs[i].pos.y = targetPosY;
			}
		}

		Draw();
	}

	void ScoreUI::AddScore(const std::wstring _Name, const int _Score)
	{
		this->drawDescs.emplace_back(DrawDesc(LifeTime, _Name + L' ' + std::to_wstring(_Score)));
		timeCounter = 0;
	}

	void ScoreUI::Draw() const
	{
		// スコア表示
		MyDX::DXTK12Font::DrawFont({ score.c_str(), DirectX::XMFLOAT2(1500, 50), COLOR_ORANGE,10 * MyMath::Pi / 180,{},0.5f });
		// オブザーバー表示
		for (auto it = this->drawDescs.begin(); it != this->drawDescs.end(); ++it)
		{
			MyDX::DXTK12Font::DrawFont({ it->drawStr, DirectX::XMFLOAT2(it->pos.x , it->pos.y), it->color,10 * MyMath::Pi / 180,{},0.3f });
		}
	}
}