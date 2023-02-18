#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class MenuUI : public Base
	{
	public:
		MenuUI();
		~MenuUI();

		void Start() {}
		void Init()override {}
		void Update();

		// Constant Variable
		const float FirstBlinkSpeed;	// 初期点滅速度
		const float MaxBlinkSpeed;		// 最大点滅速度
		const float FadeDelay;			// フェードが始まるまで
		const float FadeDuration;		// フェードにかかる時間
		const Vector4 BaseColor;		// 文字の色
	private:
		// Private Variable	
		enum class State
		{
			In,		// シーンに入った
			Idol,	// イベント待ち
			Out		// シーンを出る
		};
		State state;		// シーンの状態
		float fadeValue;	// フェードの値
		float timeCounter;	// 時間計測
		float blinkSpeed;	// 点滅速度
		Vector4 blinkColor;	// 点滅時の色

		int selectNum;

		// Private Method
		void Draw();
		void UpdateSelectNum();
	};
}