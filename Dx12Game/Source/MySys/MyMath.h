#pragma once
#include <DirectXTK12/SimpleMath.h>
//#include <cmath>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace MyMath
{
    constexpr float Pi = 3.1415926535f;
    constexpr int16_t MaxInt16 = 32767;

    inline constexpr float ToRadians(const float& _Degree)
    {
        return _Degree * Pi / 180.0f;
    }

    inline constexpr float ToDegrees(const float& _Radian)
    {
        return _Radian * 180.0f / Pi;
    }

    /// <summary>
    /// 偏りの小さい乱数を作成
    /// </summary>
    const float Random(const int& _Range);

    /// <summary>
    /// ラジアン角をベクトル化し、返却
    /// </summary>
    inline Vector2 AngleToVecs2LH(const float& _Radian)
    {
        Vector2 unit;
        unit.x = std::cosf(_Radian);
        unit.y = -std::sinf(_Radian);
        return unit;
    }

    /// <summary>
    /// ベクトルをラジアン角化し、返却
    /// </summary>
    inline float Vec2ToAnglesLH(const Vector2& _V)
    {
        // 左手座標系なので反転して返却
        return -std::atan2f(_V.y, _V.x);
    }

    /// <summary>
    /// 2つのベクトルのなす角
    /// </summary>
    inline float AngleBetweenTwoVecs2(const Vector2& _V1, const Vector2& _V2)
    {
        return cos(_V1.Dot(_V2) / (_V1.Length() + _V2.Length()));
    }

    /// <summary>
    /// Vec1とVec2はほぼ同じか？
    /// </summary>
    bool IsNearAngle(const Vector3& _V1, const Vector3& _V2);

    /// <summary>
    /// 現在の向きからAimVecに向かってDegVerocityの速度でY回転する
    /// </summary>
    Vector3 YRotateToAimVec(const Vector3& _Now, const Vector3& _Aim, float DegSpeed);

    /// <summary>
    /// Vec1→Vec2の最短の回転方向を調べる(Y軸回転を想定)。時計回り:+1.0f / 半時計回り:-1.0fを返却
    /// </summary>
    float CalcRotateYDirection(const Vector3& _V1, const Vector3& _V2);

    /// <summary>
    /// 円運動、範囲 0.0～1.0、経過時間、回転速度
    /// </summary>
    float CircularMotion(float _MinRange, float _MaxRange, float _Time, float _MotionSpeed);

    uint32_t CreateRandom(uint32_t _Min, uint32_t _Max);

    bool IsNearZero(const float& _F);
    bool IsNearZero(const Vector2& _Vec2);
    bool IsNearZero(const Vector3& _Vec3);
}