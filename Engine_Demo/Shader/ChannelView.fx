///
///
///
/// ����׿� ä�� ��
/// [2021/07/12 RUNSUMIN]

// ���� ����
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
