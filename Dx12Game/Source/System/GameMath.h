#pragma once

namespace MyGameMath
{
    /// <summary>
    /// �t�F�[�h��������w�肵���͈͂Ŕ{����ԋp����
    /// </summary>
    /// <param name="_FadeFlag">true:fadein / false:fadeout</param>
    /// <param name="_Value">���ݒn</param>
    /// <param name="_MaxValue">�ő�l</param>
    /// <param name="_MinRange">�ő�{��</param>
    /// <param name="_MaxRange">�ŏ��{��</param>
    const float& FadeMagnification(const bool& _FadeFlag, const float& _Value, const float& _MaxValue, const float& _MinRange, const float& _MaxRange);
}