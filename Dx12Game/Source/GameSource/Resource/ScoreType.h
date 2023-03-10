#pragma once

namespace Res
{
	enum class ScoreType : unsigned char
	{
		Begin,

		LifeNum = Begin,	// ライフ数
		Phase,

		Thurough,			// 通過敵の討伐数
		SlowTracker,		// 追尾敵(遅い)の討伐数
		AvoidTracker,		// 追尾敵(避け)の討伐数
		Boss,

		Rander,
		Reverser,
		Tracker,
		Missile,

		Total,				// 最終スコア

		End
	};
}