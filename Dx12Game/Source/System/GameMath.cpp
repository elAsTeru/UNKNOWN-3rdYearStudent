#include "GameMath.h"

namespace MyGameMath
{
	const float& FadeMagnification(const bool& _FadeFlag, const float& _Value, const float& _MaxValue, const float& _MinRange = 0.0f, const float& _MaxRange = 1.0f)
	{
		float mag;	// 倍率
		mag = _Value / _MaxValue;	// 0~1に補正
		// フェードアウト処理なら値を反転させる
		if (!_FadeFlag)
		{
			mag = 1.0f - mag;
		}
		// 指定した範囲に補正
		mag = mag * (_MaxRange - _MinRange) + _MinRange;

		return mag;
	}
}