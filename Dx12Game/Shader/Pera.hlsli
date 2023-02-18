
cbuffer PostEffect : register(b0)
{
    float4 bkWeights[2];
}
Texture2D<float4> tex               : register(t0); // �ʏ�e�N�X�`��
Texture2D<float4> texNormal         : register(t1); // �@��
Texture2D<float4> texHighLum        : register(t2); // ���P�x
Texture2D<float4> texShrinkHighLum  : register(t3); // �k���o�b�t�@���P�x

SamplerState smp        : register(s0);   // �T���v���[

struct Output
{
    float4 svpos    : SV_POSITION;
    float2 uv       : TEXCOORD;
};  

struct BlurOutput
{
    float4 highLum  : SV_TARGET0; //���P�x(High Luminance)
    float4 col      : SV_TARGET1; //�ʏ�̃����_�����O����
};