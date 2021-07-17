///
///
///
/// Fog - 안개 효과
/// [2021/06/25 RUNSUMIN]

// 조명 정보
struct FOG
{
	float4 Color;
	float  Density;
	bool   bOn;
};

cbuffer cbFog : register(b4)
{
	FOG g_Fog;
}

float Fog(float4 vpos)
{
	// fog linear
	//float f = (g_Fog.End - vpos.z) / (g_Fog.End - g_Fog.Start);

	// fog exp
	float f = 1 / exp(pow(length(vpos) * g_Fog.Density, 2));

	f = saturate(f);

	return f;
}