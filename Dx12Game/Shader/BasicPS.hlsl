#include "Basic.hlsli"

// ディレクショナルライト
cbuffer DirLightBuffer : register(b1)
{
    float3 LightDir     : packoffset(c0); // ライト向き
    float3 LightColor   : packoffset(c1); // ライトカラー
}

// 通常
SamplerState ColorSmp : register(s0);
Texture2D    ColorMap : register(t0);
// ブルーム適応位置を示すためのテクスチャ(白黒にして明度のみが取られる)
Texture2D    BlurMap  : register(t1);

struct PSOutput
{
    float4 col      : SV_TARGET0;
    float4 normal   : SV_TARGET1;
    float4 highLum  : SV_TARGET2; // 高輝度 High Luminance
};

// 拡散反射処理
float Lambert(const float4 _Normal, const float3 _LightDir);

PSOutput Basic(VSOutput input)
{
    PSOutput output = (PSOutput)0;
    const float4 BaseColor = ColorMap.Sample(ColorSmp, input.uv);


    //// RenderTarget0...通常→拡散反射
    output.col = BaseColor;
    if(output.col.a > 0.0f)
    {
        output.col.a -= 1 - input.alpha;    // α値を適応
    }
    else
    {
        output.col.a = 0;
    }
    // 拡散反射計算(ディレクショナルライト)
    output.col = float4(BaseColor.rgb * Lambert(input.normal, LightDir), BaseColor.a) * 2;


    //// RenderTarget1...法線
    output.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
    output.normal.a = 1;


    //// RenderTarget2...ブルームを適応する位置を示す
    // テクスチャをモノクロ化
    float4 blurCol = BlurMap.Sample(ColorSmp, input.uv);
    float sum = blurCol.r + blurCol.g + blurCol.b;
    if (sum >= 0.1f)
    {
        float4 grayScale = dot(BlurMap.Sample(ColorSmp, input.uv).rgb, float3(0.299, 0.587, 0.114));
        //float4 bloom = grayScale * BaseColor; // テクスチャ情報から光り具合を設定する場合
        float4 bloom = 0.15f * BaseColor;       // ブルームの光り具合を値で設定する場合
        output.highLum = bloom;
        output.highLum.a = 1.0f;
    }
    else
    {
        output.highLum = 0;
    }

    return output;
}

float Lambert(const float4 _Normal, const float3 _LightDir)
{
    float ret = 0.0f;
    
    ret = dot(_Normal.rgb, _LightDir);
    // 内積の結果に-1を乗算
    ret *= -1;
    if (ret < 0.0f)
    {
        ret = 0.0f;
    }
    // 拡散反射光を正規化
    ret /= 3.1415926f;
    
    return ret;
}