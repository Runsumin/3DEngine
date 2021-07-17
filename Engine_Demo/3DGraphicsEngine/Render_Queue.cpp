#include "Render_Queue.h"
#include "ShaderMgr.h"
Render_Queue::Render_Queue() :
	m_TargetPosition(Vector3::Zero), m_ShadowRadious(100.f), m_ShadowOn(false),
	m_bCulling(false), m_DrawColl(0)
{
	m_pRtMgr = new RenderTargetMgr();
	m_pFog = new Fog();
	m_pRtMgr->InitRenderTargetMgr();
	m_pFog->InitFog();
	m_DrawColl = 0;
	m_bCulling = false;
	ZeroMemory(&m_ChannelView, sizeof(CHANNELVIEW));

}

Render_Queue::~Render_Queue()
{

}

void Render_Queue::GetSkyBoxPointer(GRAPHICENGINE::ISkyBoxInterface* obj)
{
	m_pSkybox = obj;
}

void Render_Queue::PushBackTerrainDrawList(GRAPHICENGINE::ITerrain_Interface* obj)
{
	m_pTerrain.push_back(obj);
}

void Render_Queue::PushBackModelDrawList(GRAPHICENGINE::IModelInterface* obj)
{
	m_pDrawModelList.push_back(obj);
}

void Render_Queue::PushBackUIDrawList(GRAPHICENGINE::IUiInterface* obj)
{
	m_pDrawUIList.push_back(obj);
}

void Render_Queue::DrawAll(float time)
{
	m_DrawColl = 0;
	m_pRtMgr->m_BloomOnOff = m_bBloom;

	// Create Using Data
	GetDataOneTime();

	//// LightView Matrix
	RenderTargetPass(time);

	m_pRtMgr->DrawRt_Debug();

	/// UI
	int uicount = m_pDrawUIList.size();
	for (int i = 0; i < uicount; i++)
	{
		m_pDrawUIList[i]->DrawUI();
	}

}

void Render_Queue::AllModelDraw(float time)
{
	// Draw SkyBox
	SkyBoxDraw();
	// Draw BackGroundData
	BackGroundDataDraw();

	/// Model
	int cnt = m_pDrawModelList.size();

	/// 2. NonAlpha 
	for (int i = 0; i < cnt; i++)
	{
		if (m_pDrawModelList[i]->GetDrawScreen() == true)
		{
			Vector3 pos = m_pDrawModelList[i]->GetWorldModelPosition();

			int dirlitcnt = m_DirectionLightList.size();
			for (int k = 0; k < dirlitcnt; k++)
			{
				m_pDrawModelList[i]->SetDirectionLightData(m_DirectionLightList[k]->GetDirectionLitData());
			}

			if (m_pDrawModelList[i]->GetModelSort() == GRAPHICENGINE::IModelInterface::ModelSort::STATIC_MESH)
			{

				if (ViewFrustumCulling(pos, m_CamViewProjMat, m_pDrawModelList[i]->GetCollSphereRadious()) == true)
				{
					int nodecnt = m_pDrawModelList[i]->GetModelNodeCount();
					for (int j = 0; j < nodecnt; j++)
					{
						GRAPHICENGINE::RenderQueueSort sort = m_pDrawModelList[i]->GetModelNodeRendersort(j);
						if (sort == GRAPHICENGINE::RenderQueueSort::SORT_TRANSPARENT)
						{
							m_pDrawModelList[i]->FbxModelDraw(time, j, GRAPHICENGINE::IModelInterface::RenderPath::Default, m_LightViewProjMat);
						}
					}

					m_DrawColl++;
				}
			}
			else
			{
				int nodecnt = m_pDrawModelList[i]->GetModelNodeCount();
				for (int j = 0; j < nodecnt; j++)
				{
					GRAPHICENGINE::RenderQueueSort sort = m_pDrawModelList[i]->GetModelNodeRendersort(j);
					if (sort == GRAPHICENGINE::RenderQueueSort::SORT_TRANSPARENT)
					{
						m_pDrawModelList[i]->FbxModelDraw(time, j, GRAPHICENGINE::IModelInterface::RenderPath::Default, m_LightViewProjMat);
					}
				}

				m_DrawColl++;
			}

		}
	}
	/// 1. Alpha
	for (int i = 0; i < cnt; i++)
	{
		if (m_pDrawModelList[i]->GetDrawScreen() == true)
		{
			Vector3 pos = m_pDrawModelList[i]->GetWorldModelPosition();

			int dirlitcnt = m_DirectionLightList.size();
			for (int k = 0; k < dirlitcnt; k++)
			{
				m_pDrawModelList[i]->SetDirectionLightData(m_DirectionLightList[k]->GetDirectionLitData());
			}

			if (m_pDrawModelList[i]->GetModelSort() == GRAPHICENGINE::IModelInterface::ModelSort::STATIC_MESH)
			{

				if (ViewFrustumCulling(pos, m_CamViewProjMat, m_pDrawModelList[i]->GetCollSphereRadious()) == true)
				{
					int nodecnt = m_pDrawModelList[i]->GetModelNodeCount();
					for (int j = 0; j < nodecnt; j++)
					{
						GRAPHICENGINE::RenderQueueSort sort = m_pDrawModelList[i]->GetModelNodeRendersort(j);
						if (sort == GRAPHICENGINE::RenderQueueSort::SORT_ALPHATEST)
						{
							m_pDrawModelList[i]->FbxModelDraw(time, j, GRAPHICENGINE::IModelInterface::RenderPath::Default, m_LightViewProjMat);
						}
					}
					m_DrawColl++;
				}
			}
			else
			{
				int nodecnt = m_pDrawModelList[i]->GetModelNodeCount();
				for (int j = 0; j < nodecnt; j++)
				{
					GRAPHICENGINE::RenderQueueSort sort = m_pDrawModelList[i]->GetModelNodeRendersort(j);
					if (sort == GRAPHICENGINE::RenderQueueSort::SORT_ALPHATEST)
					{
						m_pDrawModelList[i]->FbxModelDraw(time, j, GRAPHICENGINE::IModelInterface::RenderPath::Default, m_LightViewProjMat);
					}
				}
				m_DrawColl++;
			}

		}
	}
}

void Render_Queue::ShadowDraw(float time)
{
	/// Shadow
	if (m_ShadowOn == true)
	{
		Matrix mat;

		int cnt = m_pDrawModelList.size();
		/// 2. NonAlpha 
		for (int i = 0; i < cnt; i++)
		{
			if (m_pDrawModelList[i]->GetDrawScreen() == true)
			{
				int nodecnt = m_pDrawModelList[i]->GetModelNodeCount();
				for (int j = 0; j < nodecnt; j++)
				{
					GRAPHICENGINE::RenderQueueSort sort = m_pDrawModelList[i]->GetModelNodeRendersort(j);
					if (sort == GRAPHICENGINE::RenderQueueSort::SORT_TRANSPARENT)
					{
						m_pDrawModelList[i]->FbxModelDraw(time, j, GRAPHICENGINE::IModelInterface::RenderPath::Depth, m_LightViewProjMat);
					}
				}
			}
		}

	}
}

void Render_Queue::GetDataOneTime()
{
	// Light ViewProj Matrix
	m_LightViewProjMat = m_pRtMgr->GetLightVPMatrix(m_TargetPosition, m_ShadowRadious);

	// Camera ViewProjMatrix
	m_CamViewProjMat = XMMatrixMultiply(D3DDevice::GetInstance()->m_View, D3DDevice::GetInstance()->m_Proj);

	// Get Depth Buffer
	ShaderMgr::GetInstance()->m_pShadow = m_pRtMgr->GetDepthTex();
	// Get Light ViewProjMatrix
	ShaderMgr::GetInstance()->m_LightViewProjMat = m_LightViewProjMat;
}

void Render_Queue::SkyBoxDraw()
{
	if (m_pSkybox != NULL)
		m_pSkybox->DrawSkybox();
}

void Render_Queue::BackGroundDataDraw()
{
	/// BackGround
	int count = m_pTerrain.size();
	for (int i = 0; i < count; i++)
	{
		m_pTerrain[i]->Render(GRAPHICENGINE::ITerrain_Interface::RenderPath::Default, m_LightViewProjMat, m_ShadowOn);
	}
}

void Render_Queue::RenderTargetPass(float time)
{
	/// Result
	//m_pRtMgr->UpdateRT_Result();

	AllModelDraw(time);

	//m_pRtMgr->ReStoreRT_Result();

	///Bloom
	//m_pRtMgr->UpdateRT_Bloom();

	//AllModelDraw(time);

	//m_pRtMgr->ReStoreRT_Bloom();


	/// DepthBuffer
	m_pRtMgr->UpdateRenderTarget_Depth();

	ShadowDraw(time);

	m_pRtMgr->RestoreRenderTarget_Depth();

	///
}

void Render_Queue::ResetRenderQueue_Model()
{
	// 단순히 Queue를 비워주는 역할... 실체 객체는 해제해주어야 한다.
	m_pDrawModelList.clear();
}

void Render_Queue::ResetRenderQueue_UI()
{
	// 단순히 Queue를 비워주는 역할... 실체 객체는 해제해주어야 한다.
	m_pDrawUIList.clear();
}

void Render_Queue::DeleteQueueList(int index)
{
	for (int i = 0; i < m_pDrawModelList.size(); i++)
	{
		if (i == index)
			m_pDrawModelList.erase(m_pDrawModelList.begin() + index);
	}
}

void Render_Queue::SetCulling(bool b)
{
	m_bCulling = b;
}

bool Render_Queue::ViewFrustumCulling(Vector3 objworldpos, Matrix viewproj, float radious)
{
	bool result = true;

	if (m_bCulling == true)
	{
		Vector4 plane[6];

		// left
		plane[0].x = viewproj.m[0][3] + viewproj.m[0][0];
		plane[0].y = viewproj.m[1][3] + viewproj.m[1][0];
		plane[0].z = viewproj.m[2][3] + viewproj.m[2][0];
		plane[0].w = viewproj.m[3][3] + viewproj.m[3][0];
		// right
		plane[1].x = viewproj.m[0][3] - viewproj.m[0][0];
		plane[1].y = viewproj.m[1][3] - viewproj.m[1][0];
		plane[1].z = viewproj.m[2][3] - viewproj.m[2][0];
		plane[1].w = viewproj.m[3][3] - viewproj.m[3][0];
		// top
		plane[2].x = viewproj.m[0][3] - viewproj.m[0][1];
		plane[2].y = viewproj.m[1][3] - viewproj.m[1][1];
		plane[2].z = viewproj.m[2][3] - viewproj.m[2][1];
		plane[2].w = viewproj.m[3][3] - viewproj.m[3][1];
		// bottom
		plane[3].x = viewproj.m[0][3] + viewproj.m[0][1];
		plane[3].y = viewproj.m[1][3] + viewproj.m[1][1];
		plane[3].z = viewproj.m[2][3] + viewproj.m[2][1];
		plane[3].w = viewproj.m[3][3] + viewproj.m[3][1];
		// Back
		plane[4].x = viewproj.m[0][2];
		plane[4].y = viewproj.m[1][2];
		plane[4].z = viewproj.m[2][2];
		plane[4].w = viewproj.m[3][2];
		// Front
		plane[5].x = viewproj.m[0][3] - viewproj.m[0][2];
		plane[5].y = viewproj.m[1][3] - viewproj.m[1][2];
		plane[5].z = viewproj.m[2][3] - viewproj.m[2][2];
		plane[5].w = viewproj.m[3][3] - viewproj.m[3][2];

		// 방향만 필요하기 때문에 노멀라이즈
		//for (int i = 0; i < 6; i++)
		//{
		//	plane[i].Normalize();
		//	//XMPlaneNormalize(plane[i]);
		//}

		float dist;
		bool rad;
		for (int i = 0; i < 6; i++)
		{
			dist = CmpPlaneAndDot(plane[i], objworldpos);

			if (dist < -radious)
				result = false;
		}
	}
	return result;
}

float Render_Queue::CmpPlaneAndDot(Vector4 plane, Vector3 dot)
{
	return plane.x * dot.x + plane.y * dot.y + plane.z * dot.z + plane.w;
}

void Render_Queue::SetFogOnOff(bool b)
{
	m_pFog->SetFogOnOff(b);
	ShaderMgr::GetInstance()->m_Fog = m_pFog->GetFogData();
}

void Render_Queue::SetFogColor(Vector4 col)
{
	m_pFog->SetFogColor(col);
	ShaderMgr::GetInstance()->m_Fog = m_pFog->GetFogData();
}

void Render_Queue::SetFogDensity(float f)
{
	m_pFog->SetFogDensity(f);
	ShaderMgr::GetInstance()->m_Fog = m_pFog->GetFogData();
}
