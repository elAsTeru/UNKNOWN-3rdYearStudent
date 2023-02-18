#include "Timer.h"
#include "InputMgr.h"
#include "Logger.h"

namespace MySys
{
	using namespace std::chrono;

	// DeltaTime実態へのポインタ定義
	Timer* Timer::singleton = nullptr;

	// コンストラクタ
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

	// デストラクタ
	Timer::~Timer()
	{
	}

	/// <summary>
	/// 初期化:この関数を使用後その他関数が使用可能になる。
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
	/// 終了:初期化を行った場合、アプリ終了前に呼び出さなければメモリリークが発生する
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

	// フレームの処理時間、計測開始
	void Timer::Update()
	{
		// 前回の時刻を記録
		system_clock::time_point prevTime = singleton->currentTime;
		// 最新時刻を更新
		singleton->currentTime = system_clock::now();
		// 1ループの処理かかった時間を求める
		system_clock::duration elapsedTime = singleton->currentTime - prevTime;
		// 1ループの処理にかかった時間をデルタタイムに変換
		singleton->deltaTime = duration_cast<microseconds>(elapsedTime).count() / 1000000.0f;

		// １ループに時間がかかりすぎていたら、補正する
		if (singleton->deltaTime >= 0.1f)
		{
			OutputLog("Fix deltaTime 0.1 \n");
			singleton->deltaTime = 0.1f;
		}
		// 出来れば、平均を記録しておき、平均から突然超えるものがあった場合はログに伝えて、
		// デルタタイムを平均値にしておく

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

	//void Timer::Wait(const int& _MilliSecond)
	//{
	//}
}