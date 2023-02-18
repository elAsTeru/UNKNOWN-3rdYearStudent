struct VSOutput
{
	float4 Position	: SV_POSITION;	// 位置座標
	float2 TexCoord	: TEXCOORD;		// テクスチャ座標
	float3 Normal	: NORMAL;		// 法線ベクトル
	float4 WorldPos	: WORLD_POS;	// ワールド空間での位置座標
};

struct PSOutput
{
	float4 Color	: SV_TARGET0;	// 出力カラー
};

cbuffer LightBuffer : register(b1)
{
	float3 LightPosition	: packoffset(c0);	// ライト位置
	float3 LightColor		: packoffset(c1);	// ライトカラー
}

cbuffer MaterialBuffer : register(b2)
{
	float3 Diffuse	: packoffset(c0);		// 拡散反射率
	float Alpha		: packoffset(c0.w);		// 透過度
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

	output.Color = float4(color.rgb * diffuse, color.a + Alpha);					//(Lambert用)

	return output;
}