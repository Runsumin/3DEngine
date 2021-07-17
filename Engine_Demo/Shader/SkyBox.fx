//
//
//
//
//
//
/// [2021/05/21 RUNSUMIN]
#include "Fog.fx"
#include "Bloom.fx"

// �⺻ �������
cbuffer ConstBuffer : register(b0)
{
    matrix mViewInv;     //���� ���. 
    matrix mProjInv;   //���� ��ȯ ���. 
    matrix mView;   //���� ��ȯ ���. 
};

struct Material
{
    float4 Color;
};

cbuffer MATERIAL : register(b1)
{
    Material g_mMat;
};


//VS ��� ����ü.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float4 viewpos : TEXCOORD101;
    float3 uvw  : TEXCOORD0;
};

// Vertex shader
VSOutput VS_Main(
    float2 pos : POSITION 
)
{
    VSOutput o = (VSOutput)0;

    float4 uvw = float4(pos.xy, 1, 1);

    uvw = mul(uvw, mProjInv);
    uvw = uvw / uvw.w;
    o.viewpos = uvw;
    uvw = mul(uvw, mViewInv);
    uvw = normalize(uvw);



    //���� ���.
    o.pos = float4(pos.xy, 1.0f, 1.0f);
    o.uvw = uvw.xyz;
    return o;
}

// Pixel Shader 

TextureCube texSkyBox : register(t0);     //�������� ���� ����. 

SamplerState smpClamp;

float4 PS_Main(
    float4 pos : SV_POSITION,
    float4 viewpos : TEXCOORD101,
    float3 uvw : TEXCOORD0   
) : SV_TARGET               
{
   float4 tex = 1;
   tex = texSkyBox.Sample(smpClamp, normalize(uvw));

   float fog = 1;
   if (g_Fog.bOn == true)
       fog = Fog(viewpos);
  // float4 col = tex * fog + g_Fog.Color * g_mMat.Color * (1 - fog);
   float4 col = tex *g_mMat.Color;


   //col = LuminanceConversion(col);

   return col;

}


