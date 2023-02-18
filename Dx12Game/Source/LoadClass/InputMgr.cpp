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
		// �p�b�h�̏�Ԃ��擾
		const auto& padState = singleton->pad->GetState(0);
		// �ڑ�����Ă���Ȃ�
		if (padState.IsConnected())
		{ singleton->padLog.Update(padState);}	// �������X�V
		else
		{ singleton->padLog.Reset();}			// �������폜

		// �L�[�̏�Ԃ��擾
		const auto& keyState = singleton->key->GetState();
		// �����X�V
		singleton->keyLog.Update(keyState);

		// Axis�̍X�V
		const auto& stick = padState.thumbSticks;
		// PAD�ɂ�鍶�X�e�B�b�N�X�V���Ȃ���΃L�[�{�[�h�ɂ��X�V���s��
		if (!singleton->UpdateStickAxisL(DirectX::SimpleMath::Vector2(stick.leftX, stick.leftY)))
		{
			singleton->UpdateKeyAxisL(keyState);
		}
		// PAD�ɂ��E�X�e�B�b�N�X�V���Ȃ���΃L�[�{�[�h�ɂ��X�V���s��
		if (!singleton->UpdateStickAxisR(DirectX::SimpleMath::Vector2(stick.rightX, stick.rightY)))
		{
			singleton->UpdateKeyAxisR(keyState);
		}
	}

	bool Mgr::UpdateStickAxisL(const DirectX::SimpleMath::Vector2& _AxisL)
	{
		// ���X�e�B�b�N�̓��͂�������ΏI��
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
		// �E�X�e�B�b�N�̓��͂�������ΏI��
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
		// ���E���͂������ɉ�����ĂȂ����H
		if (_Key.A != _Key.D)
		{
			// ���͗ʂ𑝉�����
			if (_Key.D)
			{
				axisL.x += singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// ���͗ʂ�����𒴂��Ă�����␳
				if (axisL.x > 1)
				{
					axisL.x = 1;
				}
			}
			else
			{
				axisL.x -= singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// ���͗ʂ������𒴂��Ă�����␳
				if (axisL.x < -1)
				{
					axisL.x = -1;
				}
			}
		}
		// �㉺���͂������ɉ�����ĂȂ����H
		if (_Key.W != _Key.S)
		{
			// ���͗ʂ𑝉�����
			if (_Key.W)
			{
				axisL.y += singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// ���͗ʂ�����𒴂��Ă�����␳
				if (axisL.y > 1)
				{
					axisL.y = 1;
				}
			}
			else
			{
				axisL.y -= singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// ���͗ʂ������𒴂��Ă�����␳
				if (axisL.y < -1)
				{
					axisL.y = -1;
				}
			}
		}
		// ���͗ʂ𐳋K������
		axisL.Normalize();
	}

	void Mgr::UpdateKeyAxisR(const DirectX::Keyboard::State& _Key)
	{
		DirectX::SimpleMath::Vector2& axisR = singleton->axisR;
		axisR = {};
		// ���E���͂������ɉ�����ĂȂ����H
		if (_Key.Left != _Key.Right)
		{
			// ���͗ʂ𑝉�����
			if (_Key.Right)
			{
				axisR.x += singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// ���͗ʂ�����𒴂��Ă�����␳
				if (axisR.x > 1)
				{
					axisR.x = 1;
				}
			}
			else
			{
				axisR.x -= singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// ���͗ʂ������𒴂��Ă�����␳
				if (axisR.x < -1)
				{
					axisR.x = -1;
				}
			}
		}
		// �㉺���͂������ɉ�����ĂȂ����H
		if (_Key.Up != _Key.Down)
		{
			// ���͗ʂ𑝉�����
			if (_Key.Up)
			{
				axisR.y += singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// ���͗ʂ�����𒴂��Ă�����␳
				if (axisR.y > 1)
				{
					axisR.y = 1;
				}
			}
			else
			{
				axisR.y -= singleton->SpeedKeyAxis * MySys::Timer::GetDeltaTime();
				// ���͗ʂ������𒴂��Ă�����␳
				if (axisR.y < -1)
				{
					axisR.y = -1;
				}
			}
		}
		// ���͗ʂ𐳋K������
		axisR.Normalize();
	}
}