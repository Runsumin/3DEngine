//
//
//
// Depth
// [2020/11/17 RUNSUMIN]

// 상수 버퍼
cbuffer cbDEFAULT : register(b0)
{
	matrix mTM;
	matrix mView;
	matrix mProj;
	matrix mWV;
	matrix mWVP;
};

//VS 출력 구조체.
struct VSOutput
{
	float4 Depth : TEXCOORD0;
};

VSOutput VS_Main(
	float4 pos : POSITION,
	float3 nor : NORMAL,
	float3 binor : BINORMAL,
	float3 Tangent : TANGENT,
	float2 uv : TEXCOORD)
{
	pos.w = 1;

	pos = mul(pos, mTM);

	pos = mul(pos, mView);

	pos = mul(pos, mProj);

	VSOutput o = (VSOutput)0;
	o.Depth = pos;

	return o;
}


float4 PS_Main(
	float4 Depth  : TEXCOORD0
) : SV_TARGET
{
	float depthVal;
	float4 col;

	depthVal = Depth.z / Depth.w;

   return float4(depthVal, depthVal, depthVal,1.0f);
}