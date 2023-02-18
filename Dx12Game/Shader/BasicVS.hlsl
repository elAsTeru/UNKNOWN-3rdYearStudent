#include "Basic.hlsli"

struct VSInput
{
    float3 pos      : POSITION; //位置座標
    float3 normal   : NORMAL;   //法線
    float2 uv       : TEXCOORD; //テクスチャ座標
    float3 tangent  : TANGENT;  //接線ベクトル
};

// 定数バッファ
cbuffer Transform : register(b0)
{
    float4x4 World  : packoffset(c0); // ワールド行列です.
    float4x4 View   : packoffset(c4); // ビュー行列です.
    float4x4 Proj   : packoffset(c8); // 射影行列です
    float1 Alpha    : packoffset(c12);
}

VSOutput Basic(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float4 localPos = float4(input.pos, 1.0f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    output.pos = projPos;
    output.uv = input.uv;
    output.alpha = Alpha;
    output.normal = float4(input.normal, 1);

    return output;
}