///
///
///
/// Shadow fx
/// [2021/06/29 RUNSUMIN]

SamplerComparisonState ShadowSampler
{
	ComparisonFunc = LESS;
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
};

float CalShadowMount(float4 LightPos, Texture2D ShadowMap)
{
	float Result = 0.0f;
	float depth = 0.0f;
	float3 uvz = 1.0f;

	uvz.x = LightPos.x / LightPos.w;
	uvz.y = LightPos.y / LightPos.w;
	uvz.z = LightPos.z / LightPos.w;

	// 그릴 영역 벗어나면...
	if (uvz.x < -1.0f || uvz.x > +1.0f ||
		uvz.y < -1.0f || uvz.y > +1.0f ||
		uvz.z < +0.0f || uvz.z > +1.0f)
	{
		Result = 1.0f;
		return Result;
	}

	// 범위 0 ~ 1로 변환
	uvz.x = uvz.x * 0.5f + 0.5f;
	uvz.y = uvz.y * -0.5f + 0.5f;

	depth = uvz.z;

	const float dx = 1 / 4096;

	const float2 offsets[9] =
	{
		 float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  dx)
	};

	float2 uvs = 0;
	float sum = 0;
	[unroll(9)]
	for (int i = 0; i < 9; i++)
	{
		uvs = uvz.xy + offsets[i];
		sum += ShadowMap.SampleCmpLevelZero(ShadowSampler, uvs, depth).r;
	}

	Result = sum / 9.0f;

	return Result;
}