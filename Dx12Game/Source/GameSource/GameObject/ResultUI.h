#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class ResultUI : public Base
	{
	public:
		ResultUI();
		~ResultUI();

		void Init()override {}
		void Update()override;
		void Draw()const override;
		void SetEliminateScore(const std::wstring _Score);
		void SetLifeScore(const std::wstring _Score);
		void SetPhaseScore(const std::wstring _Score);
		void SetBossScore(const std::wstring _Score);
		void SetTotalScore(const std::wstring _Score);
		// Constant Variable
		const int MaxDigits;	// スコアの桁数
	private:
		// Private Variable
		float timeCounter;
		std::wstring eliminate;	// 討伐スコア
		std::wstring life;		// 残機スコア
		std::wstring phase;		// フェーズクリアスコア
		std::wstring boss;		// ボス討伐タイムスコア(30秒ならmaxそこからどんどん引いていく)
		std::wstring total;		// 総合スコア

		// Private Method

		/// <summary>
		/// 元データが指定した桁数に満たない場合左側を0で埋める
		/// </summary>
		/// <param name="_WstrValue">数文字列</param>
		/// <param name="_Digits">桁数</param>
		void FillLeftZero(std::wstring& _WstrValue, const int& _Digits);

		/// <summary>
		/// 3桁ごとに数文字列にコンマを入れる
		/// </summary>
		/// <param name="_WstrValue">数文字列</param>
		void InsertComma(std::wstring& _WstrValue);
	};
}