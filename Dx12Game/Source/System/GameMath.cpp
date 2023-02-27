#include "GameMath.h"

namespace MyGameMath
{
	const float& FadeMagnification(const bool& _FadeFlag, const float& _Value, const float& _MaxValue, const float& _MinRange = 0.0f, const float& _MaxRange = 1.0f)
	{
		float mag;	// �{��
		mag = _Value / _MaxValue;	// 0~1�ɕ␳
		// �t�F�[�h�A�E�g�����Ȃ�l�𔽓]������
		if (!_FadeFlag)
		{
			mag = 1.0f - mag;
		}
		// �w�肵���͈͂ɕ␳
		mag = mag * (_MaxRange - _MinRange) + _MinRange;

		return mag;
	}
}