#include "Basic.hlsli"

// �f�B���N�V���i�����C�g
cbuffer DirLightBuffer : register(b1)
{
    float3 LightDir     : packoffset(c0); // ���C�g����
    float3 LightColor   : packoffset(c1); // ���C�g�J���[
}

// �ʏ�
SamplerState ColorSmp : register(s0);
Texture2D    ColorMap : register(t0);
// �u���[���K���ʒu���������߂̃e�N�X�`��(�����ɂ��Ė��x�݂̂������)
Texture2D    BlurMap  : register(t1);

struct PSOutput
{
    float4 col      : SV_TARGET0;
    float4 normal   : SV_TARGET1;
    float4 highLum  : SV_TARGET2; // ���P�x High Luminance
};

// �g�U���ˏ���
float Lambert(const float4 _Normal, const float3 _LightDir);

PSOutput Basic(VSOutput input)
{
    PSOutput output = (PSOutput)0;
    const float4 BaseColor = ColorMap.Sample(ColorSmp, input.uv);


    //// RenderTarget0...�ʏ큨�g�U����
    output.col = BaseColor;
    if(output.col.a > 0.0f)
    {
        output.col.a -= 1 - input.alpha;    // ���l��K��
    }
    else
    {
        output.col.a = 0;
    }
    // �g�U���ˌv�Z(�f�B���N�V���i�����C�g)
    output.col = float4(BaseColor.rgb * Lambert(input.normal, LightDir), BaseColor.a) * 2;


    //// RenderTarget1...�@��
    output.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
    output.normal.a = 1;


    //// RenderTarget2...�u���[����K������ʒu������
    // �e�N�X�`�������m�N����
    float4 blurCol = BlurMap.Sample(ColorSmp, input.uv);
    float sum = blurCol.r + blurCol.g + blurCol.b;
    if (sum >= 0.1f)
    {
        float4 grayScale = dot(BlurMap.Sample(ColorSmp, input.uv).rgb, float3(0.299, 0.587, 0.114));
        //float4 bloom = grayScale * BaseColor; // �e�N�X�`����񂩂������ݒ肷��ꍇ
        float4 bloom = 0.15f * BaseColor;       // �u���[���̌�����l�Őݒ肷��ꍇ
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
    // ���ς̌��ʂ�-1����Z
    ret *= -1;
    if (ret < 0.0f)
    {
        ret = 0.0f;
    }
    // �g�U���ˌ��𐳋K��
    ret /= 3.1415926f;
    
    return ret;
}