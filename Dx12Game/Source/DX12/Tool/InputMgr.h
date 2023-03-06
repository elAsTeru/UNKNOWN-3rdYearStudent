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
		static void OnInit();	// �N���X�g�p�J�n
		static void OnTerm();	// �N���X�g�p�I��

		// Public Method

		static void Update();
		/// <summary> �p�b�h�̏�Ԃ��擾(���݂P�̂�) </summary>
		static DirectX::GamePad::State GetPadState() { return singleton->pad->GetState(0); }
		static std::unique_ptr<DirectX::GamePad>& GetPad() { return singleton->pad; }
		/// <summary> �p�b�h�̓��͗������擾 </summary>
		static const DirectX::GamePad::ButtonStateTracker& GetPadLog() { return singleton->padLog; }
		static const DirectX::Keyboard::State& GetKeyState() { return singleton->key->GetState(); }
		static const DirectX::Keyboard::KeyboardStateTracker& GetKeyLog() { return singleton->keyLog; }

		static const DirectX::SimpleMath::Vector2& GetAxisL() { return singleton->axisL; }
		static const DirectX::SimpleMath::Vector2& GetAxisR() { return singleton->axisR; }

		// Constant Variable

		const float SpeedKeyAxis;

		// Private Variable

		std::unique_ptr<DirectX::GamePad>		pad;	// �p�b�h
		DirectX::GamePad::ButtonStateTracker	padLog;	// �p�b�h���͗���

		std::unique_ptr<DirectX::Keyboard>		key;	// �L�[�{�[�h
		DirectX::Keyboard::KeyboardStateTracker	keyLog;	// �L�[���͗���

		// ���̑����͋L�^

		DirectX::SimpleMath::Vector2 axisL;
		DirectX::SimpleMath::Vector2 axisR;

		bool UpdateStickAxisL(const DirectX::SimpleMath::Vector2& _AxisL);
		bool UpdateStickAxisR(const DirectX::SimpleMath::Vector2& _AxisR);
		void UpdateKeyAxisL(const DirectX::Keyboard::State& _Key);
		void UpdateKeyAxisR(const DirectX::Keyboard::State& _Key);
	};
}