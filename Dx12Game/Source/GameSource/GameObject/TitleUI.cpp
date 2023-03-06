#include "TitleUI.h"
#include "Dx12Wrapper.h"
#include "Tool/InputMgr.h"
#include "Tool/DXTK12Font.h"

namespace GameObject
{
	DirectX::XMMATRIX titleMatrix{};

	TitleUI::TitleUI() :
		Base(Tag::UI, "TitleUI"),
		FirstBlinkSpeed(2),
		MaxBlinkSpeed(20),
		FadeDelay(2),
		FadeDuration(1),
		BaseColor(1, 0.65f, 0, 1),
		blinkSpeed(FirstBlinkSpeed),
		timeCounter(0),
		blinkColor(),
		fadeValue(1),
		state(State::In)
	{}

	TitleUI::~TitleUI()
	{}

	void TitleUI::Update()
	{
		// デバッグ時のみ有効
#if defined(DEBUG) || (_DEBUG)
		//Scene::Mgr::LoadScene("Stage1");	// シーンを飛ばす
#endif

		if (this->state == State::In)
		{
			// 時間計測が遅延時間を超えたら
			if (this->timeCounter += Sys::Timer::GetDeltaTime(); this->timeCounter >= this->FadeDelay)
			{
				// フェードインする 1 → 0にする
				this->fadeValue = std::lerp(1, 0, (this->timeCounter - this->FadeDelay) / this->FadeDuration);
			}

			// 計測時間が越えていたら補正する(フェード終了
			if ((this->timeCounter - this->FadeDelay) >= this->FadeDuration)
			{
				this->timeCounter = 0;
				this->fadeValue = 0;
				this->state = State::Idol;
			}
		}
		else if (this->state == State::Idol)
		{
			this->timeCounter += Sys::Timer::GetDeltaTime();
			// 点滅処理
			this->blinkColor = this->BaseColor;
			this->blinkColor.x -= MyMath::CircularMotion(0.0f, BaseColor.x, this->timeCounter, blinkSpeed);
			this->blinkColor.y -= MyMath::CircularMotion(0.0f, BaseColor.y, this->timeCounter, blinkSpeed);

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
			this->blinkColor = this->BaseColor;
			this->blinkColor.x -= MyMath::CircularMotion(0.0f, BaseColor.x, this->timeCounter, blinkSpeed);
			this->blinkColor.y -= MyMath::CircularMotion(0.0f, BaseColor.y, this->timeCounter, blinkSpeed);
			// 時間計測が遅延時間を超えたら
			if (this->timeCounter += Sys::Timer::GetDeltaTime(); this->timeCounter >= this->FadeDelay)
			{
				// フェードインする 1 → 0にする
				this->fadeValue = std::lerp(0, 1, (this->timeCounter - this->FadeDelay) / this->FadeDuration);
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
		titleMatrix
			= Matrix::CreateScale({ 0.08f })
			* Matrix::CreateTranslation(-0.45f, 0, 0.045f);
		MyDX::Dx12Wrapper::DrawMesh2D(titleMatrix,Res::MeshType::Board,Res::MaterialType::U2);

		MyDX::DXTK12Font::DrawFont({ L"NKNOWN", DirectX::XMFLOAT2(290,410), BaseColor - Vector4(this->fadeValue),{},{},1 });
		MyDX::DXTK12Font::DrawFont({ L"Ⓐ プレイ", DirectX::XMFLOAT2(330,580), blinkColor - Vector4(this->fadeValue),{},{},0.7f });

		// フェード処理用の板
		XMMATRIX matrix
			= Matrix::CreateScale(1);
		MyDX::Dx12Wrapper::DrawMesh2D(matrix,Res::MeshType::Board,Res::MaterialType::Black,this->fadeValue);
	}
}