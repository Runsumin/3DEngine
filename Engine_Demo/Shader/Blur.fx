///
///
/// 
/// Gaussian Blur
/// [2021/06/11 RUNSUMIN]

#define POSTEFFECT_BLUR_SAMPLING_COUNT 9
// FHD
#define SCREEMSIZ_WIDTH 2048
#define SCREEMSIZ_HEIGHT 2048   

//float4  vScreenSize; // .x = width, .y = height, .z = 1/width, .w = 1/height

// X Direction Blur
float4 Blur_X(float2 uv, SamplerState smp, Texture2D tex)
{
	float weights[POSTEFFECT_BLUR_SAMPLING_COUNT] =
	{
		0.013519569015984728,
		0.047662179108871855,
		0.11723004402070096,
		0.20116755999375591,
		0.240841295721373,
		0.20116755999375591,
		0.11723004402070096,
		0.047662179108871855,
		0.013519569015984728

	};

	float indices[POSTEFFECT_BLUR_SAMPLING_COUNT] = { -4, -3, -2, -1, 0, +1, +2, +3, +4 };
	float2 dir = float2(1.0, 0.0);

	float2 step = float2(1.0f / SCREEMSIZ_WIDTH, 0);
	float4 Result = 0.0f;

	for (int i = 0; i < POSTEFFECT_BLUR_SAMPLING_COUNT; i++)
	{
		Result += tex.Sample(smp, float2(uv + indices[i] * step)) * weights[i];
	}

	return float4(Result.rgb, 1);
}

// Y Direction Blur
float4 Blur_Y(float2 uv, SamplerState smp, Texture2D tex)
{
	float weights[POSTEFFECT_BLUR_SAMPLING_COUNT] =
	{
		0.013519569015984728,
		0.047662179108871855,
		0.11723004402070096,
		0.20116755999375591,
		0.240841295721373,
		0.20116755999375591,
		0.11723004402070096,
		0.047662179108871855,
		0.013519569015984728

	};

	float indices[POSTEFFECT_BLUR_SAMPLING_COUNT] = { -4, -3, -2, -1, 0, +1, +2, +3, +4 };
	float2 dir = float2(1.0, 0.0);

	float2 step = float2(0, 1.0f / SCREEMSIZ_WIDTH);
	float4 Result = 0.0f;
	for (int i = 0; i < POSTEFFECT_BLUR_SAMPLING_COUNT; i++)
	{
		Result += tex.Sample(smp, float2(uv + indices[i] * step)) * weights[i];
	}

	return float4(Result.rgb, 1);
}

// Y Direction Blur
//float4 Blur_Y(float2 uv, SamplerState smp, float4 tex)
//{
//	float weights[POSTEFFECT_BLUR_SAMPLING_COUNT] =
//	{
//		0.0561f,
//		0.1353f,
//		0.2780f,
//		0.4868f,
//		0.7261f,
//		0.9231f,
//		1.0f,
//		0.9231f,
//		0.7261f,
//		0.4868f,
//		0.2780f,
//		0.1353f,
//		0.0561f
//	};
//
//	float indices[POSTEFFECT_BLUR_SAMPLING_COUNT] = { -6,-5, -4, -3, -2, -1, 0, +1, +2, +3, +4 ,+5,+6 };
//	float2 dir = float2(0.0, 1.0);
//
//	float2 step = float2(0, 1.0f / SCREEMSIZ_WIDTH);
//	float4 Result = 0.0f;
//	float total = 6.2108f;
//	for (int i = 0; i < POSTEFFECT_BLUR_SAMPLING_COUNT; i++)
//	{
//		Result += tex * weights[i];
//	}
//
//	return float4(Result.rgb / total, 1);
//}


float4 Blur_EX(SamplerState smp, Texture2D tex, in float2 texuv[9])
{
	//float weights[POSTEFFECT_BLUR_SAMPLING_COUNT] =
	//{
	//	0.013519569015984728,
	//	0.047662179108871855,
	//	0.11723004402070096,
	//	0.20116755999375591,
	//	0.240841295721373,
	//	0.20116755999375591,
	//	0.11723004402070096,
	//	0.047662179108871855,
	//	0.013519569015984728
	//};

	//float weights[POSTEFFECT_BLUR_SAMPLING_COUNT] =
	//{
	//	0.0561f,
	//	0.1353f,
	//	0.2780f,
	//	0.4868f,
	//	0.7261f,
	//	0.9231f,
	//	1.0f,
	//	0.9231f,
	//	0.7261f,
	//	0.4868f,
	//	0.2780f,
	//	0.1353f,
	//	0.0561f
	//};

	float4 Color = 0.0f;
	//for (int i = 0; i < POSTEFFECT_BLUR_SAMPLING_COUNT; i++)
	//{
	//	Color += tex.Sample(smp, texuv[i]) * weights[i];
	//}

	//Color.w = 1.0f;

	return Color;
}