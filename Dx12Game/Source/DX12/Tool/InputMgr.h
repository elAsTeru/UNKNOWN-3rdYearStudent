#pragma once
#include <DirectXTK12/GamePad.h>
#include <DirectXTK12/Keyboard.h>
#include "DirectXTK12/SimpleMath.h"

namespace Input
{
	class Mgr
	{
		Mgr();
		~Mgr();
		static Mgr* singleton;
	public:
		static void OnInit();	// クラス使用開始
		static void OnTerm();	// クラス使用終了

		// Public Method

		static void Update();
		/// <summary> パッドの状態を取得(現在１つのみ) </summary>
		static DirectX::GamePad::State GetPadState() { return singleton->pad->GetState(0); }
		static std::unique_ptr<DirectX::GamePad>& GetPad() { return singleton->pad; }
		/// <summary> パッドの入力履歴を取得 </summary>
		static const DirectX::GamePad::ButtonStateTracker& GetPadLog() { return singleton->padLog; }
		static const DirectX::Keyboard::State& GetKeyState() { return singleton->key->GetState(); }
		static const DirectX::Keyboard::KeyboardStateTracker& GetKeyLog() { return singleton->keyLog; }

		static const DirectX::SimpleMath::Vector2& GetAxisL() { return singleton->axisL; }
		static const DirectX::SimpleMath::Vector2& GetAxisR() { return singleton->axisR; }

		// Constant Variable

		const float SpeedKeyAxis;

		// Private Variable

		std::unique_ptr<DirectX::GamePad>		pad;	// パッド
		DirectX::GamePad::ButtonStateTracker	padLog;	// パッド入力履歴

		std::unique_ptr<DirectX::Keyboard>		key;	// キーボード
		DirectX::Keyboard::KeyboardStateTracker	keyLog;	// キー入力履歴

		// その他入力記録

		DirectX::SimpleMath::Vector2 axisL;
		DirectX::SimpleMath::Vector2 axisR;

		bool UpdateStickAxisL(const DirectX::SimpleMath::Vector2& _AxisL);
		bool UpdateStickAxisR(const DirectX::SimpleMath::Vector2& _AxisR);
		void UpdateKeyAxisL(const DirectX::Keyboard::State& _Key);
		void UpdateKeyAxisR(const DirectX::Keyboard::State& _Key);
	};
}