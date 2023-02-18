struct VSOutput
{
	float4 Position	: SV_POSITION;	// �ʒu���W
	float2 TexCoord	: TEXCOORD;		// �e�N�X�`�����W
	float3 Normal	: NORMAL;		// �@���x�N�g��
	float4 WorldPos	: WORLD_POS;	// ���[���h��Ԃł̈ʒu���W
};

struct PSOutput
{
	float4 Color	: SV_TARGET0;	// �o�̓J���[
};

cbuffer LightBuffer : register(b1)
{
	float3 LightPosition	: packoffset(c0);	// ���C�g�ʒu
	float3 LightColor		: packoffset(c1);	// ���C�g�J���[
}

cbuffer MaterialBuffer : register(b2)
{
	float3 Diffuse	: packoffset(c0);		// �g�U���˗�
	float Alpha		: packoffset(c0.w);		// ���ߓx
}

SamplerState WrapSmp	: register(s0);
Texture2D ColorMap		: register(t0);

PSOutput Lambert(VSOutput _Input)
{
	PSOutput output = (PSOutput)0;

	float3 N = normalize(_Input.Normal);
	float3 L = normalize(LightPosition - _Input.WorldPos);

	float4 color = ColorMap.Sample(WrapSmp, _Input.TexCoord);
	float3 diffuse = LightColor * Diffuse * saturate(dot(L, N));

	output.Color = float4(color.rgb * diffuse, color.a + Alpha);					//(Lambert�p)

	return output;
}