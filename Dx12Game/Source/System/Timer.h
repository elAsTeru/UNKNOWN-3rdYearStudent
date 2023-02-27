#pragma once
// ---------------------------------------------------------------------------------------------
// ループの時間を管理し、デルタタイムやヒットストップ等時間に関する処理を行うシングルトンクラス
// ---------------------------------------------------------------------------------------------
#include <chrono>

namespace Sys
{
	class Timer final
	{
		// Singleton
		Timer();
		~Timer();
		static Timer* singleton;

	public:
		// Public Method

		static void OnInit();	// クラス使用開始
		static void OnTerm();	// クラス使用終了

		/// <summary> 計測開始時刻の初期化 </summary>
		static void Init();
		/// <summary> 最新時刻、デルタタイムの更新 </summary>
		static void Update();
		/// <summary> 指定した時間処理を停止する(FPS処理が必要になったら作成する) </summary>
		//static void Wait(const int& _MilliSecond);

		/// <summary> デルタタイム取得 </summary>
		static const float& GetDeltaTime() { return singleton->deltaTime; }
		/// <summary> ヒットストップタイム取得 </summary>
		static const float& GetHitStopTime() { return singleton->hitStopTime; }
		/// <summary> ヒットストップ開始 </summary>
		static void RunHitStop();

	private:
		// Private Variable

		std::chrono::system_clock::time_point startTime;	// 計測開始時刻
		std::chrono::system_clock::time_point currentTime;	// 最新時刻
		float deltaTime;									// 前回のループにかかった時間

		const float Delay;		// 停止時間
		const float MixSpeed;	// スロー時間の最小スピード	
		const float MaxSpeed;	// スロー時間の最大スピード
		const float Duration;	// スロー時間
		float timeCounter;
		float hitStopTime;
	};
}
