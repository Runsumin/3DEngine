//
//
//
// Axis Shader Generate
// [2020/08/07 RUNSUMIN]

// Vertex Shader

cbuffer ConstBuffer : register(b0) // �������� 0���� ��� ��� 
{
    matrix mWorld;
    matrix mView;
    matrix mProj;
    matrix mWVP;
}

// vs ��� ����ü
struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};
	

VSOutput VS_Main( 
				  float4 pos : POSITION,
				  float4 col : COLOR0
				)
{

	VSOutput o = (VSOutput)0;	 
    pos.w = 1.0f;
    //��ȯ.
    pos = mul(pos, mWVP);
    	
    //���.
	o.pos = pos;						
	o.col = col;

    return o;
}

// Pixel Shader

float4 PS_Main( 
				float4 pos : SV_POSITION,
				float4 col : COLOR0
				) : SV_TARGET
{
	return col;
}


/////////////////////////////////////////////////////////////