#include "ScoreUI.h"
#include "Dx12Wrapper.h"
#include "InputMgr.h"

static const XMVECTOR COLOR_ORANGE = { 1, 0.65f, 0, 1.0f };

namespace GameObject
{
	ScoreUI::ScoreUI() :
		Base(Tag::Ui, "ScoreUI"),
		Duration(3),
		SlideDuration(0.5f)
	{
	}

	ScoreUI::~ScoreUI()
	{}

	void ScoreUI::Init()
	{
		this->score = L"";
		this->timeCounter = 0;
		this->obsDatas.clear();
	}

	void ScoreUI::Update()
	{
		// オブザーバーデザインUIの時間経過と不要データを削除
		for (auto it = this->obsDatas.begin(); it != this->obsDatas.end();)
		{
			it->remaining -= Sys::Timer::GetDeltaTime();
			it->color = COLOR_ORANGE - (COLOR_ORANGE * it->remaining / this->Duration );

			// 持続時間の経過 または 新しいデータが入ってきたら
			if ((it->remaining <= 0) || (obsDatas.size() > 10))
			{
				it = obsDatas.erase(it);
			}
			else
			{ ++it;}
		}

		// 残ったデータに座標を与える
		for (int i = 0; i < this->obsDatas.size(); ++i)
		{
			this->obsDatas[i].pos.x = 1600;
			float targetPosY = 120 + (40.0f * (this->obsDatas.size() - i));	// 加算されたスコアが表示されるべき高さ
			if (this->timeCounter += Sys::Timer::GetDeltaTime(); this->timeCounter < SlideDuration)
			{
				this->obsDatas[i].pos.y = std::lerp(this->obsDatas[i].pos.y, targetPosY, timeCounter);
			}
			else
			{
				this->obsDatas[i].pos.y = targetPosY;
			}
		}

		Draw();
	}

	void ScoreUI::AddScore(const std::wstring _Name, const int _Score)
	{
		this->obsDatas.push_back({ Duration,_Name + L' ' + std::to_wstring(_Score) });
		timeCounter = 0;
	}

	void ScoreUI::Draw() const
	{
		// スコア表示
		MyDX::Dx12Wrapper::DrawFont({ score.c_str(), DirectX::XMFLOAT2(1500, 50), COLOR_ORANGE,10 * MyMath::Pi / 180,{},0.5f });
		// オブザーバー表示
		for (auto it = this->obsDatas.begin(); it != this->obsDatas.end(); ++it)
		{
			MyDX::Dx12Wrapper::DrawFont({ it->drawStr, DirectX::XMFLOAT2(it->pos.x , it->pos.y), it->color,10 * MyMath::Pi / 180,{},0.3f });
		}
	}
}