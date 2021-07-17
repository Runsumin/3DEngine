///
///
///
/// 디버그용 박스 출력 셰이더
/// [2021/02/03 RUNSUMIN]

cbuffer ConstBuffer : register(b0)
{
	matrix mTM;
	matrix mView;
	matrix mProj;
};

// VS 출력 구조체.
struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

// Vertex Shader

VSOutput VS_Main(
	float4 pos : POSITION,
	float4 col : COLOR)
{
	VSOutput o = (VSOutput)0;

	float4 posl = float4(pos.x, pos.y, pos.z, 1.0f);

	posl = mul(posl, mTM);
	posl = mul(posl, mView);
	posl = mul(posl, mProj);

	
	o.pos = posl;
	o.col = col;


	return o;
}

// Pixel Shader

float4 PS_Main(
	float4 pos : POSITION,
	float4 col : COLOR
	) : SV_TARGET
{
	return col;
}

/////////////////////////////////////////////////////////////