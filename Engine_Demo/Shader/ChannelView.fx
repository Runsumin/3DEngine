///
///
///
/// 디버그용 채널 뷰
/// [2021/07/12 RUNSUMIN]

// 조명 정보
struct CHANNELVIEW
{
	bool Default;
	bool Diffuse;
	bool Normal;
	bool AmbientOcclusion;
	bool Roughness;
	bool Metalic;
	bool ShadowMap;
	bool IBL_SkyBox;
};

cbuffer cbChView : register(b5)
{
	CHANNELVIEW g_ChView;
};
