#include "Basic.hlsli"

struct VSInput
{
    float3 pos      : POSITION; //�ʒu���W
    float3 normal   : NORMAL;   //�@��
    float2 uv       : TEXCOORD; //�e�N�X�`�����W
    float3 tangent  : TANGENT;  //�ڐ��x�N�g��
};

// �萔�o�b�t�@
cbuffer Transform : register(b0)
{
    float4x4 World  : packoffset(c0); // ���[���h�s��ł�.
    float4x4 View   : packoffset(c4); // �r���[�s��ł�.
    float4x4 Proj   : packoffset(c8); // �ˉe�s��ł�
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