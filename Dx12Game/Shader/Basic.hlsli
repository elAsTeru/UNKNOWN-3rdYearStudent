
struct VSOutput
{
    float4 pos      : SV_POSITION;  // 位置座標です.
    float4 normal   : NORMAL;
    float2 uv       : TEXCOORD;     // テクスチャ座標です.
    float1 alpha    : COLOR;
};