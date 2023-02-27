#pragma once

#include "ScoreType.h"
#include <unordered_map>

namespace Res
{
	enum class ScoreType;
}

namespace GameObject
{
	class ScoreUI;
	class ResultUI;
}

namespace MyObj
{
	class Score
	{
		Score();
		~Score();
		static Score* singleton;
	public:
		// Public Method;
		static void OnInit();
		static void OnTerm();

		static void Init();
		static void Reset();

		static const int GetTotalScore();											// 総合スコアを取得
		static const int GetEliminateScore();										// 敵を倒したときのスコア
		static const std::list<int> GetTopScore() { return singleton->topScore; }	// トップ3のスコアを取得
		static void AddEliminateNum(const Res::ScoreType _Type);					// 敵の討伐数などを加算する

		// UI系
		static void DrawScore();
		static void CalcResult(const int _ClearPhaseCount, const int _PlayerLifeCount);
		static void DrawResult();
	private:
		// Private Variable
		std::unordered_map<Res::ScoreType, int> scoreData;			// スコア表示用の値を格納
		std::unordered_map<Res::ScoreType, int> scoreMagnification;	// スコア倍率を格納
		std::list<int>topScore;											// トップ3のスコア
		static int CalcScore(const Res::ScoreType _Type);				// 種類を渡すと倒した数と倍率からスコアを計算して返す
		static void CalcTotalScore();									// トータルスコアを求めて、データに挿入する
		
		// UI系
		GameObject::ScoreUI* scoreUI;
		GameObject::ResultUI* resultUI;

		// Playerの残機系
		const uint32_t AddLifeScore;	// 残機を追加するスコア
		uint16_t scoreCounter;			// スコアのカウンター

		/// <summary>
		/// 元データが指定した桁数に満たない場合左側を0で埋める
		/// </summary>
		/// <param name="_WstrValue">数文字列</param>
		/// <param name="_Digits">桁数</param>
		static void FillLeftZero(std::wstring& _WstrValue, const int& _Digits);

		/// <summary>
		/// 3桁ごとに数文字列にコンマを入れる
		/// </summary>
		/// <param name="_WstrValue">数文字列</param>
		static void InsertComma(std::wstring& _WstrValue);
	};
}