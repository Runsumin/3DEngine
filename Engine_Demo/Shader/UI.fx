//
//
//
// UI_shader
// 
// [2020/09/06 RUNSUMIN]

#include "Blur.fx"
#include "Bloom.fx"
// 상수 버퍼
cbuffer cbDEFAULT : register(b0)
{
	matrix mTM;
	matrix mView;
	matrix mProj;
	matrix mWV;
};

cbuffer cbOverlaycol : register(b1)
{
	float4 OverlayCol;
	float SampleCount;
};

//VS 출력 구조체.
struct VSOutput
{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

VSOutput VS_Main(
	float4 pos : POSITION,
	float2 uv : TEXCOORD0
)
{
	pos.w = 1;

	pos = mul(pos, mTM);

	pos = mul(pos, mView);

	pos = mul(pos, mProj);

	VSOutput o = (VSOutput)0;
	o.pos = pos;
	o.uv = uv;

	return o;
}

Texture2D texDiffuse: register(t0);
Texture2D texBloom: register(t1);

SamplerState smpLinear : register(s0);
SamplerState smpBorder : register(s1);

float4 DownSampling(float2 uv, Texture2D texDiffuse)
{
	float2 pixelsize = 1.0 / float2(SampleCount, SampleCount);

	float4 cout;
	float2 SampletexCoord;
		
	SampletexCoord.x = uv.x - pixelsize.x;
	SampletexCoord.y = uv.y + pixelsize.y;

	cout = texDiffuse.Sample(smpLinear, SampletexCoord);

	SampletexCoord.x = uv.x + pixelsize.x;
	SampletexCoord.y = uv.y + pixelsize.y;

	cout += texDiffuse.Sample(smpLinear, SampletexCoord);

	SampletexCoord.x = uv.x + pixelsize.x;
	SampletexCoord.y = uv.y - pixelsize.y;

	cout += texDiffuse.Sample(smpLinear, SampletexCoord);

	SampletexCoord.x = uv.x - pixelsize.x;
	SampletexCoord.y = uv.y - pixelsize.y;

	cout += texDiffuse.Sample(smpLinear, SampletexCoord);

	return cout * 0.25f;
}

float4 PS_Main(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	//텍스쳐 셈플링.
   float4 tex = texDiffuse.Sample(smpLinear, uv);

   float4 col = tex * OverlayCol;

   return col; 
}

float4 PS_DEPTH(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	//텍스쳐 셈플링.
	float4 tex = texDiffuse.Sample(smpLinear, uv);

	return tex;
}

float4 PS_BLOOM(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	float4 Tex = texDiffuse.Sample(smpLinear, uv);

	float intensity = dot(Tex.xyz, float3(0.3f, 0.3f, 0.3f));

	float bloom_intensity = GetBloomCurve(intensity, 1.0f);
	float4 bloom_color = Tex * bloom_intensity / intensity;

	return float4(bloom_color.xyz, 1.0f);
}

float4 BLUR_ACCUMLATE(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	//float4 final = texDiffuse.Sample(smpLinear, uv);

	float4 bloomx_tmp = Blur_X(uv, smpLinear, texDiffuse);

	float4 bloomx = Blur_X(uv, smpLinear, texBloom);

	return float4(bloomx_tmp.xyz + bloomx.xyz, 1.0f);
}


float4 PS_FINAL(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	float4 final = texDiffuse.Sample(smpLinear, uv);
	float4 bloom = texBloom.Sample(smpLinear, uv);

	float4 Result = float4(final.xyz + bloom.xyz, final.w);
	return final;
}

float4 PS_BLUR(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	float4 bloomx = Blur_X(uv, smpLinear, texDiffuse);

	return bloomx;
}
