#pragma once

namespace MyGameMath
{
    /// <summary>
    /// フェード処理から指定した範囲で倍率を返却する
    /// </summary>
    /// <param name="_FadeFlag">true:fadein / false:fadeout</param>
    /// <param name="_Value">現在地</param>
    /// <param name="_MaxValue">最大値</param>
    /// <param name="_MinRange">最大倍率</param>
    /// <param name="_MaxRange">最小倍率</param>
    const float& FadeMagnification(const bool& _FadeFlag, const float& _Value, const float& _MaxValue, const float& _MinRange, const float& _MaxRange);
}