///
///
///
/// ��ָ��� ���� �� ���̴�
/// [2020/11/04 RUNSUMIN]

#include "Light.fx"

cbuffer ConstBuffer : register(b0)
{
	matrix mTM;
	matrix mView;
	matrix mProj;
	matrix mWV;
	matrix mWVP;
};

// ���� ����
struct MATERIAL
{
	float4 Diffuse;
	float4 Ambient;
	float4 Specular;
};

// VS ��� ����ü.
struct VSOutput
{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
	float2 uv  : TEXCOORD;
};

// ���� ���� �������
cbuffer cbMATERIAL : register(b1)
{
	MATERIAL g_Mtrl;
};

// Vertex Shader

VSOutput VS_Main(
	float4 pos : POSITION,
	float3 nor : NORMAL,
	float3 binor : BINORMAL,
	float3 Tangent : TANGENT,
	float2 uv : TEXCOORD)
{
	pos.w = 1.0f;

	pos = mul(pos, mTM);
	pos = mul(pos, mView);
	pos = mul(pos, mProj);

	VSOutput o = (VSOutput)0;
	o.pos = pos;			
	o.uv = uv;	
	o.nor = nor;

	return o;
}

// Pixel Shader
Texture2D texDiffuse;

SamplerState smpLinear;

float4 PS_Main(
	float4 pos : SV_POSITION,
	float3 nor : NORMAL,
	float2 uv : TEXCOORD
	) : SV_TARGET
{
	float4 tex = 1;
	tex = texDiffuse.Sample(smpLinear, uv);

	float4 dlight = 1;
	dlight = Light(nor, g_Mtrl.Diffuse, g_Mtrl.Ambient, mWV, mView);
	return tex * dlight;
}

/////////////////////////////////////////////////////////////