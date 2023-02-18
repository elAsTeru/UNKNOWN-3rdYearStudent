#include "InputMgr.h"
#include "Timer.h"

namespace Input
{
	Mgr* Mgr::singleton = nullptr;

	Mgr::Mgr():
		SpeedKeyAxis(5),
		pad(),
		padLog(),
		key(),
		keyLog()
	{
		singleton = nullptr;
	}

	Mgr::~Mgr()
	{
	}

	void Mgr::OnInit()
	{
		if (!singleton)
		{
			singleton = new Mgr;
			singleton->pad = std::make_unique<DirectX::GamePad>();
			singleton->key = std::make_unique<DirectX::Keyboard>();
		}
	}

	void Mgr::OnTerm()
	{
		if (singleton)
		{
			singleton->padLog.Reset();
			singleton->pad.release();

			delete singleton;
			singleton = nullptr;
		}
	}

	void Mgr::Update()
	{
		// パッドの状態を取得
		const auto& padState = singleton->pad->GetState(0);
		// 接続されているなら
		if (padState.IsConnected())
		{ singleton->padLog.Update(padState);}	// 履歴を更新
		else
		{ singleton->padLog.Reset();}			// 履歴を削除

		// キーの状態を取得
		const auto& keyState = singleton->key->GetState();
		// 履歴更新
		singleton->keyLog.Update(keyState);

		// Axisの更新
		const auto& stick = padState.thumbSticks;
		// PADによる左スティック更新がなければキーボードによる更新を行う
		if (!singleton->UpdateStickAxisL(DirectX::SimpleMath::Vector2(stick.leftX, stick.leftY)))
		{
			singleton->UpdateKeyAxisL(keyState);
		}
		// PADによる右スティック更新がなければキーボードによる更新を行う
		if (!singleton->UpdateStickAxisR(DirectX::SimpleMath::Vector2(stick.rightX, stick.rightY)))
		{
			singleton->UpdateKeyAxisR(keyState);
		}
	}

	bool Mgr::UpdateStickAxisL(const DirectX::SimpleMath::Vector2& _AxisL)
	{
		// 左スティックの入力が無ければ終了
		if (_AxisL.x == 0.000f && _AxisL.y == 0.000f)
		{
			return false;
		}

		singleton->axisL.x = _AxisL.x;
		singleton->axisL.y = _AxisL.y;

		return true;
	}

	bool Mgr::UpdateStickAxisR(const DirectX::SimpleMath::Vector2& _AxisR)
	{
		// 右スティックの入力が無ければ終了
		if (_AxisR.x == 0.000f && _AxisR.y == 0.000f)
		{
			return false;
		}

		singleton->axisR.x = _AxisR.x;
		singleton->axisR.y = _AxisR.y;

		return true;
	}

	void Mgr::UpdateKeyAxisL(const DirectX::Keyboard::State& _Key)
	{
		DirectX::SimpleMath::Vector2& axisL = singleton->axisL;
		axisL = {};
		// 左右入力が同時に押されてないか？
		if (_Key.A != _Key.D)
		{
			// 入力量を増加する
			if (_Key.D)
			{
				axisL.x += singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// 入力量が上限を超えていたら補正
				if (axisL.x > 1)
				{
					axisL.x = 1;
				}
			}
			else
			{
				axisL.x -= singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// 入力量が下限を超えていたら補正
				if (axisL.x < -1)
				{
					axisL.x = -1;
				}
			}
		}
		// 上下入力が同時に押されてないか？
		if (_Key.W != _Key.S)
		{
			// 入力量を増加する
			if (_Key.W)
			{
				axisL.y += singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// 入力量が上限を超えていたら補正
				if (axisL.y > 1)
				{
					axisL.y = 1;
				}
			}
			else
			{
				axisL.y -= singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// 入力量が下限を超えていたら補正
				if (axisL.y < -1)
				{
					axisL.y = -1;
				}
			}
		}
		// 入力量を正規化する
		axisL.Normalize();
	}

	void Mgr::UpdateKeyAxisR(const DirectX::Keyboard::State& _Key)
	{
		DirectX::SimpleMath::Vector2& axisR = singleton->axisR;
		axisR = {};
		// 左右入力が同時に押されてないか？
		if (_Key.Left != _Key.Right)
		{
			// 入力量を増加する
			if (_Key.Right)
			{
				axisR.x += singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// 入力量が上限を超えていたら補正
				if (axisR.x > 1)
				{
					axisR.x = 1;
				}
			}
			else
			{
				axisR.x -= singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// 入力量が下限を超えていたら補正
				if (axisR.x < -1)
				{
					axisR.x = -1;
				}
			}
		}
		// 上下入力が同時に押されてないか？
		if (_Key.Up != _Key.Down)
		{
			// 入力量を増加する
			if (_Key.Up)
			{
				axisR.y += singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// 入力量が上限を超えていたら補正
				if (axisR.y > 1)
				{
					axisR.y = 1;
				}
			}
			else
			{
				axisR.y -= singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// 入力量が下限を超えていたら補正
				if (axisR.y < -1)
				{
					axisR.y = -1;
				}
			}
		}
		// 入力量を正規化する
		axisR.Normalize();
	}
}