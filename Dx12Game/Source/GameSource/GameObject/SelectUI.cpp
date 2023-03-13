#include "SelectUI.h"
#include "Dx12Wrapper.h"
#include "Tool/InputMgr.h"
#include "Tool/DXTK12Font.h"
#include "Resource/SceneFontData.h"

#include "GameObject/ScreenStrip.h"

namespace GameObject
{
	SelectUI::SelectUI() :
		Base(Tag::UI, "SelectUI"),
		Delay(1),
		Duration(1),
		MaxScale(5.0f),
		RectNum(3),
		DistX(18),
		SlideDuration(1.0f),
		BaseScale(16,9),
		timeCounter(0),
		slideTimeCounter(0),
		expan(MaxScale),
		state(State::In),
		ExitSceneTime(30.0f)
	{
		this->strip = new ScreenStrip;
		this->strip->Init();
		this->strip->SetDist(1.25f);
		this->strip->SetAlpha(0.4f);
	}

	SelectUI::~SelectUI()
	{

	}

	void SelectUI::Update()
	{
		if (this->state == State::In)
		{
			// 計測時間が処理遅延を超えたら
			if (this->timeCounter += Sys::Timer::GetDeltaTime(); this->timeCounter >= this->Delay)
			{
				// 枠を縮小させながら3つ並べる
				this->expan = std::lerp(this->MaxScale, 1, (this->timeCounter -this->Delay) /this->Duration);
				// 枠を3つ並べていく処理
				for (int i = 0; i < this->RectNum; ++i)
				{
					// 設定された間隔で、(-)(0)(+)位置になるように配置する
					this->rectPos[i].x = std::lerp(0, (this->DistX * i) - this->DistX, this->timeCounter / (this->Delay + this->Duration));
				}
			}

			// 計測時間が越えていたら補正する
			if ((this->timeCounter - this->Delay) >= this->Duration)
			{
				this->timeCounter = 0;
				this->expan = 1;	// 縮小を終了
				for (int i = 0; i < this->RectNum; ++i)
				{
					// 座標を終了位置にする
					this->rectPos[i].x = (this->DistX * i) - this->DistX;
				}
				// イベント待機状態に遷移
				this->state = State::Idol;
			}
		}
		else if (this->state == State::Idol)
		{
			// ステージ選択番号の更新
			UpdateSelectNum();
			// 選択枠の更新
			UpdateSelectRect();

			// 進むシーンの選択
			if ((Input::Mgr::GetPadLog().a == DirectX::GamePad::ButtonStateTracker::RELEASED)
				|| (Input::Mgr::GetKeyLog().IsKeyReleased(DirectX::Keyboard::Enter)))
			{
				MyDX::Tool::Sound::PlaySE(Res::SEType::Enter);

				this->timeCounter = 0;
				this->state = State::Out;
			}

			// ゲーム終了
			if ((Input::Mgr::GetPadLog().start == DirectX::GamePad::ButtonStateTracker::PRESSED)
				|| (Input::Mgr::GetKeyLog().IsKeyReleased(DirectX::Keyboard::Escape)))
			{
				MyDX::Tool::Sound::PlaySE(Res::SEType::Enter);
				Scene::Mgr::Quit();
			}
		}
		else
		{
			// 計測時間が処理遅延を超えたら
			if (this->timeCounter += Sys::Timer::GetDeltaTime(); this->timeCounter >= this->Delay)
			{
				// 枠を拡大しながら１つにする
				this->expan = std::lerp( 1, this->MaxScale, (this->timeCounter - this->Delay) / this->Duration);
				// 枠を3つ並べていく処理
				for (int i = 0; i < this->RectNum; ++i)
				{
					// 全ての枠が中心に来るように移動する処理
					this->rectPos[i].x = std::lerp(0, (this->DistX * i) - this->DistX, this->timeCounter / (this->Delay + this->Duration));
				}
			}

			// 計測時間が越えていたら補正する
			if ((this->timeCounter - this->Delay) >= this->Duration)
			{
				this->timeCounter = 0;
				this->expan = this->MaxScale;	// 拡大を終了
				for (int i = 0; i < this->RectNum; ++i)
				{
					// 座標を終了位置にする
					this->rectPos[i].x = 0;
				}
				// 選択されている番号にあったシーンに遷移する
				Scene::Mgr::LoadScene("Stage" + std::to_string(selectNum + 1));
			}
		}

		// 一定時間操作がなければタイトルへ戻る
		if (this->noInputTime >= this->ExitSceneTime)
		{
			Scene::Mgr::LoadScene("Title");
		}
		noInputTime += Sys::Timer::GetDeltaTime();

		Draw();
	}

	void SelectUI::Draw() const
	{
		// 左、中央、右の３つの枠を表示
		for (int i = 0; i < 3; ++i)
		{
			DirectX::XMFLOAT4 lineColor{ 51 / 255.0f,38 / 255.0f,64 / 255.0f,1 };
			float expansion = -0.05f;	// 中枠に足される拡大率

			// 選択されている枠に変化をつける
			if ((this->state == State::Idol) && (i == this->selectNum))
			{
				lineColor = { 25 / 255.0f,1, 9 / 255.0f,1 };
				expansion = +0.05f;
			}

			// 外枠、中枠の順に表示
			for (int j = 0; j < 2; ++j)
			{
				float magnification = this->expan + (expansion * j);
				DirectX::XMMATRIX matrix
					= Matrix::CreateScale(this->BaseScale.x * magnification, 1, this->BaseScale.y * magnification)
					* Matrix::CreateTranslation(rectPos[i]);
				MyDX::Dx12Wrapper::DrawRect(matrix, lineColor);
			}

		}

		if (this->state == State::Idol)
		{
			// 選択枠の表示
			DirectX::XMMATRIX matrix
				= Matrix::CreateScale(this->BaseScale.x * 1.15f * this->magnificationScale, 1.0f, this->BaseScale.y * 1.15f * this->magnificationScale)
				* Matrix::CreateTranslation(this->selectRectX, 0, 0);
			MyDX::Dx12Wrapper::DrawRect(matrix, { 25.0f / 255.0f,1, 9.0f / 255.0f,1 });
		}

		// 上下の帯表示
		this->strip->SetDist(1.25f + (0.2f * this->expan) - 0.2f);
		this->strip->Draw();

		// FontDataを描画する
		for (const auto& font : Res::fontData[Res::FontType::Select])
		{
			// 選択したステージにオレンジ色の文字を上乗せする
			if (this->state == State::Idol
				&& font.color == TransOrange
				&& font.str.starts_with(L"ステージ ")
				&& !font.str.ends_with(std::to_wstring(this->selectNum + 1)))
			{
				continue;
			}

			// fontの位置を移動させるための仮変数
			MyDX::FontData temp = font;
			// 座標が画面半分より上なら、上から下へスライドして出てくる
			if (font.pos.y <= 540)
			{
				temp.pos.y = font.pos.y - (100 * this->expan) + 100;
			}
			else
			{
				temp.pos.y = font.pos.y + (100 * this->expan) - 100;
			}

			// 描画
			MyDX::DXTK12Font::DrawFont(temp);
		}
	}

	void SelectUI::UpdateSelectNum()
	{
		// 選択を左にずらす
		if ((Input::Mgr::GetPadLog().dpadLeft == DirectX::GamePad::ButtonStateTracker::PRESSED)
			|| (Input::Mgr::GetKeyLog().IsKeyPressed(DirectX::Keyboard::Left)))
		{
			this->slideTimeCounter = 0.0f;
			this->noInputTime = 0.0f;
			MyDX::Tool::Sound::PlaySE(Res::SEType::Select);
			// 選択番号を更新後、0を下回ったら2にする
			if (--this->selectNum; this->selectNum < 0)
			{
				this->selectNum = this->RectNum - 1;
			}
		}
		// 選択を右にずらす
		else if ((Input::Mgr::GetPadLog().dpadRight == DirectX::GamePad::ButtonStateTracker::PRESSED)
			|| (Input::Mgr::GetKeyLog().IsKeyPressed(DirectX::Keyboard::Right)))
		{
			this->slideTimeCounter = 0.0f;
			this->noInputTime = 0.0f;
			MyDX::Tool::Sound::PlaySE(Res::SEType::Select);
			// 選択番号を更新後、2を上回ったら0にする
			if (++this->selectNum; this->selectNum > 2)
			{
				this->selectNum = 0;
			}
		}
	}

	void SelectUI::UpdateSelectRect()
	{
		// 選択されている番号の枠の位置と、選択を示す枠の場所が違うなら
		if (this->selectRectX != this->rectPos[this->selectNum].x)
		{
			this->slideTimeCounter += Sys::Timer::GetDeltaTime();
			// 現在選択されている枠への線形補間移動を行う
			this->selectRectX = std::lerp(this->selectRectX, this->rectPos[this->selectNum].x, this->slideTimeCounter / this->SlideDuration);
			// 所要時間を越えていたら
			if (this->slideTimeCounter >= this->SlideDuration)
			{
				this->slideTimeCounter = 0;
				this->selectRectX = this->rectPos[this->selectNum].x;
			}
		}
		else
		{
			this->slideTimeCounter = 0;
			// 円運動を利用して選択枠の拡大率を変更する
			this->timeCounter += Sys::Timer::GetDeltaTime();
			this->magnificationScale = MyMath::CircularMotion(0.98f, 1.0f, this->timeCounter, 15.0f);
		}
	}
}