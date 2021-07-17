///
///
///
/// 노멀맵핑 적용 모델 셰이더
/// [2020/11/04 RUNSUMIN]

#include "Light.fx"
#include "Fog.fx"
#include "Blur.fx"
#include "Shadow.fx"
#include "Bloom.fx"

cbuffer ConstBuffer : register(b0)
{
	matrix mTM;
	matrix mView;
	matrix mProj;
	matrix mWVP;
	matrix mLightVP;
	matrix mTextm;
};

struct STATE
{
	bool shadowOnOff;
};

// VS 출력 구조체.
struct VSOutput
{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
	float2 uv  : TEXCOORD;
	float4 uvsd : TEXCOORD101;
	float4 posSM : TEXCOORD102;
	float4 posP : TEXCPPRD103;
	float4 posV : TEXCPPRD104;
};

// 재질 정보 상수버퍼
cbuffer cbSTATE : register(b1)
{
	STATE g_State;
};
// Vertex Shader

VSOutput VS_Main(
	float3 pos : POSITION,
	float3 nor : NORMAL,
	float2 uv : TEXCOORD)
{
	VSOutput o = (VSOutput)0;

	float4 posvec = float4(pos, 1.0f);

	posvec = mul(posvec, mTM);

	float4 posv;
	posv = mul(posvec, mView);
	o.posV = posv;
	/// 광원 기준 변환
	o.posSM = mul(posvec, mLightVP);

	posvec = mul(posvec, mView);
	posvec = mul(posvec, mProj);

	float4 uv1 = mul(posvec, mTextm);


	o.pos = posvec;
	o.uv = uv;
	o.nor = nor;
	o.uvsd = uv1;
	o.posP = posvec;

	return o;
}

// Pixel Shader
Texture2D texDiffuse : register(t0);
Texture2D texshadow  : register(t1);

SamplerState smpLinear : register(s0);
SamplerState smpBorder : register(s1);

float4 PS_Main(
	float4 pos : SV_POSITION,
	float3 nor : NORMAL,
	float2 uv : TEXCOORD,
	float4 uvsd : TEXCOORD101,
	float4 posSM : TEXCOORD102,
	float4 posP : TEXCPPRD103,
	float4 posV : TEXCPPRD104
) : SV_TARGET
{
	float4 color = 1;
	float4 tex = 1;
	float factor = 0.0f;
	tex = texDiffuse.Sample(smpLinear, uv);
	tex = float4(pow(tex.xyz, 2.2f), tex.w);
	tex = float4(tex.xyz * g_Lit.Diffuse.xyz * 0.5f, tex.w);

	//// LightProj를 거친 PosL을 ShadowMap의 UV로 변환
	float fog = 1;
	if (g_Fog.bOn == true)
		fog = Fog(posV);

	if (g_State.shadowOnOff == true)
	{
		factor = CalShadowMount(posSM, texshadow);
	}

	color = float4(pow(tex.xyz * factor, 1 / 2.2f), 1);

	//float4 Result = 1.0f;
	if (g_Fog.bOn == true)
	{
		return float4(color.xyz * fog + g_Fog.Color.xyz * (1 - fog), color.w);
		//return float4(Tone * fog + g_Fog.Color.xyz * (1 - fog), color.w);
		//return float4(EmissiveMap.xyz, color.w);
	}
	else
	{
		return float4(color);
		//return float4(Tone , color.w);
		//return float4(EmissiveMap.xyz, color.w);
	}

	//Result = LuminanceConversion(Result);

	//return Result;
}


float PS_DEPTH(
	float4 pos : SV_POSITION,
	float3 nor : NORMAL,
	float2 uv : TEXCOORD,
	float4 uvsd : TEXCOORD101,
	float4 posSM : TEXCOORD102,
	float4 posV : TEXCPPRD104

) : SV_TARGET
{
	float depthVal;

	depthVal = pos.z;

	return depthVal;

}

///////////////////////////////////////////////////////////saas