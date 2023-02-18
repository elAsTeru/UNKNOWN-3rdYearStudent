#include "Score.h"
#include "ScoreUI.h"
#include "ResultUI.h"
#include "Player.h"

//スコア倍率
static const int SCORE_MAGNIFICATION_THROUGH	  = 2500;	// 直進敵
static const int SCORE_MAGNIFICATION_SLOWTRACKER  = 1500;	// 追跡敵
static const int SCORE_MAGNIFICATION_AVOIDTRACKER = 4000;	// 追跡敵、弾除け
static const int SCORE_MAGNIFICATION_MISSILE		= 50;	// ミサイル
static const int SCORE_MAGNIFICATION_LIFE		  = 10000;	// 残機
static const int SCORE_MAGNIFICATION_PHASE		  = 5000;	// クリアフェーズ数
static const int SCORE_MAGNIFICATION_BOSS		  = 300000;	// ボス

static const int SCORE_MAGNIFICATION_REVERSER = 0;			// 反転敵
static const int SCORE_MAGNIFICATION_RANDER = 0;			// 反転敵
static const int SCORE_MAGNIFICATION_TRACKER = 0;			// 反転敵

static const int TOP_RECORD_NUM = 3;	// トップ記録数

namespace MyObj
{
	Score* Score::singleton = nullptr;

	Score::Score():
		AddLifeScore(50000)
	{
		// トップスコア数のサイズにする
		topScore.resize(TOP_RECORD_NUM);
	}

	Score::~Score()
	{
		scoreData.clear();
		scoreMagnification.clear();
	}

	void Score::OnInit()
	{
		if (!singleton)
		{
			singleton = new Score;
			// UIの生成
			singleton->scoreUI = new GameObject::ScoreUI;
			singleton->resultUI = new GameObject::ResultUI;
		}
		Init();
	}

	void Score::OnTerm()
	{
		if (singleton)
		{
			delete singleton->resultUI;
			singleton->resultUI = nullptr;
			delete singleton->scoreUI;
			singleton->scoreUI = nullptr;
			delete singleton;
			singleton = nullptr;
		}
	}

	void Score::Init()
	{
		// 各スコア倍率を設定
		singleton->scoreMagnification[MyRes::ScoreType::Thurough]		= SCORE_MAGNIFICATION_THROUGH;
		singleton->scoreMagnification[MyRes::ScoreType::SlowTracker]	= SCORE_MAGNIFICATION_SLOWTRACKER;
		singleton->scoreMagnification[MyRes::ScoreType::AvoidTracker]	= SCORE_MAGNIFICATION_AVOIDTRACKER;
		singleton->scoreMagnification[MyRes::ScoreType::LifeNum]		= SCORE_MAGNIFICATION_LIFE;
		singleton->scoreMagnification[MyRes::ScoreType::Phase]			= SCORE_MAGNIFICATION_PHASE;
		singleton->scoreMagnification[MyRes::ScoreType::Rander]			= SCORE_MAGNIFICATION_RANDER;
		singleton->scoreMagnification[MyRes::ScoreType::Reverser]		= SCORE_MAGNIFICATION_REVERSER;
		singleton->scoreMagnification[MyRes::ScoreType::Tracker]		= SCORE_MAGNIFICATION_TRACKER;
		singleton->scoreMagnification[MyRes::ScoreType::Missile]		= SCORE_MAGNIFICATION_MISSILE;
		singleton->scoreMagnification[MyRes::ScoreType::Boss]			= SCORE_MAGNIFICATION_BOSS;
	}

	void Score::Reset()
	{
		// スコアリセット
		for (auto type = MyRes::ScoreType::Begin; type != MyRes::ScoreType::End; ++type)
		{
			// 倒した数を0にする
			singleton->scoreData[type] = 0;
		}
		singleton->scoreUI->Init();
	}

	int const Score::GetTotalScore()
	{
		// 総合スコアをリセットする
		singleton->scoreData[MyRes::ScoreType::Total] = 0;
		// 総合スコアを求める
		CalcTotalScore();

		// トップスコアを更新
		//{
		//	bool isUpdate = false;
		//	// トップスコアと総合スコアを比較して更新する
		//	for (auto rank : topScore)
		//	{
		//		// スコアが越えていたら
		//		if (rank <= scoreData[MyRes::ScoreType::Total])
		//		{
		//			// 元々あったスコアで最下位を上書きし
		//			rank = scoreData[MyRes::ScoreType::Total];
		//		}
		//	}
		//}

		return singleton->scoreData[MyRes::ScoreType::Total];
	}

	const int Score::GetEliminateScore()
	{
		int score
			= CalcScore(MyRes::ScoreType::Thurough)
			+ CalcScore(MyRes::ScoreType::SlowTracker)
			+ CalcScore(MyRes::ScoreType::AvoidTracker);

		return score;
	}

	void Score::AddEliminateNum(const MyRes::ScoreType _Type)
	{
		singleton->scoreData[_Type] += 1;

		// スコアUIに渡すデータ
		std::wstring name = L"";
		if (_Type == MyRes::ScoreType::Thurough)
		{
			name = L"rht";
		}
		else if (_Type == MyRes::ScoreType::SlowTracker)
		{
			name = L"sart";
		}
		else if (_Type == MyRes::ScoreType::AvoidTracker)
		{
			name = L"aart";
		}
		else if (_Type == MyRes::ScoreType::Missile)
		{
			name = L"msil";
		}
		uint16_t score = singleton->scoreMagnification[_Type];
		// UIのスコア加算表示
		singleton->scoreUI->AddScore(name, score);


		if (name != L"")
		{
			// プレイヤーの残機加算処理
			singleton->scoreCounter += score;
			if (singleton->scoreCounter >= singleton->AddLifeScore)
			{
				if (auto player = static_cast<GameObject::Player*>(GameObject::Mgr::FindActiveObj("Player")))
				{
					player->AddLife();
					singleton->scoreCounter -= singleton->AddLifeScore;
				}
			}
		}
	}

	void Score::DrawScore()
	{
		// 討伐スコアを取得し、表示する形式に変更
		std::wstring eliminateScore = std::to_wstring(GetEliminateScore());
		FillLeftZero(eliminateScore, 7);
		InsertComma(eliminateScore);
		// 討伐スコアを渡して
		singleton->scoreUI->SetScore(eliminateScore);
		// 描画
		singleton->scoreUI->Update();
	}

	void Score::CalcResult(const int _ClearPhaseCount, const int _PlayerLifeCount)
	{
		// スコアの反映
		// 残機スコア
		singleton->scoreData[MyRes::ScoreType::LifeNum] = 0;
		for(int i = 0;i<_PlayerLifeCount;++i)
		{ AddEliminateNum(MyRes::ScoreType::LifeNum); }
		// フェーズスコア
		singleton->scoreData[MyRes::ScoreType::Phase] = 0;
		for (int i = 0; i < _ClearPhaseCount; ++i)
		{ AddEliminateNum(MyRes::ScoreType::Phase); }
		// 総スコアを計算
		CalcTotalScore();


		// リザルトUIにデータを渡す
		// 討伐スコア
		std::wstring score = std::to_wstring(GetEliminateScore());
		InsertComma(score);
		singleton->resultUI->SetEliminateScore(score);
		// 残機スコア
		score = std::to_wstring(CalcScore(MyRes::ScoreType::LifeNum));
		InsertComma(score);
		singleton->resultUI->SetLifeScore( score);
		// フェーズスコア
		score = std::to_wstring(CalcScore(MyRes::ScoreType::Phase));
		InsertComma(score);
		singleton->resultUI->SetPhaseScore(score);
		// ボス討伐スコア
		score = std::to_wstring(CalcScore(MyRes::ScoreType::Boss));
		InsertComma(score);
		singleton->resultUI->SetBossScore(score);

		// 総合スコア
		score = std::to_wstring(singleton->scoreData[MyRes::ScoreType::Total]);
		InsertComma(score);
		singleton->resultUI->SetTotalScore(score);
	}

	void Score::DrawResult()
	{
		singleton->resultUI->Update();
	}

	int Score::CalcScore(MyRes::ScoreType _Type)
	{
		return singleton->scoreData[_Type] * singleton->scoreMagnification[_Type];
	}

	void Score::CalcTotalScore()
	{
		singleton->scoreData[MyRes::ScoreType::Total] = 0; // 重複防止
		for (auto type = MyRes::ScoreType::Begin; type != MyRes::ScoreType::End; ++type)
		{
			singleton->scoreData[MyRes::ScoreType::Total] += CalcScore(type);
		}
	}

	void Score::FillLeftZero(std::wstring& _WstrValue, const int& _Digits)
	{
		std::wstring temp;	// 左を0で埋めた数文字列を一時保存
		// 指定した桁数に満たない分0を入れていく
		for (int i = 0; i < _Digits - _WstrValue.size(); ++i)
		{
			temp += '0';
		}
		// 元データの数値を追加していく
		for (int i = 0; i < _WstrValue.size(); ++i)
		{
			temp += _WstrValue[i];
		}
		// 元データを書き換えて終了
		_WstrValue = temp;
	}

	void Score::InsertComma(std::wstring& _WstrValue)
	{
		std::wstring reverse;		// 文字列を反転したもの
		std::wstring temp;			// コロンで区切った数文字列を一時保存

		// 反転
		for (int i = 0; i < _WstrValue.size(); ++i)
		{
			reverse += _WstrValue[(_WstrValue.size() - 1) - i];
		}
		// 3の倍数桁ごとにコンマを入れる
		for (int i = 0; i < reverse.size(); ++i)
		{
			if ((i != 0) && (i % 3 == 0))
			{
				temp += ',';
			}
			temp += reverse[i];
		}
		// データをもとに戻し、元データを書き換える
		_WstrValue.clear();
		for (int i = 0; i < temp.size(); ++i)
		{
			_WstrValue += temp[(temp.size() - 1) - i];
		}
	}
}