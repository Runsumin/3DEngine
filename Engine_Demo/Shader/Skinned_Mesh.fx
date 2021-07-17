///
///
///
/// FBX 모델 기본 셰이더
/// [2020/10/15 RUNSUMIN]

#include "Light.fx"
#include "PBRCommon.fx"
#include "Tone_Mapping.fx"
#include "Fog.fx"
#include "Blur.fx"
#include "Bloom.fx"

cbuffer ConstBuffer : register(b0)
{
	matrix mTM;
	matrix mWorld;
	matrix mView;
	matrix mProj;
	matrix mWV;
	matrix mWVP;
	matrix mBoneTM[100];
	matrix Axischange;
	matrix LightViewProj;
	float4 CamWorldPos;
};

// 재질 정보
struct MATERIAL
{
	float4 Diffuse;
	float4 Ambient;
	float4 Specular;
	bool Texon;
	bool NormalOn;
	bool SKinOn;
	bool MaskOn;
	bool BumpMappingOn;
	bool PBROn;
	bool EmissiveOn;
};

// VS 출력 구조체.
struct VSOutput
{
	float4 pos		  : SV_POSITION;
	float4 nor		  : TEXCOORD101;
	float4 binor	  : TEXCOORD102;
	float4 tangentnor : TEXCOORD103;
	float4 posL		  : TEXCOORD104;
	float4 posP		  : TEXCOORD105;
	float4 posv		  : TEXCOORD106;
	float3 Viewvec	  : TEXCOORD107;
	float3 WorldPos	  : TEXCOORD108;
	float2 uv		  : TEXCOORD;
};

// 재질 정보 상수버퍼
cbuffer cbMATERIAL : register(b1)
{
	MATERIAL g_Mtrl;
};

float4x4 invTangentMatrix(float3 tangent, float3 binormal, float3 normal);
float4 LightNM(float3 nrm, float4 lighttangentdirection);
float4 Skinning(float4 pos, float4 weight, uint4 index);

// Vertex Shader

VSOutput VS_Main(
	float4 pos : POSITION,
	float4 weight : BLENDWEIGHT,
	uint4 index : BLENDINDICES,
	float3 nor : NORMAL,
	float3 binor : BINORMAL,
	float3 Tangent : TANGENT,
	float2 uv : TEXCOORD)
{
	VSOutput o = (VSOutput)0;
	pos.w = 1.0f;

	o.WorldPos = mul(pos, mTM);

	float4 SkinNor = 1.0f;
	float4 SkinBINor = 1.0f;
	float4 SkinTan = 1.0f;

	if (g_Mtrl.SKinOn == true)
	{
		pos = Skinning(pos, weight, index);
		SkinNor = Skinning(float4(nor.x, nor.y, nor.z, 0.0f), weight, index);
		SkinBINor = Skinning(float4(binor.x, binor.y, binor.z, 0.0f), weight, index);
		SkinTan = Skinning(float4(Tangent.x, Tangent.y, Tangent.z, 0.0f), weight, index);
	}

	// 광원 기준 버텍스 위치 계산...
	float4 posL;
	posL = mul(pos, mTM);
	posL = mul(posL, LightViewProj);

	float4 posv;
	posv = mul(pos, mWV);
	o.posv = posv;

	float3 viewvector = normalize(CamWorldPos.xyz - pos.xyz);

	pos = mul(pos, mTM); //world로 변환
	pos = mul(pos, mWVP);

	/////
	float4 Normal = mul(SkinNor, mTM);
	float4 Binormal = mul(SkinBINor, mTM);
	float4 tant = mul(SkinTan, mTM);

	normalize(Normal);
	normalize(Binormal);
	normalize(tant);

	o.posP = pos;

	o.pos = pos;
	o.nor = Normal;
	o.binor = Binormal;
	o.tangentnor = tant;
	o.posL = posL;
	o.uv = uv;
	o.Viewvec = viewvector;

	return o;
}
float4 Skinning(float4 pos, float4 weight, uint4 index)
{
	float4 skinVtx;

	float4 _v0 = mul(pos, mBoneTM[index.x]);
	float4 _v1 = mul(pos, mBoneTM[index.y]);
	float4 _v2 = mul(pos, mBoneTM[index.z]);
	float4 _v3 = mul(pos, mBoneTM[index.w]);

	skinVtx = (_v0 * weight.x) + (_v1 * weight.y) + (_v2 * weight.z) + (_v3 * weight.w);

	return skinVtx;
}

float4x4 invTangentMatrix(float3 tangent, float3 binormal, float3 normal)
{
	float4x4 mat = {
		float4(tangent,0.0f),
		float4(binormal,0.0f),
		float4(normal,0.0f),
		{0,0,0,1},
	};

	return transpose(mat);
}

float4 LightNM(float3 nrm)
{
	float4 N = float4(nrm, 0);    N.w = 0;
	float4 L = -g_Lit.Direction;

	N = mul(N, mWV);
	L = mul(L, mView);

	N = normalize(N);
	L = normalize(L);


	//조명 계산 
	float4 diff = max(dot(N, L), 0) * g_Lit.Diffuse * g_Mtrl.Diffuse;
	float4 amb = g_Lit.Ambient * g_Mtrl.Ambient;

	return diff + amb;
}

//float4 CmpShadowMap(float4)
// Pixel Shader

Texture2D texDiffuse : register(t0);
Texture2D texNormal  : register(t1);
Texture2D texMaskMap  : register(t2);
Texture2D texORMMAP  : register(t3);
Texture2D texShadowMap  : register(t4);
TextureCube texSkyBox : register(t5);     //레지스터 직접 지정. 
TextureCube texSkyBox_irr : register(t6);     //레지스터 직접 지정. 

SamplerState smpLinear : register(s0);
SamplerState smpBorder : register(s1);
SamplerState smpClamp  : register(s2);

float4 PS_Main(
	float4 pos : SV_POSITION,
	float4 nor : TEXCOORD101,
	float4 binor : TEXCOORD102,
	float4 tangentnor : TEXCOORD103,
	float4 posL : TEXCOORD104,
	float4 posP : TEXCOORD105,
	float4 posv : TEXCOORD106,
	float3 Viewvec : TEXCOORD107,
	float3 WorldPos : TEXCOORD108,
	float2 uv : TEXCOORD
) : SV_TARGET
{
	float4 color = 1;		// 최종 출력 색깔
	float4 Diffuse = 1;		// 텍스쳐
	float4 BumpMap = 1;		// 노멀맵
	float4 MaskMap = 1;		// 마스크맵
	float4 ORMMap = 1;		// AO, Roughness, Metallic
	float4 lit = 1;			// 라이트
	float lightIntensity;
	float bright = 1;

	// Light
	lit = Light(float3(nor.xyz), mWV, mView);

	if (g_Mtrl.Texon == true)
	{
		Diffuse = texDiffuse.Sample(smpLinear, uv);		// 디퓨즈맵 얻어오기
		Diffuse = float4(pow(Diffuse.xyz, 2.2f), Diffuse.w);

		if (g_Mtrl.MaskOn == true)
		{
			MaskMap = texMaskMap.Sample(smpLinear, uv);
		}
		if (g_Mtrl.NormalOn == true)
		{
			BumpMap = texNormal.Sample(smpLinear, uv);		// 노말맵 색 얻어오기
		}
		if (g_Mtrl.PBROn == true)
		{
			ORMMap = texORMMAP.Sample(smpLinear, uv);		// ORM 색 얻어오기
		}
	}

	float3 localNormal = TwoChannelNormalX2(BumpMap.xy);
	float3x3 TBN = float3x3(tangentnor.xyz, binor.xyz, nor.xyz);
	float3 N = normalize(mul(localNormal, TBN));

	float3 V = normalize(CamWorldPos.xyz - WorldPos);

	if(g_Mtrl.PBROn == true)
		color = LightSurface(V, N, 1, g_Lit.Diffuse, g_Lit.Direction, Diffuse * MaskMap.r,
			ORMMap.g, ORMMap.b, ORMMap.r, texSkyBox, smpBorder, texSkyBox_irr);
	else
	{
		BumpMap = (2.0f * BumpMap) - 1.0f;				// 벡터로 변환

		float4 BumpNormal = 1;

		BumpNormal = nor + BumpMap.x * tangentnor + BumpMap.y * binor;

		BumpNormal = normalize(BumpNormal);							// 정규화

		float4 lightdir = -g_Lit.Direction;

		bright = saturate(dot(BumpNormal, lightdir));		// 명도 산출
		bright = bright * 0.5f + 0.2f;

		if (g_Mtrl.NormalOn == true)
		{
			if (g_Mtrl.MaskOn == true)
			{
				color = float4(bright * Diffuse.xyz, 1.0f) * MaskMap.r;
			}
			else
			{
				color = float4(bright * Diffuse.xyz, 1.0f);
			}
		}
		else
		{
			color = Diffuse;
		}

	}


	float3 Tone = Flimic_ToneMapping(color.xyz);

	float2 Luv;
	Luv.x = posL.x / posL.w;
	Luv.y = posL.y / posL.w;

	Luv.x = Luv.x * 0.5f + 0.5f;
	Luv.y = Luv.y * -0.5f + 0.5f;

	// ShadowMap과 깊이값을 비교
	float depth = texShadowMap.Sample(smpBorder, Luv).r;
	float curdepth = posL.z / posL.w;

	if (curdepth > depth + 0.015f)
		Tone = Tone * 0.85f;

	float fog = 1;
	if (g_Fog.bOn == true)
		fog = Fog(posv);

	if (g_ChView.Diffuse == true)
	{
		Tone = Diffuse;
	}
	if (g_ChView.Normal == true)
	{
		Tone = BumpMap;
	}

	return float4(( (Tone * fog + g_Fog.Color.xyz * (1 - fog)) ), color.w);

	//return lit;

	/*float4 Result = 1.f;
	Result = float4(Tone * fog + g_Fog.Color.xyz * (1 - fog), color.w);

	Result = LuminanceConversion(Result);

	return Result;*/
}



float PS_DEPTH(
	float4 pos	      : SV_POSITION,
	float4 nor : TEXCOORD101,
	float4 binor : TEXCOORD102,
	float4 tangentnor : TEXCOORD103,
	float4 posL : TEXCOORD104,
	float4 posP : TEXCOORD105,
	float4 posv : TEXCOORD106,
	float3 Viewvec : TEXCOORD107,
	float2 uv : TEXCOORD
) : SV_TARGET
{
	float depthVal;

	depthVal = posL.z / posL.w;

	return depthVal;

}

float4 PS_SHADOW(
	float4 pos		  : SV_POSITION,
	float4 nor : TEXCOORD101,
	float4 binor : TEXCOORD102,
	float4 tangentnor : TEXCOORD103,
	float4 posL : TEXCOORD104,
	float4 posP : TEXCOORD105,
	float4 posv : TEXCOORD106,
	float3 Viewvec : TEXCOORD107,
	float2 uv : TEXCOORD
) : SV_TARGET
{
	float bias;
	float4 color;
	float2 projectTexcoord;
	float depthval;
	float lightdepthval;
	float lightIntensity;
	float4 texcolor;

	return color;
}
///////////////////aw//////////////////////////////////////////w