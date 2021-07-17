//
//
//
// 인터페이스 - 오픈될 정보들
// 추후에 다른 API 와의 확정성을 고려하여 제작
// [2020/12/07 RUNSUMIN]
#pragma once
#include <windows.h>	// 윈도우 핸들..
#include <SimpleMath.h>	// 수학 헤더
#include <string>
#include <vector>

namespace GRAPHICENGINE
{
	using namespace DirectX::SimpleMath;
	using namespace std;
	///
	// 그래픽스 엔진 디바이스  셋팅
	/// Device
	enum class RenderState
	{
		CULLBACK, CULLFRONT, WIREFRAME, SOLID
	};

	enum class eChannelView
	{
		DEFAULT, DIFFUSE, NORMAL, AO, ROUGHNESS, MATALIC, SHADOWMAP, IBL
	};

	// 렌더링 순서.
	enum class RenderQueueSort
	{
		SORT_BACKGROUND = 100,	// 가장 먼저 그려짐
		SORT_OPAQUE = 200,		// 불투명 
		SORT_ALPHATEST = 300,	// 알파 테스트						// 알파 O
		SORT_OPAQUELAST = 400,	// 불투명 마지막
		SORT_TRANSPARENT = 500,	// 투명(순서는 뒤에서 앞으로)			// 알파 X
		SORT_OVERLAY = 600,		// 중첩 효과
		SORT_UI = 700			// 유저 인터페이스
	};

	// 엔진 셋팅
	bool EngineSetUp(HWND hwnd);
	// 그리기 시작
	void BeginRender(Vector4 backbuffercolor);
	// 그리기 종료
	void EndRender();
	// 엔진 해제
	void Release();
	// 엔진 타임 받아오기
	float GetEngineTime();
	// FPS
	float GetFramePerSecond();
	// 렌더 모드 변경
	void ChangeRenderState(RenderState renderstate);
	// 카메라 매트릭스 Getter
	void GetCameraMatrixOnce(Matrix view, Matrix proj);
	// 빛 정보 Getter
	void GetDirectionLightData(Vector4 Direction, Vector4 Diffuse, Vector4 Ambient);
	// 뷰포트 변경
	void SetViewport(float w, float h);
	// 전체 Draw
	void DrawAll(float time);
	// View Frustum Culling On/Off
	void SetViewFrustumCulling(bool b);
	// Shadow On, Off
	void SetShadow(bool b, Vector3 centerposition, float radious);
	// Map ChannelChange
	void SetChannelView(eChannelView view);

	/// FbxLoader(그래픽스 엔진 내부 포함)
	bool FbxLoaderSetUp();										/// 꼭 해줘야 하는 부분
	void ReleaseFbxLoader();									/// 꼭 해줘야 하는 부분

	// 화면에 문자 출력...
	//void DrawText(int x , int y, const)

	/// Camera
	// 201211 사용 안할듯
	class ICameraInterface abstract
	{
	public:
		enum class Mode
		{
			DEFAULT, AUTOROTATION, MOVE, FOLLOW, ORTHO, INPUT
		};
	public:
		// 카메라 초기화
		virtual void InitializeCamera() abstract;
		// 카메라 업데이트
		virtual void UpdateCamera(float dTime) abstract;
		// 카메라 모드 변경
		virtual void ChangeCameraMode(Mode emode) abstract;
		// 카메라 데이터 설정
		virtual void SetCamerSRTData(Vector3 pos, Vector3 rot, Vector3 scl) abstract;
		// 카메라 스피드 설정
		virtual void SetCameraSpeed(float speed) abstract;
		// info
		virtual void CameraInfo() abstract;
		// delete
		virtual void ReleaseCamera() abstract;
		// Get,Set
		virtual Matrix GetCameraViewMat() abstract;
		virtual Matrix GetCameraProjMat() abstract;
		virtual Vector3 GetCameraPos() abstract;
		virtual Vector3 GetCameraRot() abstract;
		virtual Vector3 GetCameraScl() abstract;
	};

	/// Grid
	class IGridInterface abstract
	{
	public:
		virtual void InitializeGrid(int numx, int numy, float size) abstract;
		virtual void UpdateGrid() abstract;
		virtual void DrawGrid()	abstract;
		virtual void ReleaseGrid() abstract;
		virtual void ChangeGridSize(int numx, int numy, float size) abstract;
	};


	/// Light - Direction
	class IDirectionLightInterface abstract
	{
	public:
		struct IDirectrionLitData
		{
			Vector4 Direction;
			Vector4 Diffuse;
			Vector4 Ambient;
		};
	public:
		virtual void InitializeDirectrionLight(Vector4 dir, Vector4 dif, Vector4 amb) abstract;
		virtual void SetDirectrionLightDirection(Vector4 dir) abstract;
		virtual void SetDirectrionLightDiffuse(Vector4 dif) abstract;
		virtual void SetDirectrionLightAmbient(Vector4 amb) abstract;
	};

	/// Light - Point
	class IPointLightInterface abstract
	{
	public:
		struct IPointLitData
		{
			Vector4 Diffuse;
			Vector4 Ambient;
			Vector3 Position;
			float Range;
		};
	public:
		virtual void InitializePointLight(Vector4 dif, Vector4 amb, Vector3 pos, float range) abstract;
		virtual void SetPointLightDIffuse(Vector4 dif) abstract;
		virtual void SetPointLightAmbient(Vector4 amb) abstract;
		virtual void SetPointLightPosition(Vector3 pos) abstract;
		virtual void SetPointLightRange(float range) abstract;
	};

	/// Material
	class IMaterialInterface abstract
	{
	public:
		enum class TextureSort
		{
			ALBEDO, NORMAL, ORM, MASKMAP, EMISSIVE
		};
	public:
		virtual void SetTexturePath(string filename, TextureSort sort) abstract;
		virtual string GetTexturePath(TextureSort sort) abstract;
		virtual void SetLinkNodename(string nodename) abstract;
		virtual string GetLinkNodename() abstract;
	};

 	/// UI Material
	//class IMaterialInterface abstract
	//{
	//public:
	//	enum class TextureSort
	//	{
	//		ALBEDO, NORMAL, ORM, MASKMAP, EMISSIVE
	//	};
	//public:
	//	virtual void InitMaterial() abstract;
	//	virtual void SetTexture(string filename, TextureSort sort) abstract;
	//};


	/// Model
	class IModelInterface abstract
	{
	public:
		enum class ModelSort
		{
			SKINNED_MESH, STATIC_MESH
		};

		enum class RenderPath
		{
			Default, Depth
		};

	public:
		virtual void InitFbxModel() abstract;
		virtual void InitModelTexture() abstract;
		virtual void FbxModelUpdate(float time) abstract;
		virtual void FbxModelDraw(float time, int nodecount, RenderPath path, Matrix shadow) abstract;
		virtual void FbxModelInfo() abstract;
		virtual void ReleaseFbxModel() abstract;
		virtual void SetUserMatrix(Matrix usertm) abstract;
		virtual Matrix GetUserMatrix() abstract;
		virtual string GetModelName() abstract;
		virtual string GetNodeName(int nodecount) abstract;
		virtual int GetModelNodeCount() abstract;
		virtual RenderQueueSort GetModelNodeRendersort(int nodecount) abstract;
		virtual void SetModelNodeRenderQueuesort(int nodecount, RenderQueueSort sort) abstract;		// 모델 노드당 렌더 순서 지정
		virtual void SetDrawScreen(bool draw) abstract;
		virtual bool GetDrawScreen() abstract;
		virtual void SetModelRenderState(RenderState state) abstract;
		virtual Vector3 GetWorldModelPosition() abstract;
		virtual float GetCollSphereRadious() abstract;
		virtual ModelSort GetModelSort() abstract;

		// 애니메이션 전용... 분리 필요
		virtual void SetAnimationSpeed(float speed) abstract;		// 스피드 설정
		virtual void ChangeAnimation(int count) abstract;			// 애니메이션 전환
		virtual float GetAniTotalTime() abstract;					// 애니메이션 총 시간
		virtual bool GetAniEndTimeing() abstract;					// 애니메이션 마지막 타임 받아옴
		virtual int GetAniTotalKeyCount() abstract;					// 애니메이션 총 프레임
		virtual int GetAniNowKeyCount()	abstract;					// 애니메이션 현채 
		virtual void SetNowKeyCount(int key) abstract;				// 현재 키 설정
		virtual void SetAniOn() abstract;							// 애니메이션 ON;
		virtual void SetAniOff() abstract;							// 애니메이션 OFF
		virtual vector<string> GetBoneOffSetName() abstract;		// BoneOffSet

		// 애니메이션은 기본적으로 한번만 돈다. loop를 true 시켜주면 loop함
		virtual void SetAnimationLoop(bool loop) abstract;
		// Bone Matrix 정보 받아오기(이름)
		virtual Matrix GetHiearchyMatrixByName(string name) abstract;

		// Material
		virtual void SetMaterialData(IMaterialInterface* pobj, string nodename, IMaterialInterface::TextureSort sort) abstract;
		virtual void SetEmissiveMapOnOff(bool b) abstract;
		virtual void AddEmissiveMap(string filename) abstract;
		virtual void ChangeApplyEmissiveMap(int index) abstract;

		//Light Info
		virtual void SetDirectionLightData(IDirectionLightInterface::IDirectrionLitData data) abstract;
		virtual void SetPointLightData(IPointLightInterface::IPointLitData data) abstract;
	};

	/// UI
	class IUiInterface abstract
	{
	public:
		virtual	void UpdateUI(Vector3 pos, Vector3 vScl) abstract;
		virtual	void DrawUI() abstract;
		virtual	DWORD GetTextureWidth() abstract;
		virtual	DWORD GetTextureHeight() abstract;
		virtual void SetOverlayColorOnUI(Vector4 color) abstract;
		virtual void SetUISize(float x, float y) abstract;
		virtual	void ReleaseUI() abstract;
		virtual	void OnUi() abstract;
		virtual	void OffUi() abstract;
	};

	/// Geometry
	enum class Geomerty_Sort
	{
		BOX, SPHERE, CYLINDER, LINE
	};
	class IGeoMetry_InterFace abstract
	{
	public:
		virtual void Initialize() abstract;
		virtual void Update(Matrix World) abstract;
		virtual void Draw() abstract;
		virtual void Release() abstract;
		virtual void ChangeColor(Vector4 color) abstract;
	};



	/// Gizmno
	class IGizmo_InterFace abstract
	{
	public:
		virtual int InitAxis() abstract;
		virtual int Update(Matrix world) abstract;
		virtual int Render() abstract;
		virtual void Release() abstract;
	};

	/// Terrain
	class ITerrain_Interface abstract
	{
	public:
		enum class RenderPath
		{
			Default, Depth, Shadow
		};
	public:
		virtual void InitTerrain() abstract;
		virtual void Update() abstract;
		virtual void Render(RenderPath path, Matrix shadow, bool b) abstract;
		virtual void Release() abstract;
	};


	/// SkyBox
	class ISkyBoxInterface abstract
	{
	public:
		virtual void InitSkybox() abstract;
		virtual void ChangeSkyboxColor(Vector4 Color) abstract;
		virtual void DrawSkybox() abstract;
		virtual void ReleaseSkybox() abstract;
	};

	/// Create
	// Camera
	void CreateCamera(ICameraInterface** pobj, Vector3 pos, Vector3 rot);

	// Grid
	void CreateGird_Debug(IGridInterface** pobj);

	/// Mesh Low data Load
	// FBX 파일에서 데이터 로드 후 리스트에 저장
	void CreateModelObject(string filename);		/// 모델 데이터 fbxloader에서 로드

	// 애니메이션 리스트를 추가하는 함수. 순서를 사용자가 알아야 하는 불편함이 있다.
	void CreateModelObject_List(vector<string> filenamelist);	/// 모델 애니메이션 포함

	//////////////////////////////////////////////////////////////////////////

	///Material - Texture
	void CreateModelMaterial_Texture();

	/// Inputer - interface to GameObject
	// 데이터 리스트에서 이름으로 파일 로드.
	void SetModelObject(IModelInterface** pobj, string filename);

	// 애니메이션 리스트를 추가하는 함수. 순서를 사용자가 알아야 하는 불편함이 있다.
	/// 삭제
	//void SetModelObject_List(IModelInterface** pobj, vector<string> filenamelist);

	// UI
	void SetUiObject(IUiInterface** pobj, Vector3 pos, Vector3 vScl, string filename);

	// Geometry
	void CreateGeometry(IGeoMetry_InterFace** pobj, Geomerty_Sort sort, float width, float height, float depth, Vector4 color);

	// Gizmo
	void CreateGizmo(IGizmo_InterFace** pobj);

	// Terrain
	void CreateTerrain_Obj(ITerrain_Interface** obj, string filename, float width, float height, float slice);

	// Skybox
	void CreateSkybox_Obj(ISkyBoxInterface** obj, string filename);

	// Material
	void CreateMaterial_Mesh(IMaterialInterface** obj);


	/// Light
	void CreateDirectionLight(IDirectionLightInterface** pobj);

	void CreatePointLight(IPointLightInterface** pobj);

	///RenderQueue
	//
	void ClearRenderQueue_Model();
	void ClearRenderQueue_UI();
	void DeleteQueueListcertainindex(int index);

	/// FOG
	void SetFogOnOff(bool b);	// OnOff
	void SetFogColor(Vector4 col);
	void SetFogDensity(float f);	// Max - 1 

	///BLoom
	void BloomOnOff(bool b);
}


