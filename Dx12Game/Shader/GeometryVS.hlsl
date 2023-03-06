struct VSInput
{
    float3 Position : POSITION;         //�ʒu���W
    //float3 Normal   : NORMAL;         //�@��
    //float3 Tangent  : TANGENT;        //�ڐ��x�N�g��
};

struct VSOutput
{
    float4 Position : SV_POSITION;      // �ʒu���W
    float4 Color : COLOR;
};

cbuffer Line : register(b0)
{
    float4x4 World : packoffset(c0);    // ���[���h�s��
    float4x4 View : packoffset(c4);     // �r���[�s��
    float4x4 Proj : packoffset(c8);     // �ˉe�s��
    float4 Color : packoffset(c12);
}

VSOutput Geometory(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float4 localPos = float4(input.Position, 1.0f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    output.Position = projPos;
    output.Color = Color;

    return output;
}