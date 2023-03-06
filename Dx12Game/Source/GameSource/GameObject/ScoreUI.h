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
		/// <summary> 総合スコアを設定 </summary>
		void SetTotalScore(const std::wstring _Score) { this->score = _Score; }
		/// <summary> スコアの加算表示処理 </summary>
		void AddScore(const std::wstring _Name, const int _Score);

		// Constant Variable

		const float LifeTime;		// 表示時間
		const float SlideDuration;	// スコアのスライドにかかる時間
	private:
		// Private Variable

		struct DrawDesc
		{
			float remaining;		// 残りの表示時間
			std::wstring drawStr;	// 表示する文字列
			Vector2 pos;			// 表示される位置
			XMVECTOR color;			// 表示カラー
		};
		std::vector<DrawDesc> drawDescs;

		std::wstring score;    // 討伐スコア
		float timeCounter;
	};
}