#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class ScreenStrip;
	class Board;

	class ResultUI : public Base
	{
	public:
		ResultUI();
		~ResultUI();

		void Init(){}
		void Update()override;
		void Draw()const override;

		void SetEliminateScore(const std::wstring _Score);
		void SetLifeScore(const std::wstring _Score);
		void SetPhaseScore(const std::wstring _Score);
		void SetBossScore(const std::wstring _Score);
		void SetTotalScore(const std::wstring _Score);
	private:
		// Private Variable
		std::wstring eliminate;	// 討伐スコア
		std::wstring life;		// 残機スコア
		std::wstring phase;		// フェーズクリアスコア
		std::wstring boss;		// ボス討伐タイムスコア(30秒ならmaxそこからどんどん引いていく)
		std::wstring total;		// 総合スコア

		ScreenStrip* strip;
		Board* board;
	};
}