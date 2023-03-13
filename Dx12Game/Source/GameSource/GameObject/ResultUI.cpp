#include "ResultUI.h"
#include "Dx12Wrapper.h"
#include "Tool/InputMgr.h"
#include "Tool/DXTK12Font.h"
#include "Resource/SceneFontData.h"

#include "GameObject/ScreenStrip.h"
#include "GameObject/Board.h"

namespace GameObject
{
	ResultUI::ResultUI() :
		Base(Tag::UI, "ResultUI"),
		eliminate(),
		life(),
		phase(),
		boss(),
		total()
	{
		this->strip = new ScreenStrip;
		this->strip->Init();
		this->strip->SetDist(1.25f);
		this->strip->SetAlpha(0.4f);

		this->board = new Board;
		this->board->Init();
		this->board->SetAlpha(0.8f);
	}

	ResultUI::~ResultUI()
	{
		delete this->strip;
		this->strip = nullptr;
		delete this->board;
		this->board = nullptr;
	}

	void ResultUI::Update()
	{
		if ((Input::Mgr::GetPadLog().a == DirectX::GamePad::ButtonStateTracker::RELEASED)
			|| (Input::Mgr::GetKeyLog().IsKeyReleased(DirectX::Keyboard::Escape)))
		{

			Scene::Mgr::LoadScene("Select");
		}

		Draw();
	}

	void ResultUI::Draw() const
	{
		// 上下の帯描画
		this->strip->Draw();
		// 背景を暗くするための板描画
		this->board->Draw();

		struct ScoreData
		{
			std::wstring type;
			std::wstring score;
		};
		ScoreData scores[] =
		{
			{L"ElimScore",this->eliminate},
			{L"LifeScore",this->life},
			{L"StageScore",this->phase},
			{L"BossScore",this->boss},
			{L"TotalScore",this->total}
		};

		// FontDataを描画する
		for (const auto& font : Res::fontData[Res::FontType::Result])
		{
			MyDX::FontData temp = font;

			for (auto score : scores)
			{
				if (score.type == font.str)
				{
					temp.str = score.score;
				}
			}

			MyDX::DXTK12Font::DrawFont(temp);
		}
	}

	void ResultUI::SetEliminateScore(const std::wstring _Score)
	{
		this->eliminate = _Score;
	}

	void ResultUI::SetLifeScore(const std::wstring _Score)
	{
		this->life = _Score;
	}

	void ResultUI::SetPhaseScore(const std::wstring _Score)
	{
		this->phase = _Score;
	}

	void ResultUI::SetBossScore(const std::wstring _Score)
	{
		this->boss = _Score;
	}

	void ResultUI::SetTotalScore(const std::wstring _Score)
	{
		this->total = _Score;
	}
}