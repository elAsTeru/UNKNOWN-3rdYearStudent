
cbuffer PostEffect : register(b0)
{
    float4 bkWeights[2];
}
Texture2D<float4> tex               : register(t0); // 通常テクスチャ
Texture2D<float4> texNormal         : register(t1); // 法線
Texture2D<float4> texHighLum        : register(t2); // 高輝度
Texture2D<float4> texShrinkHighLum  : register(t3); // 縮小バッファ高輝度

SamplerState smp        : register(s0);   // サンプラー

struct Output
{
    float4 svpos    : SV_POSITION;
    float2 uv       : TEXCOORD;
};  

struct BlurOutput
{
    float4 highLum  : SV_TARGET0; //高輝度(High Luminance)
    float4 col      : SV_TARGET1; //通常のレンダリング結果
};