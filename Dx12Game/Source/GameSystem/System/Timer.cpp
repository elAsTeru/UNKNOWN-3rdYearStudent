#include "Timer.h"
#include <Logger.h>

namespace Sys
{
	using std::chrono::system_clock;
	using std::chrono::microseconds;

	Timer* Timer::singleton = nullptr; // Timerクラスの実体

	Timer::Timer() :
		startTime(),
		currentTime(),
		deltaTime(0),
		Delay(1.0f),
		MixSpeed(0.1f),
		MaxSpeed(0.5f),
		MaxDeltaTime(0.1f),
		Duration(3.0f),
		timeCounter(Delay + Duration),
		hitStopTime(0.0f)
	{
		singleton = nullptr;
	}

	Timer::~Timer()
	{
	}

	void Timer::OnInit()
	{
		if (!singleton)
		{
			singleton = new Timer;
			Init();
		}
	}

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

	// フレームの処理時間、計測開始
	void Timer::Update()
	{
		singleton->CalcDeltaTime();
		singleton->FixDeltaTime();


		// 仮置きhitstop処理
		// 計測時間が持続時間を超えてなければ
		if (singleton->timeCounter < singleton->Delay + singleton->Duration)
		{
			// 時間を計測し停止時間を超えてなければ
			if (singleton->timeCounter += singleton->deltaTime; singleton->timeCounter < singleton->Delay)
			{
				singleton->hitStopTime = 0;
				return;
			}

			// 持続時間と範囲から現在の処理速度を決定
			singleton->hitStopTime = singleton->deltaTime * std::lerp(singleton->MixSpeed, singleton->MaxSpeed, singleton->timeCounter / (singleton->Delay + singleton->Duration));
			return;
		}
		// ここに来るのは計測時間がhitstopの処理時間を超えた時のみなので
		singleton->hitStopTime = singleton->deltaTime;
	}

	void Timer::RunHitStop()
	{
		singleton->timeCounter = 0;
	}

	void Timer::CalcDeltaTime()
	{
		// 前回の時刻を記録し、最新時刻を更新
		system_clock::time_point prevTime = singleton->currentTime;
		singleton->currentTime = system_clock::now();
		// 1ループの処理かかった時間を求め、デルタタイムに変換
		system_clock::duration elapsedTime = singleton->currentTime - prevTime;
		// 1ループの処理時間が短い場合、sec・milSecだと0秒になる可能性があるので、micSecに経過時間を変換後、secに変換する
		singleton->deltaTime = duration_cast<microseconds>(elapsedTime).count();
		singleton->MicSecToSec(singleton->deltaTime);
	}

	void Timer::MicSecToSec(float& _MicSec)
	{
		_MicSec /= 1000000.0f;
	}

	void Timer::FixDeltaTime()
	{
		// １ループに時間がかかりすぎていたら、補正する
		if (singleton->deltaTime >= singleton->MaxDeltaTime)
		{
			DLOG("Warning : Fix deltaTime %.1f", singleton->MaxDeltaTime);
			singleton->deltaTime = singleton->MaxDeltaTime;
		}

		// 細かく必要になったら、平均を記録して突然超えるものがあった場合、
		// ログに表示して、デルタタイムを平均値に補正するようにする。
	}

	//void Timer::Wait(const int& _MilliSecond)
	//{
	//}
}