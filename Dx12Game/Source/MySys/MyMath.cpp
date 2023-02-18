#include "MyMath.h"

namespace MyMath
{
    const float Random(const int& _Range)
    {
        const int MaxAdjusted = (MaxInt16 + 1) - (MaxInt16 + 1) % _Range;	// 調整済み最大値
        int rand;
        do {
            rand = std::rand();
        } while (rand >= MaxAdjusted);

        return ((double)rand / MaxAdjusted) * _Range;
    }

    bool IsNearAngle(const Vector3& _V1, const Vector3& _V2)
    {
        float dot = _V1.Dot(_V2);
        if (dot > 0.99f)
        {
            return true;
        }
        return false;
    }

    Vector3 YRotateToAimVec(const Vector3& _Now, const Vector3& _Aim, float _DegSpeed)
    {
        // 角速度をラジアンに変換、左右回転を調べる
        float radSpeed = ToRadians(_DegSpeed);
        radSpeed *= CalcRotateYDirection(_Now, _Aim);

        // Y軸回転行列を作成
        Matrix yRotMat = Matrix::CreateRotationY(radSpeed);
        // Y軸回転を行う
        Vector3 rotVec;
        rotVec.Transform(_Now, yRotMat);

        return rotVec;
    }

    float CalcRotateYDirection(const Vector3& _V1, const Vector3& _V2)
    {
        Vector3 cross;
        _V1.Cross(_V2, cross);
        if (cross.y < 0.0f)
        {
            return -1.0f;
        }
        return 1.0f;
    }

    float CircularMotion(float _MinRange, float _MaxRange, float _Time, float _MotionSpeed)
    {
        float rangeWidth = _MaxRange - _MinRange;	// 範囲幅を求める
        float result = (0.5f * (std::cos(_Time * _MotionSpeed) + 1)) * rangeWidth + _MinRange;
        return result;
    }

    uint32_t CreateRandom(uint32_t _Min, uint32_t _Max)
    {
        // +1をすることで_Min ～ _Max -1の値が出るのを補正する
        uint32_t adjustedMax = (MAXINT16 + 1) - (MAXINT16 + 1) % (_Max - _Min + 1);
        int rand;
        do
        {
            rand = std::rand();
        } while (rand >= adjustedMax);

        return (uint32_t)(((double)rand / adjustedMax) * (_Max - _Min + 1));
    }

    bool IsNearZero(const float& _F)
    {
        // 1e-9 == 0.000000001
        if (-1e-9 < _F && _F < 1e-9)
        {
            return true;
        }
        return false;
    }

    bool IsNearZero(const Vector2& _Vec2)
    {
        if (IsNearZero(_Vec2.x) && IsNearZero(_Vec2.y))
        {
            return true;
        }
        return false;
    }

    bool IsNearZero(const Vector3& _Vec3)
    {
        if (IsNearZero(_Vec3.x) && IsNearZero(_Vec3.y) && IsNearZero(_Vec3.z))
        {
            return true;
        }
        return false;
    }
}