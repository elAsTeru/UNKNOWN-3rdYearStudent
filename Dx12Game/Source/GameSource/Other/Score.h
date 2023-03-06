#pragma once
#include "Resource/ScoreType.h"
#include <unordered_map>

namespace Res
{
	enum class ScoreType : unsigned char;
}

namespace GameObject
{
	class ScoreUI;
	class ResultUI;
}

namespace Other
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
		static void AddEliminateNum(const Res::ScoreType _Type);					// 敵の討伐数などを加算する

		// UI系
		static void DrawScore();
		static void CalcResult(const int _ClearPhaseCount, const int _PlayerLifeCount);
		static void DrawResult();
	private:
		// Private Variable
		std::unordered_map<Res::ScoreType, int> scoreData;				// スコア表示用の値を格納
		std::unordered_map<Res::ScoreType, int> scoreMagnification;		// スコア倍率を格納
		static int CalcScore(const Res::ScoreType _Type);				// 種類を渡すと倒した数と倍率からスコアを計算して返す
		static void CalcTotalScore();									// トータルスコアを求めて、データに挿入する
		
		// UI系
		GameObject::ScoreUI* scoreUI;
		GameObject::ResultUI* resultUI;

		// Playerの残機系
		const uint32_t AddLifeScore;	// 残機を追加するスコア
		uint16_t scoreCounter;			// スコアのカウンター
	};
}