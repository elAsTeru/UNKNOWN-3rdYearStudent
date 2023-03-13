#pragma once
#include <DirectXTK12/SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace MyMath
{
    const float Pi = 3.1415926535f;
    const int16_t MaxInt16 = 32767;

    /// <summary> ディグリー角をラジアン角化し、返却 </summary>
    inline float ToRadians(const float& _Degree)
    {
        return _Degree * Pi / 180.0f;
    }

    /// <summary> ラジアン角をディグリー角化し、返却 </summary>
    inline float ToDegrees(const float& _Radian)
    {
        return _Radian * 180.0f / Pi;
    }

    /// <summary> ラジアン角をベクトル化し、返却 </summary>
    inline Vector2 AngleToVecs2LH(const float& _Radian)
    {
        Vector2 unit;
        unit.x = std::cosf(_Radian);
        unit.y = -std::sinf(_Radian);
        return unit;
    }

    /// <summary> ベクトルをラジアン角化し、返却 </summary>
    inline float Vec2ToAnglesLH(const Vector2& _V)
    {
        // 左手座標系なので反転して返却
        return -std::atan2f(_V.y, _V.x);
    }

    /// <summary> 2つのベクトルのなす角 </summary>
    inline float AngleBetweenTwoVecs2(const Vector2& _V1, const Vector2& _V2)
    {
        return cos(_V1.Dot(_V2) / (_V1.Length() + _V2.Length()));
    }

    /// <summary> Vec1とVec2はほぼ同じか？ </summary>
    bool IsNearAngle(const Vector3& _V1, const Vector3& _V2);
    
    /// <summary> 現在の向きからAimVecに向かってDegVerocityの速度でY回転する </summary>
    Vector3 YRotateToAimVec(const Vector3& _Now, const Vector3& _Aim, float DegSpeed);
    
    /// <summary> Vec1→Vec2の最短の回転方向を調べる(Y軸回転を想定)。時計回り:+1.0f / 半時計回り:-1.0fを返却 </summary>
    float CalcRotateYDirection(const Vector3& _V1, const Vector3& _V2);
    
    /// <summary> 円運動処理 : 最小・最大範囲(0.0~1.0の範囲で指定)、経過時間、回転速度 </summary>
    float CircularMotion(float _MinRange, float _MaxRange , float _Time, float _MotionSpeed);
    
    /// <summary> 偏りの小さい乱数を作成 </summary>
    uint32_t CreateRandom(uint32_t _Min, uint32_t _Max);

    bool IsNearZero(const float& _F);
    bool IsNearZero(const Vector2& _Vec2);
    bool IsNearZero(const Vector3& _Vec3);

    /// <summary> 値の上昇で倍率が上昇(フェードイン)する処理。 </summary>
    /// <param name="_Value">現在値</param>
    /// <param name="_MaxValue">最大値(最小は0固定)</param>
    /// <param name="_MinRange">倍率変換時の最小値(未設定0.0f)</param>
    /// <param name="_MaxRange">倍率変換時の最大値(未設定1.0f)</param>
    float GetFadeInMag(const float& _Value, const float& _MaxValue, const float& _MinRange = 0.0f, const float& _MaxRange = 1.0f);
    /// <summary> 値の上昇で倍率が減少(フェードアウト)する処理。 </summary>
    /// <param name="_Value">現在値</param>
    /// <param name="_MaxValue">最大値(最小は0固定)</param>
    /// <param name="_MinRange">倍率変換時の最小値(未設定0.0f)</param>
    /// <param name="_MaxRange">倍率変換時の最大値(未設定1.0f)</param>
    float GetFadeOutMag(const float& _Value, const float& _MaxValue, const float& _MinRange = 0.0f, const float& _MaxRange = 1.0f);
}