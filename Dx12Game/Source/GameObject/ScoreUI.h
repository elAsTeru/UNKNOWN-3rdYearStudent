#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class ScoreUI : public Base
	{
	public:
		ScoreUI();
		~ScoreUI();

		void Init()override;
		void Update();
		void Draw()const override;
		void SetScore(const std::wstring _Score) { this->score = _Score; }	// スコアをセットする
		void AddScore(const std::wstring _Name, const int _Score);	// スコアを加算する

		// Constant Variable
		const float Duration;		// 表示時間
		const float SlideDuration;	// スコアのスライドにかかる時間
	private:
		// Private Variable
		struct ObsData
		{
			float remaining;
			std::wstring drawStr;
			Vector2 pos;
			XMVECTOR color;
		};
		std::vector<ObsData> obsDatas;

		std::wstring score;    // 討伐スコア
		float timeCounter;
	};
}