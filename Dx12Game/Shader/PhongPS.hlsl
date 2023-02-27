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
    float3 CameraPosition	: packoffset(c2);	// �J�����ʒu	(Phong�Œǉ�)
}

cbuffer MaterialBuffer : register(b2)
{
	float3 Diffuse	: packoffset(c0);		// �g�U���˗�
	float Alpha		: packoffset(c0.w);		// ���ߓx
    float3 Specular : packoffset(c1);		// ���ʔ���			(Phong�Œǉ�)
    float Shininess : packoffset(c1.w);		// ���ʔ��ˋ��x		(Phong�Œǉ�)
}

SamplerState WrapSmp	: register(s0);
Texture2D ColorMap		: register(t0);

PSOutput Phong(VSOutput _Input)
{
	PSOutput output = (PSOutput)0;

	float3 N = normalize(_Input.Normal);
	float3 L = normalize(LightPosition - _Input.WorldPos.xyz);
    float3 V = normalize(CameraPosition - _Input.WorldPos.xyz);							//(Phong�Œǉ�)

    float3 R = normalize(-reflect(V, N));											//(Phong�Œǉ�)
	
	float4 color = ColorMap.Sample(WrapSmp, _Input.TexCoord);
    float3 diffuse = LightColor * Diffuse * saturate(dot(L, N)) + 0.3f; // �S�̓I�ɖ��邭���邽�߂�+0.3;
    float3 specular = LightColor * Specular * pow(saturate(dot(L, R)), Shininess);	//(Phong�Œǉ�)

	//output.Color = float4(color.rgb * diffuse, color.a + Alpha);					//(Lambert�p)
    output.Color = float4(color.rgb * (diffuse + specular), color.a * Alpha);		//(Phong�Œ�����)
	
	return output;
}