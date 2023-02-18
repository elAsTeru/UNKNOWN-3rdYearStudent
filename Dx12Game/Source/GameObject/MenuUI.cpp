#include "MenuUI.h"
#include "Dx12Wrapper.h"
#include "InputMgr.h"

namespace GameObject
{
	MenuUI::MenuUI() :
		Base(Tag::UI, "MenuUI"),
		FirstBlinkSpeed(2),
		MaxBlinkSpeed(20),
		FadeDelay(1),
		FadeDuration(1),
		BaseColor(1, 0.65f, 0, 1),
		blinkSpeed(FirstBlinkSpeed),
		timeCounter(0),
		blinkColor(),
		fadeValue(1),
		state(State::Out),
		selectNum(-1)
	{}

	MenuUI::~MenuUI()
	{}

	void MenuUI::Update()
	{
		if (this->state == State::In)
		{
			// 時間計測が遅延時間を超えたら
			if (this->timeCounter += MySys::Timer::GetDeltaTime(); this->timeCounter >= this->FadeDelay)
			{
				// フェードインする 1 → 0にする
				this->fadeValue = std::lerp(1, 0, (this->timeCounter - this->FadeDelay) / this->FadeDuration);
			}

			// 計測時間が越えていたら補正する(フェード終了
			if ((this->timeCounter - this->FadeDelay) >= this->FadeDuration)
			{
				this->timeCounter = 0;
				this->fadeValue = 0;
				this->selectNum = 0;
				this->state = State::Idol;
			}
		}
		else if (this->state == State::Idol)
		{
			this->timeCounter += MySys::Timer::GetDeltaTime();
			// 点滅処理
			this->blinkColor = this->BaseColor;
			this->blinkColor.x -= MyMath::CircularMotion(0.0f, BaseColor.x, this->timeCounter, blinkSpeed);
			this->blinkColor.y -= MyMath::CircularMotion(0.0f, BaseColor.y, this->timeCounter, blinkSpeed);

			// ボタン入力処理
			// 選択変更
			UpdateSelectNum();
			// シーン切り替え
			if ((Input::Mgr::GetPadLog().a == DirectX::GamePad::ButtonStateTracker::RELEASED)
				|| (Input::Mgr::GetKeyLog().IsKeyReleased(DirectX::Keyboard::Enter)))
			{
				MyObj::Sound::Play(2, false, true);	// 選択決定SE
				this->blinkSpeed = MaxBlinkSpeed;	// 点滅速度上昇
				this->timeCounter = 0;
				this->state = State::Out;
			}
			if ((Input::Mgr::GetPadLog().start == DirectX::GamePad::ButtonStateTracker::PRESSED)
				|| (Input::Mgr::GetKeyLog().IsKeyPressed(DirectX::Keyboard::Escape)))
			{
				MyObj::Sound::Play(2, false, true);	// 選択決定SE
				this->selectNum = -1;
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
			if (this->timeCounter += MySys::Timer::GetDeltaTime(); this->timeCounter >= this->FadeDelay)
			{
				// フェードインする 1 → 0にする
				this->fadeValue = std::lerp(0, 1, (this->timeCounter - this->FadeDelay) / this->FadeDuration);
			}

			// 計測時間が越えていたら補正する(フェード終了
			if ((this->timeCounter - this->FadeDelay) >= this->FadeDuration)
			{
				switch (this->selectNum)
				{
				case 0:
					Scene::Mgr::LoadScene("Title");
					break;
				case 1:
					Scene::Mgr::Quit();
				}
			}

			if ((Input::Mgr::GetPadLog().start == DirectX::GamePad::ButtonStateTracker::PRESSED)
				|| (Input::Mgr::GetKeyLog().IsKeyPressed(DirectX::Keyboard::Escape)))
			{
				MyObj::Sound::Play(2, false, true);	// 選択決定SE
				this->timeCounter = 0;
				this->state = State::In;
			}
		}
	
		Draw();
	}

	void MenuUI::Draw()
	{
		// 右から左
		XMMATRIX matrix
			= Matrix::CreateTranslation(1.3, 0, 0);
		MyDX::Dx12Wrapper::Draw2DUI({ matrix,MyRes::MeshType::Board,1,0.8f });

		// 左から右
		matrix
			= Matrix::CreateTranslation(-1.52, 0, 0);
		MyDX::Dx12Wrapper::Draw2DUI({ matrix,MyRes::MeshType::Board,1,0.8f });

		MyDX::Dx12Wrapper::DrawFont({ L"㊂ メニュー", DirectX::XMFLOAT2(1530, 40), {1, 0.65f, 0, 1},{},{},0.5f });

		MyDX::Dx12Wrapper::DrawFont({ L"タイトル", DirectX::XMFLOAT2(1550, 160 ), {0.20f, 0.15f, 0.25f, 1},{},{},0.6f });
		MyDX::Dx12Wrapper::DrawFont({ L"終了", DirectX::XMFLOAT2(1600, 280 ), {0.20f, 0.15f, 0.25f, 1},{},{},0.6f });

		// 選択されている文字の上にオレンジ色の文字を表示する
		switch (this->selectNum) {
		case 0:
			MyDX::Dx12Wrapper::DrawFont({ L"タイトル", DirectX::XMFLOAT2(1550, 160), {1, 0.65f, 0, 1},{},{},0.6f });
			break;
		case 1:
			MyDX::Dx12Wrapper::DrawFont({ L"終了", DirectX::XMFLOAT2(1600, 280), {1, 0.65f, 0, 1},{},{},0.6f });
		}
	}

	void MenuUI::UpdateSelectNum()
	{
		if ((Input::Mgr::GetPadLog().dpadDown == DirectX::GamePad::ButtonStateTracker::PRESSED)
			|| (Input::Mgr::GetKeyLog().IsKeyPressed(DirectX::Keyboard::Down)))
		{
			MyObj::Sound::Play(1, false, true);	//選択変更SE
			// 選択番号を更新後、0を下回ったら2にする
			if (--this->selectNum; this->selectNum < 0)
			{
				this->selectNum = 1;
			}
		}
		else if ((Input::Mgr::GetPadLog().dpadUp == DirectX::GamePad::ButtonStateTracker::PRESSED)
			|| (Input::Mgr::GetKeyLog().IsKeyPressed(DirectX::Keyboard::Up)))
		{
			MyObj::Sound::Play(1, false, true);	//選択変更SE
			// 選択番号を更新後、2を上回ったら0にする
			if (++this->selectNum; this->selectNum > 1)
			{
				this->selectNum = 0;
			}
		}
	}
}