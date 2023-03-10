#include "TitleUI.h"
#include "Dx12Wrapper.h"
#include "Tool/InputMgr.h"
#include "Tool/DXTK12Font.h"
#include "Resource/SceneFontData.h"

#include "GameObject/Board.h"

namespace GameObject
{
	TitleUI::TitleUI() :
		Base(Tag::UI, "TitleUI"),
		FadeDelay(2.0f),
		FadeDuration(1.0f),
		FirstBlinkSpeed(2),
		MaxBlinkSpeed(20),
		blinkSpeed(FirstBlinkSpeed),
		timeCounter(0),
		blinkColor(),
		state(State::In)
	{
		this->board = new Board;
		this->board->Init();
		this->board->SetAlpha(1.0f);
	}

	TitleUI::~TitleUI()
	{}

	void TitleUI::Update()
	{
		// デバッグ時のみ有効(コメントアウトし忘れ防止)
#if defined(DEBUG) || (_DEBUG)
		//Scene::Mgr::LoadScene("Stage1");	// シーンを飛ばす
#endif
		if (this->state == State::In)
		{
			// 時間計測が遅延時間を超えたら
			if (this->timeCounter += Sys::Timer::GetDeltaTime(); this->timeCounter >= this->FadeDelay)
			{
				// フェード用の板をフェードアウトする 1 → 0にする
				this->board->SetAlpha(MyMath::GetFadeOutMag(this->timeCounter - this->FadeDelay, this->FadeDuration));
			}

			// 計測時間が越えていたら補正する(フェード終了)
			if ((this->timeCounter - this->FadeDelay) >= this->FadeDuration)
			{
				this->timeCounter = 0;
				this->board->SetAlpha(0.0f);
				this->state = State::Idol;
			}
		}
		else if (this->state == State::Idol)
		{
			this->timeCounter += Sys::Timer::GetDeltaTime();
			// 点滅処理
			this->blinkColor = Orange;
			this->blinkColor.x -= MyMath::CircularMotion(0.0f, Orange.x, this->timeCounter, blinkSpeed);
			this->blinkColor.y -= MyMath::CircularMotion(0.0f, Orange.y, this->timeCounter, blinkSpeed);

			// ボタン入力処理
			if ((Input::Mgr::GetPadLog().a == DirectX::GamePad::ButtonStateTracker::RELEASED)
				|| (Input::Mgr::GetKeyLog().IsKeyReleased(DirectX::Keyboard::Enter)))
			{
				MyDX::Tool::Sound::PlaySE(Res::SEType::Enter);
				this->blinkSpeed = MaxBlinkSpeed;	// 点滅速度上昇
				this->timeCounter = 0;
				this->state = State::Out;
			}
		}
		else
		{
			// 点滅処理
			this->blinkColor = Orange;
			this->blinkColor.x -= MyMath::CircularMotion(0.0f, Orange.x, this->timeCounter, blinkSpeed);
			this->blinkColor.y -= MyMath::CircularMotion(0.0f, Orange.y, this->timeCounter, blinkSpeed);

			// 時間計測が遅延時間を超えたら
			if (this->timeCounter += Sys::Timer::GetDeltaTime(); this->timeCounter >= this->FadeDelay)
			{
				// フェード用の板をフェードインする 1 → 0にする
				this->board->SetAlpha(MyMath::GetFadeInMag(this->timeCounter - this->FadeDelay, this->FadeDuration));
			}

			// 計測時間が越えていたら補正する(フェード終了
			if ((this->timeCounter - this->FadeDelay) >= this->FadeDuration)
			{
				Scene::Mgr::LoadScene("Select");
			}
		}
		Draw();
	}

	void TitleUI::Draw() const
	{
		// タイトルのUはフォントでは表示できないのでテクスチャを使用する
		DirectX::XMMATRIX titleMatrix
			= Matrix::CreateScale({ 0.08f })
			* Matrix::CreateTranslation(-0.45f, 0, 0.045f);
		MyDX::Dx12Wrapper::DrawMesh2D(titleMatrix,Res::MeshType::Board,Res::MaterialType::U2);

		// FontDataを描画する
		for (const auto& font : Res::fontData[Res::FontType::Title])
		{
			MyDX::FontData temp = font;		// フェード等をかけるために使用
			//　点滅処理
			if (font.str == L"Ⓐ プレイ")
			{
				temp.color = this->blinkColor;
			}
			// Font全体にフェード処理をかける
			temp.color -= Vector4(this->board->GetAlpha());
			// 描画
			MyDX::DXTK12Font::DrawFont(temp);
		}

		// フェード処理用の板
		XMMATRIX matrix
			= Matrix::CreateScale(1);
		MyDX::Dx12Wrapper::DrawMesh2D(matrix,Res::MeshType::Board,Res::MaterialType::Black,this->board->GetAlpha());
	}
}