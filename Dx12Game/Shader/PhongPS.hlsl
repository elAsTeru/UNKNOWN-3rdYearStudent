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
    float3 CameraPosition	: packoffset(c2);	// カメラ位置	(Phongで追加)
}

cbuffer MaterialBuffer : register(b2)
{
	float3 Diffuse	: packoffset(c0);		// 拡散反射率
	float Alpha		: packoffset(c0.w);		// 透過度
    float3 Specular : packoffset(c1);		// 鏡面反射			(Phongで追加)
    float Shininess : packoffset(c1.w);		// 鏡面反射強度		(Phongで追加)
}

SamplerState WrapSmp	: register(s0);
Texture2D ColorMap		: register(t0);

PSOutput Phong(VSOutput _Input)
{
	PSOutput output = (PSOutput)0;

	float3 N = normalize(_Input.Normal);
	float3 L = normalize(LightPosition - _Input.WorldPos.xyz);
    float3 V = normalize(CameraPosition - _Input.WorldPos.xyz);							//(Phongで追加)

    float3 R = normalize(-reflect(V, N));											//(Phongで追加)
	
	float4 color = ColorMap.Sample(WrapSmp, _Input.TexCoord);
    float3 diffuse = LightColor * Diffuse * saturate(dot(L, N)) + 0.3f; // 全体的に明るくするために+0.3;
    float3 specular = LightColor * Specular * pow(saturate(dot(L, R)), Shininess);	//(Phongで追加)

	//output.Color = float4(color.rgb * diffuse, color.a + Alpha);					//(Lambert用)
    output.Color = float4(color.rgb * (diffuse + specular), color.a * Alpha);		//(Phongで調整後)
	
	return output;
}