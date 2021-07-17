///
///
/// 
/// Tone Mapping
/// [2021/06/11 RUNSUMIN]

// ALPHA - X 
static const float Luminance = 0.08f;
static const float fMiddleGray = 0.18f;
static const float fWhiteCutoff = 0.9f;


// - WhitePoint
float3 ReinHard_ToneMapping(float3 color)
{
	float3 final = color;
	final = final / (final + float3(1.0f, 1.0f, 1.0f));
	return final;
}

float3 Flimic_ToneMapping(float3 color)
{
	float3 tone = max(0, color.xyz - 0.004f);
	float3 final = (tone * (6.2f * tone + 0.5f)) / (tone * (6.2f * tone + 1.7f) + 0.06f);

	return final;
}

float3 Uncharted2_ToneMapping(float3 color)
{
	float a = 0.15f;	// shulder
	float b = 0.5f;		// Linear strength
	float c = 0.1f;		// Linear Angle
	float d = 0.2f;		// Toe strength
	float e = 0.02f;	// ToeNumerator
	float f = 0.3f;		// ToeDenominator

	//return ((color * (a * color + c * b)) + d * d) / (color * (a * color + b) + d * f)) - e / f;
}