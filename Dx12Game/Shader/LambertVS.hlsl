struct VSInput
{
	float3 Position : POSITION;		// 位置座標
	float3 Normal	: NORMAL;		// 法線ベクトル
	float2 TexCoord	: TEXCOORD;		// テクスチャ座標
	float3 Tangent	: TANGENT;		// 接線ベクトル
};

struct VSOutput
{
	float4 Position : SV_POSITION;	// 位置座標
	float2 TexCoord	: TEXCOORD;		// テクスチャ座標
	float3 Normal	: NORMAL;		// 法線ベクトル
	float4 WorldPos	: WORLD_POS;	// 接線ベクトル
};

cbuffer Transform : register(b0)
{
	float4x4 World	: packoffset(c0);	// ワールド行列
	float4x4 View	: packoffset(c4);	// ビュー行列
	float4x4 Proj	: packoffset(c8);	// 射影行列
}

VSOutput Lambert(VSInput _Input)
{
	VSOutput output = (VSOutput)0;

	float4 localPos = float4(_Input.Position, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Proj, viewPos);

	output.Position = projPos;
	output.TexCoord = _Input.TexCoord;
	output.WorldPos = worldPos;
	output.Normal = normalize(mul((float3x3)World, _Input.Normal));

	return output;
}