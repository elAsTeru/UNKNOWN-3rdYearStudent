#pragma once
#include <Xinput.h>

namespace MyRes
{
	enum class Button : unsigned int
	{
		UP			= XINPUT_GAMEPAD_DPAD_UP,
		DOWN		= XINPUT_GAMEPAD_DPAD_DOWN,
		LEFT		= XINPUT_GAMEPAD_DPAD_LEFT,
		RIGHT		= XINPUT_GAMEPAD_DPAD_RIGHT,
		START		= XINPUT_GAMEPAD_START,
		BACK		= XINPUT_GAMEPAD_BACK,
		L_THUMB		= XINPUT_GAMEPAD_LEFT_THUMB,
		R_THUMB		= XINPUT_GAMEPAD_RIGHT_THUMB,
		L_SHOULDER	= XINPUT_GAMEPAD_LEFT_SHOULDER,
		R_SHOULDER	= XINPUT_GAMEPAD_RIGHT_SHOULDER,
		A			= XINPUT_GAMEPAD_A,
		B			= XINPUT_GAMEPAD_B,
		X			= XINPUT_GAMEPAD_X,
		Y			= XINPUT_GAMEPAD_Y
	};

	enum class EventType
	{
		PRESS,
		RELEASE
	};

	struct Axis
	{
		float x;
		float y;
	};

	// 入力イベントクラス
	class ButtonEvent
	{
	public:
		ButtonEvent(EventType _Type, MyRes::Button _Button):
			type(_Type),
			button(_Button)
		{}

		bool IsPress() const
		{
			return type == EventType::PRESS;
		}

		bool IsRelease() const
		{
			return type == EventType::RELEASE;
		}

		Button GetButton() const
		{
			return button;
		}

	private:
		// Private Variable
		EventType type;
		Button button;
	};
}