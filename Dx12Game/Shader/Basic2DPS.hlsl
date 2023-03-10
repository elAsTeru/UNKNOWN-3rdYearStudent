#include "Basic.hlsli"

struct PSOutput
{
    float4 col : SV_TARGET0;
};

// 通常
SamplerState ColorSmp : register(s0);
Texture2D ColorMap : register(t0);

PSOutput Basic2D(VSOutput input)
{
    PSOutput output = (PSOutput) 0;

    // 色
    output.col = ColorMap.Sample(ColorSmp, input.uv);
    if (output.col.a > 0.0f)
    {
        output.col.a -= 1 - input.alpha; // α値を適応
    }
    else
    {
        output.col.a = 0;
    }
    
    return output;
}