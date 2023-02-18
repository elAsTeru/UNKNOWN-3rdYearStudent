#include "Timer.h"
#include "InputMgr.h"
#include "Logger.h"

namespace MySys
{
	using namespace std::chrono;

	// DeltaTime���Ԃւ̃|�C���^��`
	Timer* Timer::singleton = nullptr;

	// �R���X�g���N�^
	Timer::Timer() :
		startTime(),
		currentTime(),
		deltaTime(0),
		Delay(1.0f),
		MixSpeed(0.1f),
		MaxSpeed(0.5f),
		Duration(3.0f)
	{
		singleton = nullptr;
	}

	// �f�X�g���N�^
	Timer::~Timer()
	{
	}

	/// <summary>
	/// ������:���̊֐����g�p�セ�̑��֐����g�p�\�ɂȂ�B
	/// </summary>
	void Timer::OnInit()
	{
		if (!singleton)
		{
			singleton = new Timer;
			Init();
		}
	}

	/// <summary>
	/// �I��:���������s�����ꍇ�A�A�v���I���O�ɌĂяo���Ȃ���΃��������[�N����������
	/// </summary>
	void Timer::OnTerm()
	{
		if (singleton)
		{
			delete singleton;
			singleton = nullptr;
		}
	}

	void Timer::Init()
	{
		singleton->startTime = system_clock::now();
		singleton->currentTime = singleton->startTime;

		singleton->timeCounter = singleton->Delay + singleton->Duration;
	}

	// �t���[���̏������ԁA�v���J�n
	void Timer::Update()
	{
		// �O��̎������L�^
		system_clock::time_point prevTime = singleton->currentTime;
		// �ŐV�������X�V
		singleton->currentTime = system_clock::now();
		// 1���[�v�̏��������������Ԃ����߂�
		system_clock::duration elapsedTime = singleton->currentTime - prevTime;
		// 1���[�v�̏����ɂ����������Ԃ��f���^�^�C���ɕϊ�
		singleton->deltaTime = duration_cast<microseconds>(elapsedTime).count() / 1000000.0f;

		// �P���[�v�Ɏ��Ԃ������肷���Ă�����A�␳����
		if (singleton->deltaTime >= 0.1f)
		{
			OutputLog("Fix deltaTime 0.1 \n");
			singleton->deltaTime = 0.1f;
		}
		// �o����΁A���ς��L�^���Ă����A���ς���ˑR��������̂��������ꍇ�̓��O�ɓ`���āA
		// �f���^�^�C���𕽋ϒl�ɂ��Ă���

		// ���u��hitstop����
		// �v�����Ԃ��������Ԃ𒴂��ĂȂ����
		if (singleton->timeCounter < singleton->Delay + singleton->Duration)
		{
			// ���Ԃ��v������~���Ԃ𒴂��ĂȂ����
			if (singleton->timeCounter += singleton->deltaTime; singleton->timeCounter < singleton->Delay)
			{
				singleton->hitStopTime = 0;
				return;
			}

			// �������ԂƔ͈͂��猻�݂̏������x������
			singleton->hitStopTime = singleton->deltaTime * std::lerp(singleton->MixSpeed, singleton->MaxSpeed, singleton->timeCounter / (singleton->Delay + singleton->Duration));
			return;
		}
		// �����ɗ���̂͌v�����Ԃ�hitstop�̏������Ԃ𒴂������݂̂Ȃ̂�
		singleton->hitStopTime = singleton->deltaTime;
	}

	void Timer::RunHitStop()
	{
		singleton->timeCounter = 0;
	}

	//void Timer::Wait(const int& _MilliSecond)
	//{
	//}
}