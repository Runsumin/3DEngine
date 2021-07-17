///
///
///
/// 조명 기본 셰이더
/// [2020/11/03 RUNSUMIN]

// 조명 정보
struct LIGHT
{
	float4 Direction;
	float4 Diffuse;
	float4 Ambient;
};

struct POINTLIGHT
{
	float4 Diffuse;
	float4 Ambient;
	float3 Position;
	float Range;
};
//조명 정보용 상수버퍼
cbuffer cbLIGHT : register(b2)
{
	LIGHT g_Lit;
};

cbuffer cbLIGHT : register(b3)
{
	POINTLIGHT g_PointLit;
};

float4 Light(float3 nrm, matrix wv, matrix view)
{
	float4 N = float4(nrm, 0);    N.w = 0;
	float4 L = -g_Lit.Direction;

	N = mul(N, wv);
	L = mul(L, view);

	N = normalize(N);
	L = normalize(L);


	//조명 계산 
	float4 diff = max(dot(N, L), 0) * g_Lit.Diffuse;
	float4 amb = g_Lit.Ambient;

	return diff + amb;
}

float4 PointLight(float4 pos, float4 nrm)
{
	float4 col = 0;
	float4 vlit = float4(g_PointLit.Position, 0) - pos;
	float len = length(vlit);

	float4 N = normalize(nrm);
	vlit = normalize(vlit);

	float4 diff = max(dot(N, vlit), 0) * g_PointLit.Diffuse;
	float4 amb = g_PointLit.Ambient;

	col = diff + amb;

	// 감쇠율
	float atten = (1 / (len * len)) + 5 / len;

	col = col * atten;

	return col;
}