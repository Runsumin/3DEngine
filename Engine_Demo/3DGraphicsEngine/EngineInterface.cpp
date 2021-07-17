#include "D3DDevice.h"
#include "FbxLoader.h"
#include "EngineInterface.h"
#include "UI.h"
#include "Camera.h"
#include "Terrain.h"
#include "Light.h"
#include "Grid.h"
#include "Geometry_Generater.h"
#include "Axis.h"
#include "Render_Queue.h"
#include "MeshFilter.h"
#include "SKinned_Mesh.h"
#include "Static_Mesh.h"
#include "Terrain.h"
#include "FbxDataExporter.h"
#include "ShaderMgr.h"
#include "Skybox.h"
#include "Material.h"
#include "Light.h"

D3DDevice* g_Dev = nullptr;										// 장치 관리 객체
FbxLoader* g_FbxLoader = nullptr;								// 메쉬 로드 관리 객체
Render_Queue* g_RenderQueue = nullptr;							// RenderQueue
FbxDataExporter* g_MetadataExporter = new FbxDataExporter();	// BinaryFile
// 한번만 갱신될 데이터들....
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GRAPHICENGINE::EngineSetUp(HWND hwnd)
{
	if (g_Dev != nullptr)
		return false;
	g_Dev = D3DDevice::GetInstance();
	g_Dev->EngineSetUp(hwnd);

	// Shader Mgr
	ShaderMgr::GetInstance()->InitShaderMgr();

	// RenderQueue Generate
	g_RenderQueue = new Render_Queue();	// 렌더 큐

	return true;
}

/// 위 아래 두개 합치자
bool GRAPHICENGINE::FbxLoaderSetUp()
{
	if (g_FbxLoader != nullptr)
		return false;
	// Fbx Loader Singleton 일 필요 없음.. 수정 필요
	g_FbxLoader = new FbxLoader();
	g_FbxLoader->InitializeFbxLoader();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GRAPHICENGINE::BeginRender(Vector4 backbuffercolor)
{
	if (g_Dev == nullptr)
		throw;
	g_Dev->BeginRend(backbuffercolor);

}

void GRAPHICENGINE::EndRender()
{
	if (g_Dev == nullptr)
		throw;
	g_Dev->EndRend();

}

void GRAPHICENGINE::Release()
{
	if (g_Dev == nullptr)
		throw;
	g_Dev->Release();
	ShaderMgr::GetInstance()->ReleaseShaderMgr();
	g_MetadataExporter->DeleteImportDataList();
	delete g_RenderQueue;
	delete g_Dev;
	delete g_MetadataExporter;

	DrawText_Debug::GetInstance()->Finalize();

}

float GRAPHICENGINE::GetEngineTime()
{
	if (g_Dev == nullptr)
		throw;
	float time = g_Dev->GetEngineTime();
	return time;

}

float GRAPHICENGINE::GetFramePerSecond()
{
	if (g_Dev == nullptr)
		throw;
	float time = g_Dev->GetFramePerSecond();
	return time;
}

void GRAPHICENGINE::ChangeRenderState(RenderState renderstate)
{
	if (g_Dev == nullptr)
		throw;
	g_Dev->ChangeRenderState(renderstate);

}

void GRAPHICENGINE::GetCameraMatrixOnce(Matrix view, Matrix proj)
{
	if (g_Dev == nullptr)
		throw;
	g_Dev->GetCameraMatrixOnce(view, proj);
}

void GRAPHICENGINE::GetDirectionLightData(Vector4 Direction, Vector4 Diffuse, Vector4 Ambient)
{
	if (g_Dev == nullptr)
		throw;
	g_Dev->GetDirectionLightData(Direction, Diffuse, Ambient);
}

void GRAPHICENGINE::SetViewport(float w, float h)
{
	if (g_Dev == nullptr)
		throw;
	g_Dev->SetViewport(w, h);
}

void GRAPHICENGINE::DrawAll(float time)
{
	if (g_RenderQueue == nullptr)
		throw;
	g_RenderQueue->DrawAll(time);
}

void GRAPHICENGINE::SetViewFrustumCulling(bool b)
{
	g_RenderQueue->SetCulling(b);
}

void GRAPHICENGINE::SetShadow(bool b, Vector3 centerposition, float radious)
{
	g_RenderQueue->m_TargetPosition = centerposition;
	g_RenderQueue->m_ShadowRadious = radious;
	g_RenderQueue->m_ShadowOn = b;

}

void GRAPHICENGINE::SetChannelView(eChannelView view)
{
	switch (view)
	{
	case GRAPHICENGINE::eChannelView::DEFAULT:
		g_RenderQueue->m_ChannelView.Default = true;
		g_RenderQueue->m_ChannelView.Diffuse = false;
		g_RenderQueue->m_ChannelView.AmbientOcclusion = false;
		g_RenderQueue->m_ChannelView.Normal = false;
		g_RenderQueue->m_ChannelView.IBL_SkyBox = false;
		g_RenderQueue->m_ChannelView.Metalic = false;
		g_RenderQueue->m_ChannelView.Roughness = false;
		g_RenderQueue->m_ChannelView.ShadowMap = false;
		break;
	case GRAPHICENGINE::eChannelView::DIFFUSE:
		g_RenderQueue->m_ChannelView.Default = false;
		g_RenderQueue->m_ChannelView.Diffuse = true;
		g_RenderQueue->m_ChannelView.AmbientOcclusion = false;
		g_RenderQueue->m_ChannelView.Normal = false;
		g_RenderQueue->m_ChannelView.IBL_SkyBox = false;
		g_RenderQueue->m_ChannelView.Metalic = false;
		g_RenderQueue->m_ChannelView.Roughness = false;
		g_RenderQueue->m_ChannelView.ShadowMap = false;
		break;
	case GRAPHICENGINE::eChannelView::NORMAL:
		g_RenderQueue->m_ChannelView.Default = false;
		g_RenderQueue->m_ChannelView.Diffuse = false;
		g_RenderQueue->m_ChannelView.AmbientOcclusion = false;
		g_RenderQueue->m_ChannelView.Normal = true;
		g_RenderQueue->m_ChannelView.IBL_SkyBox = false;
		g_RenderQueue->m_ChannelView.Metalic = false;
		g_RenderQueue->m_ChannelView.Roughness = false;
		g_RenderQueue->m_ChannelView.ShadowMap = false;
		break;
	case GRAPHICENGINE::eChannelView::AO:
		g_RenderQueue->m_ChannelView.Default = false;
		g_RenderQueue->m_ChannelView.Diffuse = false;
		g_RenderQueue->m_ChannelView.AmbientOcclusion = true;
		g_RenderQueue->m_ChannelView.Normal = false;
		g_RenderQueue->m_ChannelView.IBL_SkyBox = false;
		g_RenderQueue->m_ChannelView.Metalic = false;
		g_RenderQueue->m_ChannelView.Roughness = false;
		g_RenderQueue->m_ChannelView.ShadowMap = false;
		break;
	case GRAPHICENGINE::eChannelView::ROUGHNESS:
		g_RenderQueue->m_ChannelView.Default = false;
		g_RenderQueue->m_ChannelView.Diffuse = false;
		g_RenderQueue->m_ChannelView.AmbientOcclusion = false;
		g_RenderQueue->m_ChannelView.Normal = false;
		g_RenderQueue->m_ChannelView.IBL_SkyBox = false;
		g_RenderQueue->m_ChannelView.Metalic = false;
		g_RenderQueue->m_ChannelView.Roughness = true;
		g_RenderQueue->m_ChannelView.ShadowMap = false;
		break;
	case GRAPHICENGINE::eChannelView::MATALIC:
		g_RenderQueue->m_ChannelView.Default = false;
		g_RenderQueue->m_ChannelView.Diffuse = false;
		g_RenderQueue->m_ChannelView.AmbientOcclusion = false;
		g_RenderQueue->m_ChannelView.Normal = false;
		g_RenderQueue->m_ChannelView.IBL_SkyBox = false;
		g_RenderQueue->m_ChannelView.Metalic = true;
		g_RenderQueue->m_ChannelView.Roughness = false;
		g_RenderQueue->m_ChannelView.ShadowMap = false;
		break;
	case GRAPHICENGINE::eChannelView::SHADOWMAP:
		g_RenderQueue->m_ChannelView.Default = false;
		g_RenderQueue->m_ChannelView.Diffuse = false;
		g_RenderQueue->m_ChannelView.AmbientOcclusion = false;
		g_RenderQueue->m_ChannelView.Normal = false;
		g_RenderQueue->m_ChannelView.IBL_SkyBox = false;
		g_RenderQueue->m_ChannelView.Metalic = false;
		g_RenderQueue->m_ChannelView.Roughness = false;
		g_RenderQueue->m_ChannelView.ShadowMap = true;
		break;
	case GRAPHICENGINE::eChannelView::IBL:
		g_RenderQueue->m_ChannelView.Default = false;
		g_RenderQueue->m_ChannelView.Diffuse = false;
		g_RenderQueue->m_ChannelView.AmbientOcclusion = false;
		g_RenderQueue->m_ChannelView.Normal = false;
		g_RenderQueue->m_ChannelView.IBL_SkyBox = true;
		g_RenderQueue->m_ChannelView.Metalic = false;
		g_RenderQueue->m_ChannelView.Roughness = false;
		g_RenderQueue->m_ChannelView.ShadowMap = false;
		break;
	}

	ShaderMgr::GetInstance()->m_ChannelView = g_RenderQueue->m_ChannelView;
}

void GRAPHICENGINE::BloomOnOff(bool b)
{
	g_RenderQueue->m_bBloom = b;
}

void GRAPHICENGINE::CreateCamera(ICameraInterface** pobj, Vector3 pos, Vector3 rot)
{
	Camera* temp = new Camera(pos, rot);
	*pobj = temp;
}

void GRAPHICENGINE::CreateGird_Debug(IGridInterface** pobj)
{
	Grid* temp = new Grid();
	*pobj = temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GRAPHICENGINE::ReleaseFbxLoader()
{
	if (g_FbxLoader == nullptr)
		throw;
	g_FbxLoader->ReleaseFbxLoader();
	delete g_FbxLoader;
}

void GRAPHICENGINE::CreateModelObject(string filename)
{
	/// 파일 이름으로 리스트에 맵 추가.
	Fbx_MeshData* pmodel = nullptr;

	if (g_FbxLoader == nullptr)
		throw;
	string binname = g_MetadataExporter->detachstring_fbx(filename);
	if (g_MetadataExporter->FindBinaryfile(binname) == true)
	{
		// bin data 리스트에 추가
		g_MetadataExporter->ImportFbxMetafile(binname.c_str());
	}
	else
	{
		// fbx 원본 데이터 리스트에 추가
		// 이부분은 처음 로드할때만 적용된다.
		pmodel = g_FbxLoader->LoadModel(filename);
		// bin 파일 생성
		g_MetadataExporter->ExportFbxData_Flex(filename, pmodel);
	}

}

void GRAPHICENGINE::CreateModelObject_List(vector<string> filenamelist)
{
	/// 파일 이름으로 리스트에 맵 추가.
	Fbx_MeshData* pmodel = nullptr;

	string binname = g_MetadataExporter->detachstring_fbx(filenamelist[0]);
	if (g_MetadataExporter->FindBinaryfile(binname) == true)
	{
		//// bin data 리스트에 추가
		g_MetadataExporter->ImportFbxMetafile(binname.c_str());
	}
	else
	{
		// bin 파일 없을때...
		int cnt = filenamelist.size();
		for (int i = 0; i < cnt; i++)
		{
			if (i == 0)
				pmodel = g_FbxLoader->LoadModel(filenamelist[i]);	// 모델 데이터 구성.
			else
			{
				pmodel->m_pAniDataList.push_back(g_FbxLoader->LoadAnimationDataStack(filenamelist[i]));
			}
		}

		// bin 파일 생성
		g_MetadataExporter->ExportFbxData_Flex(filenamelist[0], pmodel);
	}
}

void GRAPHICENGINE::CreateModelMaterial_Texture()
{
	// 텍스쳐 데이터 생성...
	g_FbxLoader->SetTextrueDate();
	g_MetadataExporter->SetTextrueDate();
}

void GRAPHICENGINE::SetModelObject(IModelInterface** pobj, string filename)
{
	if (g_FbxLoader == nullptr)
		throw;
	// 먼저 binary 파일이 있는지 검사하고 없으면 만들어준다.
	Fbx_MeshData* pmodel = nullptr;
	string binname = g_MetadataExporter->detachstring_fbx(filename);
	if (g_MetadataExporter->FindBinaryfile(binname) == true)
	{
		pmodel = g_MetadataExporter->SetFbxBinFile(binname.c_str());
	}
	else
	{
		pmodel = g_FbxLoader->SetModel(filename);
	}

	//////////////////////////////////////////////////////////////////////////

	MeshFilter::eMeshfiltersort sort = pmodel->m_MeshFilterSort;
	switch (sort)
	{
	case MeshFilter::eMeshfiltersort::SkinnedMesh:
	{
		SKinned_Mesh* pskinmesh = new SKinned_Mesh();
		pskinmesh->SetModelData(pmodel);
		*pobj = pskinmesh;
	}
	break;
	case MeshFilter::eMeshfiltersort::StaticMesh:
	{
		Static_Mesh* pstaticmesh = new Static_Mesh();
		pstaticmesh->SetModelData(pmodel);
		*pobj = pstaticmesh;
	}
	break;
	}
	// 렌더큐에 Push
	g_RenderQueue->PushBackModelDrawList(*pobj);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GRAPHICENGINE::SetUiObject(IUiInterface** pobj, Vector3 pos, Vector3 vScl, string filename)
{
	UI* temp = new UI(pos, vScl, filename);
	*pobj = temp;
	g_RenderQueue->PushBackUIDrawList(*pobj);
}

void GRAPHICENGINE::CreateGeometry(IGeoMetry_InterFace** pobj, Geomerty_Sort sort, float width, float height, float depth, Vector4 color)
{
	/// <summary>
	/// BOX : width, height, depth
	/// Sphere : radious, slice stack
	/// Line : Lenght, thickness, 0
	/// </summary>

	Geometry_Generater* temp = nullptr;

	switch (sort)
	{
	case GRAPHICENGINE::Geomerty_Sort::BOX:
		temp = new Box(width, height, depth, color);
		break;
	case GRAPHICENGINE::Geomerty_Sort::SPHERE:
		temp = new Sphere(width, height, depth, color);
		break;
	case GRAPHICENGINE::Geomerty_Sort::CYLINDER:
		break;
	case GRAPHICENGINE::Geomerty_Sort::LINE:
		break;
	default:
		break;
	}

	*pobj = temp;
}

void GRAPHICENGINE::CreateGizmo(IGizmo_InterFace** pobj)
{
	Axis* tmp = new Axis();
	*pobj = tmp;
}

void GRAPHICENGINE::CreateTerrain_Obj(ITerrain_Interface** obj, string filename, float width, float height, float slice)
{
	Terrain* tmp = new Terrain(filename, width, height, slice);
	*obj = tmp;

	// 렌더큐에 집어넣기
	g_RenderQueue->PushBackTerrainDrawList(*obj);
}

void GRAPHICENGINE::CreateSkybox_Obj(ISkyBoxInterface** obj, string filename)
{
	Skybox* tmp = new Skybox(filename);
	*obj = tmp;

	g_RenderQueue->GetSkyBoxPointer(*obj);
}

void GRAPHICENGINE::CreateMaterial_Mesh(IMaterialInterface** obj)
{
	Material* tmp = new Material();
	*obj = tmp;
}

void GRAPHICENGINE::CreateDirectionLight(IDirectionLightInterface** pobj)
{
	DirectionLight* tmp = new DirectionLight();
	*pobj = tmp;

	g_RenderQueue->m_DirectionLightList.push_back(tmp);
}

void GRAPHICENGINE::CreatePointLight(IPointLightInterface** pobj)
{
	PointLight* tmp = new PointLight();
	*pobj = tmp;

	g_RenderQueue->m_PointLightList.push_back(tmp);
}

void GRAPHICENGINE::ClearRenderQueue_Model()
{
	g_RenderQueue->ResetRenderQueue_Model();
}

void GRAPHICENGINE::ClearRenderQueue_UI()
{
	g_RenderQueue->ResetRenderQueue_UI();
}

void GRAPHICENGINE::DeleteQueueListcertainindex(int index)
{
	g_RenderQueue->DeleteQueueList(index);
}

void GRAPHICENGINE::SetFogOnOff(bool b)
{
	g_RenderQueue->SetFogOnOff(b);
}

void GRAPHICENGINE::SetFogColor(Vector4 col)
{
	g_RenderQueue->SetFogColor(col);
}

void GRAPHICENGINE::SetFogDensity(float f)
{
	g_RenderQueue->SetFogDensity(f);
}

void GRAPHICENGINE::ISkyBoxInterface::ReleaseSkybox()
{

}
