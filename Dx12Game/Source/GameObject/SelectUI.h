#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class SelectUI : public Base
	{
	public:
		SelectUI();
		~SelectUI();

		void Start() {}
		void Init()override {}
		void Update();
		const float Delay;			// 処理開始を遅延させる
		const float Duration;		// 処理時間
		const float MaxScale;		// 最大サイズ
		const int RectNum;			// 枠の数
		const float DistX;			// 間隔
		const float SlideDuration;	// 選択されている場所に向かうのにかかる所要時間
		const Vector2 BaseScale;	// 枠の基準値
	private:
		// Private Variable	
		enum class State
		{
			In,			// シーンに入った
			Idol,		// イベント待ち
			Out			// シーンを出る
		};
		State state;				// シーンの状態
		float timeCounter;			// 時間計測
		float slideTimeCounter;		// 選択枠の移動用時間計測
		Vector3 rectPos[3];			// 3つの枠の座標
		float selectRectX;			// 選択を示す枠のX座標
		float expan;				// 枠のサイズ
		int selectNum;				// 選択番号
		float magnificationScale;	// 拡大率
		// Private Method
		void Draw();
		void UpdateSelectNum();
		void UpdateSelectRect();

		const float ExitSceneTime;			// シーンを出るまでの時間
		float noInputTime;					// 操作されてない時間
	};
}