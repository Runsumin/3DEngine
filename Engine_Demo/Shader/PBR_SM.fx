///
///
/// 
/// PBR - Base on UNREALENGINE
/// SOURCE
/// Real Shading in Unreal Engine 4
/// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
/// http://viclw17.github.io/2019/06/15/PBR_note/
///
/// Physically Based Rendering - Cook–Torrance
/// http://www.codinglabs.net/article_physically_based_rendering_cook_torrance.aspx
/// [2021/06/09 RUNSUMIN]

static const float PI = 3.14159265f;
static const float ONE_OVER_PI = 0.318309f;
static const float ONE_OVER_TWO_PI = 0.159154f;
static const float EPSILON = 1e-6f;

float3 CalDiffuse(in float3 albedo)
{
	return albedo * ONE_OVER_PI;
}

float3 CalHalfVector(in float3 Litvec, in float3 Viewvec)
{
	return normalize(Litvec + Viewvec);
}

// Specular D -  Normal distribution
// NDFGGXTR(n,h,α)=α2π((n⋅h)2(α2−1)+1)2
float CalNDF(in float3 surfNorm, in float3 halfVector, in float roughness)
{
    float a2 = (roughness * roughness);
    float halfAngle = dot(surfNorm, halfVector);

    return (a2 / (PI * pow((pow(halfAngle, 2.0) * (a2 - 1.0) + 1.0), 2.0)));
}
/**
 * GGX/Schlick-Beckmann microfacet geometric attenuation.
 * GSchlickGGX(n,v,k)=n⋅v(n⋅v)(1−k)+k
 * k=(Roughness+1)28
 */
float CalculateAttenuation(in float3 surfNorm, in float3 vec, in float k)
{
    float d = max(dot(surfNorm, vec), 0.0);
    return (d / ((d * (1.0 - k)) + k));
}

/**
 * GGX/Schlick-Beckmann attenuation for analytical light sources.
 */
float CalculateAttenuationAnalytical(in float3 surfNorm, in float3 toLight, in float3 toView, in float roughness)
{
    float k = pow((roughness + 1.0), 2.0) * 0.125;

    // G(l) and G(v)
    float lightAtten = CalculateAttenuation(surfNorm, toLight, k);
    float viewAtten = CalculateAttenuation(surfNorm, toView, k);

    // Smith
    return (lightAtten * viewAtten);
}

/**
 * Calculates the Fresnel reflectivity.
 */
float CalculateFresnel(in float3 surfNorm, in float3 toView, in float3 fresnel0)
{
    float d = max(dot(surfNorm, toView), 0.0);
    float p = ((-5.55473 * d) - 6.98316) * d;

    // Fresnel-Schlick approximation
    return fresnel0 + ((1.0 - fresnel0) * pow(1.0 - d, 5.0));
    // modified by Spherical Gaussian approximation to replace the power, more efficient
    return fresnel0 + ((1.0 - fresnel0) * pow(2.0, p));
}