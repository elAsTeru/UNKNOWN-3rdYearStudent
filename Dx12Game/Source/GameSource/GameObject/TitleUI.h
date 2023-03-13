#pragma once
#include "GameObjectBase.h"
#include <vector>

namespace MyDX
{
	struct FontData;
}

namespace GameObject
{
	class Board;

	class TitleUI : public Base
	{
	public:
		TitleUI();
		~TitleUI();

		void Init()override {}

		void Update()override;
		void Draw()const override;

		// Constant Variable

		const float FadeDelay;			// フェードが始まるまで
		const float FadeDuration;		// フェードにかかる時間
		const float FirstBlinkSpeed;	// 初期点滅速度
		const float MaxBlinkSpeed;		// 最大点滅速度
	private:
		enum class State
		{
			In,		// シーンに入った
			Idol,	// イベント待ち
			Out		// シーンを出る
		};

		// Private Variable	

		State state;		// シーンの状態
		float timeCounter;	// 時間計測
		float blinkSpeed;	// 点滅速度
		Vector4 blinkColor;	// 点滅時の色

		Board* board;
	};
}