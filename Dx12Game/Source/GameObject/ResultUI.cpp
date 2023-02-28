#include "ResultUI.h"
#include "Dx12Wrapper.h"
#include "InputMgr.h"
#include "DXTK12Font.h"

static constexpr XMVECTOR COLOR_ORANGE = { 1, 0.65f, 0, 0.1f };

namespace GameObject
{
	ResultUI::ResultUI() :
		Base(Tag::Ui, "ResultUI"),
		MaxDigits(7),
		eliminate(),
		life(),
		phase(),
		boss(),
		total()
	{
	}

	ResultUI::~ResultUI()
	{}

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
		// スコア影の表示
		MyDX::DXTK12Font::DrawFont({ L"RESULT", DirectX::XMFLOAT2(300, 185), {0.05f, 0.05f, 0.05f, 1},{},{},0.8f });
		MyDX::DXTK12Font::DrawFont({ L"RESULT", DirectX::XMFLOAT2(300, 180), {1, 0.65f, 0, 1},{},{},0.8f });

		MyDX::DXTK12Font::DrawFont({ L"Eliminate", DirectX::XMFLOAT2(450, 295), {0.05f, 0.05f, 0.05f, 1},{},{},0.5f });
		MyDX::DXTK12Font::DrawFont({ L"Eliminate", DirectX::XMFLOAT2(450, 290), {1, 0.65f, 0, 1},{},{},0.5f });

		MyDX::DXTK12Font::DrawFont({ this->eliminate, DirectX::XMFLOAT2(540, 365), {0.05f, 0.05f, 0.05f, 1},{},{},0.5f });
		MyDX::DXTK12Font::DrawFont({ this->eliminate, DirectX::XMFLOAT2(540, 360), {1, 0.65f, 0, 1},{},{},0.5f });

		MyDX::DXTK12Font::DrawFont({ L"Life", DirectX::XMFLOAT2(450, 435), {0.05f, 0.05f, 0.05f, 1},{},{},0.5f });
		MyDX::DXTK12Font::DrawFont({ L"Life", DirectX::XMFLOAT2(450, 430), {1, 0.65f, 0,1},{},{},0.5f });

		MyDX::DXTK12Font::DrawFont({ this->life, DirectX::XMFLOAT2(540, 505), {0.05f, 0.05f, 0.05f, 1},{},{},0.5f });
		MyDX::DXTK12Font::DrawFont({ this->life, DirectX::XMFLOAT2(540, 500), {1, 0.65f, 0, 1},{},{},0.5f });

		MyDX::DXTK12Font::DrawFont({ L"Stage", DirectX::XMFLOAT2(450, 575), {0.05f, 0.05f, 0.05f, 1},{},{},0.5f });
		MyDX::DXTK12Font::DrawFont({ L"Stage", DirectX::XMFLOAT2(450, 570), {1, 0.65f, 0,1},{},{},0.5f });

		MyDX::DXTK12Font::DrawFont({ this->phase, DirectX::XMFLOAT2(540, 645), {0.05f, 0.05f, 0.05f, 1},{},{},0.5f });
		MyDX::DXTK12Font::DrawFont({ this->phase, DirectX::XMFLOAT2(540, 640), {1, 0.65f, 0, 1},{},{},0.5f });

		MyDX::DXTK12Font::DrawFont({ L"Boss", DirectX::XMFLOAT2(450, 715), {0.05f, 0.05f, 0.05f, 1},{},{},0.5f });
		MyDX::DXTK12Font::DrawFont({ L"Boss", DirectX::XMFLOAT2(450, 710), {1, 0.65f, 0,1},{},{},0.5f });

		MyDX::DXTK12Font::DrawFont({ this->boss, DirectX::XMFLOAT2(540, 785), {0.05f, 0.05f, 0.05f, 1},{},{},0.5f });
		MyDX::DXTK12Font::DrawFont({ this->boss, DirectX::XMFLOAT2(540, 780), {1, 0.65f, 0, 1},{},{},0.5f });

		MyDX::DXTK12Font::DrawFont({ L"Total Score", DirectX::XMFLOAT2(950, 385), {0.05f, 0.05f, 0.05f, 1},{},{},0.9f });
		MyDX::DXTK12Font::DrawFont({ L"Total Score", DirectX::XMFLOAT2(950, 380), {1, 0.65f, 0, 1},{},{},0.9f });

		MyDX::DXTK12Font::DrawFont({ this->total, DirectX::XMFLOAT2(1050, 485), {0.05f, 0.05f, 0.05f, 1},{},{},0.9f });
		MyDX::DXTK12Font::DrawFont({ this->total, DirectX::XMFLOAT2(1050, 480), {1, 0.65f, 0, 1},{},{},0.9f });

		// 上から下
		XMMATRIX matrix
			= Matrix::CreateTranslation(0, 0, 1.25);
		MyDX::Dx12Wrapper::Draw2DUI({ matrix,Res::MeshType::Board,Res::MaterialType::Gray,0.4f });
		// 下から上
		matrix
			= Matrix::CreateTranslation(0, 0, -1.25);
		MyDX::Dx12Wrapper::Draw2DUI({ matrix,Res::MeshType::Board,Res::MaterialType::Gray,0.4f });
		// 黒板
		matrix
			= Matrix::CreateScale(1);
		MyDX::Dx12Wrapper::Draw2DUI({ matrix,Res::MeshType::Board,Res::MaterialType::Black,0.8 });

		MyDX::DXTK12Font::DrawFont({ L"Ⓐ 戻る", DirectX::XMFLOAT2(140, 970), COLOR_ORANGE,{},{},0.5f });
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

	void ResultUI::FillLeftZero(std::wstring& _WstrValue, const int& _Digits)
	{
		std::wstring temp;	// 左を0で埋めた数文字列を一時保存
		// 指定した桁数に満たない分0を入れていく
		for (int i = 0; i < _Digits - _WstrValue.size(); ++i)
		{ temp += '0'; }
		// 元データの数値を追加していく
		for ( int i = 0; i < _WstrValue.size();++i)
		{temp += _WstrValue[i]; }
		// 元データを書き換えて終了
		_WstrValue = temp;
	}

	void ResultUI::InsertComma(std::wstring& _WstrValue)
	{
		std::wstring reverse ;		// 文字列を反転したもの
		std::wstring temp;			// コロンで区切った数文字列を一時保存

		// 反転
		for (int i = 0; i < _WstrValue.size(); ++i)
		{
			reverse += _WstrValue[(_WstrValue.size() - 1) - i];
		}
		// 3の倍数桁ごとにコンマを入れる
		for (int i = 0; i < reverse.size(); ++i)
		{
			if ((i != 0) && (i % 3 == 0))
			{
				temp += ',';
			}
			temp += reverse[i];
		}
		// データをもとに戻し、元データを書き換える
		_WstrValue.clear();
		for (int i = 0; i < temp.size(); ++i)
		{
			_WstrValue += temp[(temp.size() - 1) - i];
		}
	}
}