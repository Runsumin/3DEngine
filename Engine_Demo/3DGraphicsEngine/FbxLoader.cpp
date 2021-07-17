#include "FbxLoader.h"
#include "MeshFilter.h"

FbxLoader::FbxLoader()
	: m_fbxMgr(nullptr), m_Scene(nullptr), m_Ios(nullptr), m_FbxImporter(nullptr)
{

}
FbxLoader::~FbxLoader()
{

}

bool FbxLoader::InitializeFbxLoader()
{
	// fbx �Ŵ��� ����
	m_fbxMgr = FbxManager::Create();

	// �� ���� ����
	m_Scene = FbxScene::Create(m_fbxMgr, "Scene");

	// I/O ���� ��ü ����
	m_Ios = FbxIOSettings::Create(m_fbxMgr, IOSROOT);
	m_fbxMgr->SetIOSettings(m_Ios);

	// Fbx ������ ����
	m_FbxImporter = FbxImporter::Create(m_fbxMgr, "");

	return true;
}

void FbxLoader::SceneSetting(FbxScene* Scene)
{
	// ��� ���ÿ� �� ���� �������ִ� �κ��� �ֱ�� �ѵ� �� �ʿ��� �۾������� �ǹ�
	// �� ������ ����� �ش�
	m_SceneInfo = FbxDocumentInfo::Create(m_fbxMgr, "Scene");
	m_SceneInfo->mTitle = "Test scene";
	m_SceneInfo->mSubject = "Test for fbx Export Scene.";
	m_SceneInfo->mAuthor = "Fbx_Expor_Test.";
	m_SceneInfo->mRevision = "rever. 0.1";
	m_SceneInfo->mKeywords = "Fbx_Test";
	m_SceneInfo->mComment = "no particular comments required.";

	Scene->SetSceneInfo(m_SceneInfo);

	// ��ǥ���� �����´�
	FbxAxisSystem sceneAxisSystem = Scene->GetGlobalSettings().GetAxisSystem();

	// �� ���� ��ǥ���� �ٲ۴� - �� �������ʿ� �ϴ��� ���ϸ� �׳� ����
	m_AxisSystem = Scene->GetGlobalSettings().GetAxisSystem();
	CalAxisMaxToDirectX(m_AxisSystem);

	//FbxAxisSystem::DirectX.ConvertScene(Scene);

	// �� ������ �ﰢ��ȭ �� �� �ִ� ��� ��带 �ﰢ��ȭ ��Ų��
	// �ƽ� �ȿ��� Editable poly ���¶�� �� �۾��� ���ؾ� �Ѵ�... �׷��� �ƽ��� ������ ��ġ�� ���´�.
	FbxGeometryConverter geometryConverter(m_fbxMgr);
	geometryConverter.Triangulate(Scene, true);

}

Fbx_MeshData* FbxLoader::LoadModel(string Filename)
{
	// �� �ʱ�ȭ
	Fbx_MeshData* pmodel = nullptr;

	// ���� ����Ʈ�� �ִ��� �˻�...
	std::map<std::string, Fbx_MeshData*>::iterator it;
	for (it = m_ImportDatalist.begin(); it != m_ImportDatalist.end(); it++)
	{
		string name = it->first;

		if (name == Filename)
		{
			pmodel = it->second;
			return pmodel;
		}
	}

	// ������ �ʱ�ȭ
	bool status = m_FbxImporter->Initialize(Filename.c_str(), -1, m_Ios);

	// ���� ��
	if (status == false)
	{
		cout << "Call LoadModel Faild" << endl;
		cout << "Error : " << m_FbxImporter->GetStatus().GetErrorString() << endl;

		return pmodel;
	}
	// ���� ��
	else
	{
		// fbx ���� ������ ������ �����´�
		m_FbxImporter->Import(m_Scene);
		SceneSetting(m_Scene);

		pmodel = new Fbx_MeshData();
		int posecnt = m_Scene->GetPoseCount();

		const TCHAR* pname = ConvertStringToTCHAR(Filename);

		TCHAR tempfname[256];
		GetFileName(pname, tempfname);		// ���� ���ϸ� �޾ƿ���

		pmodel->m_ModelName = ConvertTCharToString(tempfname);

		FbxPose* pose = m_Scene->GetPose(posecnt);
		FbxNode* rootnode = m_Scene->GetRootNode();

		pmodel->m_AxisChange = m_AxisChange;
		LoadSkeletone(rootnode, pmodel, nullptr, pose);
		pmodel->m_pAniDataList.push_back(LoadAnimationData(m_Scene, pmodel->m_pAllBoneList));
		LoadNode(rootnode, pmodel, pose);
		GetTexturePath(Filename, m_Scene, pmodel);

		// weight �ִ��� �Ǻ�..
		bool skinon = pmodel->m_SKinOn;
		switch (skinon)
		{
		case true:
			pmodel->m_MeshFilterSort = MeshFilter::eMeshfiltersort::SkinnedMesh;
			break;
		case false:
			pmodel->m_MeshFilterSort = MeshFilter::eMeshfiltersort::StaticMesh;
			break;
		}
		m_ImportDatalist.insert(std::pair<std::string, Fbx_MeshData*>(Filename, pmodel));

		return pmodel;
	}
}

Fbx_MeshData* FbxLoader::SetModel(string Filename)
{
	// �� �ʱ�ȭ
	Fbx_MeshData* pmodel = nullptr;

	// ���� ����Ʈ�� �ִ��� �˻�...
	std::map<std::string, Fbx_MeshData*>::iterator it;
	for (it = m_ImportDatalist.begin(); it != m_ImportDatalist.end(); it++)
	{
		string name = it->first;

		if (name == Filename)
		{
			pmodel = it->second;

			return pmodel;
		}
	}
}

void FbxLoader::SetTextrueDate()
{
	std::map<std::string, Fbx_MeshData*>::iterator it;
	for (it = m_ImportDatalist.begin(); it != m_ImportDatalist.end(); it++)
	{
		it->second->SetMaterialDataAuto();
	}
}

AnimationData* FbxLoader::LoadAnimationDataStack(string Filename)
{
	// ������ �ʱ�ȭ
	bool status = m_FbxImporter->Initialize(Filename.c_str(), -1, m_Ios);
	AnimationData* panidata = nullptr;
	// ���� ��
	if (status == false)
	{
		cout << "Call LoadModel Faild" << endl;
		cout << "Error : " << m_FbxImporter->GetStatus().GetErrorString() << endl;
	}
	// ���� ��
	else
	{
		m_FbxImporter->Import(m_Scene);
		int posecnt = m_Scene->GetPoseCount();

		FbxPose* pose = m_Scene->GetPose(posecnt);
		FbxNode* rootnode = m_Scene->GetRootNode();

		vector<Bone*> HirearchyData;
		GetHierarchydata(rootnode, HirearchyData, nullptr);
		panidata = new AnimationData();
		panidata = LoadAnimationData(m_Scene, HirearchyData);
	}

	return panidata;
}

void FbxLoader::GetHierarchydata(FbxNode* rootnode, vector<Bone*>& bonelist, Bone* parentbone)
{
	// ��������� ��带 Ž���Ѵ�...
	FbxNodeAttribute* nodeattr = rootnode->GetNodeAttribute();
	// ���ε��� ��� �Ӽ��� ������ NULL ��ȯ

	Bone* pbone = nullptr;
	if (nodeattr)
	{
		switch (nodeattr->GetAttributeType())
		{
			// �� ����. �������� ���� �ʿ�
		case FbxNodeAttribute::eSkeleton:
		{
			FbxNodeAttribute::EType nodeType = nodeattr->GetAttributeType();
			pbone = new Bone();
			GetSkeletonData(rootnode, pbone, nodeType, bonelist.size());
			pbone->pParentBone = parentbone;
			bonelist.push_back(pbone);
		}
		break;
		}
	}

	const int count = rootnode->GetChildCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		GetHierarchydata(rootnode->GetChild(i), bonelist, pbone);
	}
}


void FbxLoader::LoadNode(FbxNode* pNode, Fbx_MeshData* pmodel, FbxPose* pose)
{
	// ��������� ��带 Ž���Ѵ�...
	FbxNodeAttribute* nodeattr = pNode->GetNodeAttribute();
	// ���ε��� ��� �Ӽ��� ������ NULL ��ȯ
	FbxData_Mesh* pdata = nullptr;


	if (nodeattr != nullptr)
	{

		if (nodeattr)
		{
			FbxNodeAttribute::EType nodeType = nodeattr->GetAttributeType();

			if (nodeType != FbxNodeAttribute::eMesh)
				return;

			switch (nodeattr->GetAttributeType())
			{
				// �� ����� �Ӽ��� �Ž��̸�
			case FbxNodeAttribute::eMesh:
			{
				pdata = new FbxData_Mesh();

				// ���ؽ� ���� ���ϱ�
				GetVertex_List(pNode, pdata, pmodel);
				// ��� ���� ���ϱ�
				GetWorldMatrix(pNode, pdata, pose);
				// ������ ���� ���ϱ�
				pdata->m_PolygonCount = GetPolygonCnt(pNode);
				// ��� �̸� ���ϱ�
				pdata->m_NodeName = GetNodeName(pNode);
				// ���� ���� ���ϱ�
				GetMaterials(pNode, pdata, pmodel);
				// �ִϸ��̼� ���� ���ϱ�
				//GetAnimationMatrix(pNode, pmodel);

				// ������ ����...
				pmodel->m_pFbxDataList.push_back(pdata);

			}
			break;
			}
		}
	}

	const int count = pNode->GetChildCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		LoadNode(pNode->GetChild(i), pmodel, pose);
	}
}

void FbxLoader::LoadSkeletone(FbxNode* pNode, Fbx_MeshData* pmodel, Bone* parentbone, FbxPose* pose)
{
	// ��������� ��带 Ž���Ѵ�...
	FbxNodeAttribute* nodeattr = pNode->GetNodeAttribute();
	// ���ε��� ��� �Ӽ��� ������ NULL ��ȯ

	Bone* pbone = nullptr;
	if (nodeattr)
	{
		switch (nodeattr->GetAttributeType())
		{
			// �� ����. �������� ���� �ʿ�
		case FbxNodeAttribute::eSkeleton:
		{
			FbxNodeAttribute::EType nodeType = nodeattr->GetAttributeType();
			pbone = new Bone();
			GetSkeletonData(pNode, pbone, nodeType, pmodel->m_BoneCount);
			pmodel->m_BoneCount++;
			pbone->pParentBone = parentbone;
			pmodel->m_pAllBoneList.push_back(pbone);
			if (parentbone == nullptr)
			{
				pmodel->m_pBoneList_Hierarchy.push_back(pbone);
			}
			else
			{
				parentbone->pChildBoneList.push_back(pbone);
			}
		}
		break;
		}
	}

	const int count = pNode->GetChildCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		LoadSkeletone(pNode->GetChild(i), pmodel, pbone, pose);
	}
}

void FbxLoader::GetSkeletonData(FbxNode* pnode, Bone* pBonedata, FbxNodeAttribute::EType type, int BoneIndex)
{
	pBonedata->BoneIndex = BoneIndex;
	pBonedata->m_Name = pnode->GetName();
	pBonedata->m_Parentname = pnode->GetParent()->GetName();
	pBonedata->LocalTm = ConvertFbxMattoDirxMat(pnode->GetScene()->GetAnimationEvaluator()->GetNodeLocalTransform(pnode));
	pBonedata->GlobalTM = ConvertFbxMattoDirxMat(pnode->GetScene()->GetAnimationEvaluator()->GetNodeGlobalTransform(pnode));
	pBonedata->InvGlobalTm = ConvertFbxMattoDirxMat(pnode->GetScene()->GetAnimationEvaluator()->GetNodeGlobalTransform(pnode).Inverse());
}


void FbxLoader::GetTexturePath(string filename, FbxScene* scene, Fbx_MeshData* pmodel)
{
	// ���ϸ��� ���ҽ��� �� ���� �ȿ� �ִٴ� ���� �Ͽ�.....
	// �ѹ��� ������ �Ǽ� 1���� �����Ѵ�.
	FbxVideo* vid = scene->GetVideo(0);
	FbxSurfaceMaterial* pmat = scene->GetMaterial(0);
	if (pmat != NULL && vid != NULL)
	{
		FbxString name = vid->GetFileName();		// fbx �ȿ� ����� ���� ���

		string tempname;							//	
		tempname = name;

		const TCHAR* pname = ConvertStringToTCHAR(tempname);

		TCHAR tempfname[256];
		GetFileName(pname, tempfname);				// ���� ���ϸ� �޾ƿ���

		const TCHAR* filepath = ConvertStringToTCHAR(filename);

		TCHAR texfilename[256] = L"";				// �ؽ��� �ε��� ���
		GetPath(filepath, texfilename);

		string finalpath = ConvertTCharToString(texfilename);

		pmodel->m_TexPath = finalpath;

	}
}

void FbxLoader::CalAxisMaxToDirectX(FbxAxisSystem maxAxisSystem)
{
	FbxVector4	vScale(1.0, 1.0, -1.0, 1);
	FbxVector4	vRotation(90, 0.0, 0.0, 1);
	FbxVector4	vTranslation(0.0, 0.0, 0.0, 1);

	FbxAMatrix rot(FbxVector4(0.0, 0.0, 0.0), vRotation, FbxVector4(1.0, 1.0, 1.0));
	FbxAMatrix scale(FbxVector4(0.0, 0.0, 0.0), FbxVector4(0.0, 0.0, 0.0), vScale);

	FbxAMatrix rootTM;

	int direction = 1;

	//3d max���� export �ɼ� �� ��ǥ�� ������ �ִ�. yUp �Ǵ� zUp�� ������ �� �ִ�.
	if (maxAxisSystem.GetUpVector(direction) == FbxAxisSystem::eYAxis)
	{
		rootTM = scale;
	}
	else if (maxAxisSystem.GetUpVector(direction) == FbxAxisSystem::eZAxis)
	{
		rootTM = rot * scale;
	}

	m_AxisChange = ConvertFbxMattoDirxMat(rootTM);
}

AnimationData* FbxLoader::LoadAnimationData(FbxScene* pscene, vector<Bone*> HirearchyData)
{
	float framesPerSecond = (float)FbxTime::GetFrameRate(pscene->GetGlobalSettings().GetTimeMode());

	// fbx ���� ���� �ִϸ��̼� ����
	int numStacks = pscene->GetSrcObjectCount<FbxAnimStack>();

	AnimationData* pAnidata = nullptr;
	for (int i = 0; i < numStacks; i++)
	{
		// �ִϸ��̼� ���� Ŭ����
		FbxAnimStack* pAnimStack = FbxCast<FbxAnimStack>(pscene->GetSrcObject<FbxAnimStack>(i));

		string name = pAnimStack->GetName();
		FbxTimeSpan timeSpan = pAnimStack->GetLocalTimeSpan();
		double startTime = timeSpan.GetStart().GetSecondDouble();
		double endTime = timeSpan.GetStop().GetSecondDouble();

		pAnidata = new AnimationData();

		pAnidata->FramesPerSecond = framesPerSecond;
		pAnidata->TotalFrames = (int)((endTime - startTime) * (double)framesPerSecond);
		pAnidata->StartTime = startTime;
		pAnidata->EndTime = endTime;
		pAnidata->TotalTime = endTime - startTime;
		pAnidata->Name = name;
		pAnidata->AnimationList.resize(HirearchyData.size());
		// �ִϸ��̼� Ŀ�� ��� ������ Ŭ����
		FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>();
		GetAnimationData_Recur(pscene->GetRootNode(), pAnidata, HirearchyData);
		//pmodel->m_pAniDataList.push_back(pAnidata);
	}

	return pAnidata;
}

void FbxLoader::GetAnimationData_Recur(FbxNode* prootnode, AnimationData* panidata, vector<Bone*> HirearchyData)
{
	// ��������� ��带 Ž���Ѵ�...
	FbxNodeAttribute* nodeattr = prootnode->GetNodeAttribute();

	if (nodeattr != nullptr)
	{

		if (nodeattr)
		{
			FbxNodeAttribute::EType nodeType = nodeattr->GetAttributeType();

			if (nodeType != FbxNodeAttribute::eMesh)
				return;

			switch (nodeattr->GetAttributeType())
			{
				// �� ����� �Ӽ��� �Ž��̸�
			case FbxNodeAttribute::eMesh:
			{
				GetAnimationMatrix(prootnode, panidata, HirearchyData);
			}
			break;
			}
		}
	}

	const int count = prootnode->GetChildCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		GetAnimationData_Recur(prootnode->GetChild(i), panidata, HirearchyData);
	}
}

void FbxLoader::GetAnimationMatrix(FbxNode* pNode, AnimationData* panidata, vector<Bone*> HirearchyData)
{
	FbxMesh* currMesh = pNode->GetMesh();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();

	FbxProperty pProperty = pNode->LclTranslation;

	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

		if (!currSkin) { continue; }

		unsigned int numOfClusters = currSkin->GetClusterCount();

		// �޽��� ���ε��� �� ���� ��ŭ �ִϸ��̼� �߰�...
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			string currJointName = currCluster->GetLink()->GetName();

			int bonecnt = HirearchyData.size();
			FbxAnimEvaluator* pSceneEvaluator = m_Scene->GetAnimationEvaluator();

			int Boneindex = FindBoneIndex(currJointName, HirearchyData);

			OneFrameList* onefrmdata = new OneFrameList;

			FbxLongLong index;
			int frmcnt = panidata->TotalFrames;
			float time = panidata->TotalTime;
			int FramePerSecond = panidata->FramesPerSecond;

			// 20210511 �߰� - start time�� ���� �ƴҶ� ����ó��
			float onetick = time / frmcnt;
			float starttime = panidata->StartTime;;
			int startindex = starttime / onetick;


			for (index = 0; index < frmcnt; ++index)
			{
				FbxTime currTime;
				switch (FramePerSecond)
				{
				default:
					currTime.SetFrame(startindex + index, FbxTime::eFrames30);
					break;
				case 60:
					currTime.SetFrame(startindex + index, FbxTime::eFrames60);
					break;
				case 100:
					currTime.SetFrame(startindex + index, FbxTime::eFrames100);
					break;
				}

				KeyFrameData currKey;
				currKey.Time = time / float(frmcnt) * index;

				FbxAMatrix currentTransformOffset = pSceneEvaluator->GetNodeGlobalTransform(pNode, currTime);
				FbxAMatrix temp = currentTransformOffset.Inverse() * pSceneEvaluator->GetNodeGlobalTransform(currCluster->GetLink(), currTime);

				Matrix TM = ConvertFbxMattoDirxMat(temp);

				XMVECTOR vScale;
				XMVECTOR vQuatRot;
				XMVECTOR vTrans;

				XMMatrixDecompose(&vScale, &vQuatRot, &vTrans, TM);

				XMStoreFloat3(&currKey.Scale, vScale);
				XMStoreFloat4(&currKey.Rot_Quat, vQuatRot);
				XMStoreFloat3(&currKey.Trans, vTrans);
				onefrmdata->FrmList.push_back(currKey);
			}

			panidata->AnimationList[Boneindex] = onefrmdata;
		}
	}
}

void FbxLoader::GetVertex_List(FbxNode* pNode, FbxData_Mesh* pdata, Fbx_MeshData* pmodel)
{
	FbxMesh* mesh = pNode->GetMesh();

	unsigned int count = mesh->GetControlPointsCount();

	//Vertex* vertex = new Vertex[count];
	vector<Vertex> vertex;
	vertex.resize(count);
	for (unsigned int i = 0; i < count; i++)
	{
		Vertex pos;
		pos.Pos.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);	// x
		pos.Pos.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);	// y
		pos.Pos.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);	// z

		vertex[i].Pos = pos.Pos;

		// ���� ū ��...
		if (pos.Pos.x > pmodel->m_BiggstX)
		{
			pmodel->m_BiggstX = pos.Pos.x;
		}
		if (pos.Pos.y > pmodel->m_BiggstY)
		{
			pmodel->m_BiggstY = pos.Pos.y;
		}
		if (pos.Pos.z > pmodel->m_BiggstZ)
		{
			pmodel->m_BiggstZ = pos.Pos.z;
		}
		// ���� ���� ��..
		if (pos.Pos.x < pmodel->m_leastX)
		{
			pmodel->m_leastX = pos.Pos.x;
		}
		if (pos.Pos.y < pmodel->m_leastY)
		{
			pmodel->m_leastY = pos.Pos.y;
		}
		if (pos.Pos.z < pmodel->m_leastZ)
		{
			pmodel->m_leastZ = pos.Pos.z;
		}

	}

	/// �ε��� ����
	int cnt = mesh->GetPolygonCount();

	vector<Vertex> verlist;
	vector<Index> indlist;
	verlist.resize(cnt * 3);
	indlist.resize(cnt * 3);

	int vertcnt = 0;
	int num = 0;

	/// Bone index & weight
	vector<BoneIndexWeight> dummybonelist(count);
	LoadBoneIndexWeight(pNode, dummybonelist, pdata, pmodel);
	SortWeightData(dummybonelist);

	for (int i = 0; i < cnt; i++)
	{
		Vertex tempVertex;
		Index tempIndex;

		for (int j = 0; j < 3; j++, vertcnt++)
		{
			int indexcnt = mesh->GetPolygonVertex(i, j);

			switch (j)
			{
			case 0:
				indlist[i].A = j + num;
				break;
			case 1:
				indlist[i].C = j + num;
				break;
			case 2:
				indlist[i].B = j + num;
				break;
			}
			// vertex
			tempVertex.Pos = vertex[indexcnt].Pos;
			// uv
			tempVertex.Uv = GetUV(mesh, i, j);
			// normal
			tempVertex.Nor = GetNormal(mesh, i, j);
			// bone index , weight
			if (dummybonelist.size() > 0)
			{
				BoneIndexWeight currBoneIndexWeight = dummybonelist[indexcnt];

				for (int i = 0; i < currBoneIndexWeight.boneIndexWeights.size(); i++)
				{
					tempVertex.index[i] = currBoneIndexWeight.boneIndexWeights[i].first;
					switch (i)
					{
					case 0:
						tempVertex.Weight.x = currBoneIndexWeight.boneIndexWeights[i].second;
						break;
					case 1:
						tempVertex.Weight.y = currBoneIndexWeight.boneIndexWeights[i].second;
						break;
					case 2:
						tempVertex.Weight.z = currBoneIndexWeight.boneIndexWeights[i].second;
						break;
					case 3:
						tempVertex.Weight.w = currBoneIndexWeight.boneIndexWeights[i].second;
						break;
					default:
						break;
					}
				}
			}

			verlist[cnt * 3 - 1 - vertcnt] = tempVertex;
		}
		num += 3;
	}

	pdata->m_pVertexList = verlist;
	pdata->m_pIndexList = indlist;
	pdata->m_IndexCount = cnt;
	pdata->m_VertexCount = vertcnt;
}

void FbxLoader::LoadBoneIndexWeight(FbxNode* pnode, vector<BoneIndexWeight>& dummybonelist, FbxData_Mesh* pdata, Fbx_MeshData* pmodel)
{
	FbxMesh* currMesh = pnode->GetMesh();
	string name = pnode->GetName();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();

	int num = 0;

	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

		if (!currSkin)
		{
			pmodel->m_SKinOn = false;
			continue;
		}
		pmodel->m_SKinOn = true;
		unsigned int numOfClusters = currSkin->GetClusterCount();

		pdata->m_BoneOffSet.resize(pmodel->m_pAllBoneList.size());
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			string currJointName = currCluster->GetLink()->GetName();

			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;
			FbxAMatrix geometryTransform = GetGeometryTransformation(pnode);


			currCluster->GetTransformMatrix(transformMatrix);
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			int Boneindex = FindBoneIndex(currJointName, pmodel->m_pAllBoneList);

			pdata->m_BoneOffSet[Boneindex] = ConvertFbxMattoDirxMat(globalBindposeInverseMatrix);
			pmodel->m_pAllBoneList[Boneindex]->GlobalBindingTm = ConvertFbxMattoDirxMat(globalBindposeInverseMatrix);

			/// �̺κ� �� �ʿ� ����...
			pdata->m_GlobalBindPos = ConvertFbxMattoDirxMat(transformMatrix);
			pdata->m_isBinding = true;

			int* indices = currCluster->GetControlPointIndices();
			double* weights = currCluster->GetControlPointWeights();

			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();
			for (int i = 0; i < numOfIndices; i++)
			{
				std::pair<unsigned int, float> currIndexWeight(Boneindex, weights[i]);
				dummybonelist[indices[i]].boneIndexWeights.push_back(currIndexWeight);
			}
		}
	}
}
int FbxLoader::FindBoneIndex(string name, vector<Bone*> HirearchyData)
{
	int boneIndex = -1;
	int cnt = HirearchyData.size();
	for (int i = 0; i < cnt; i++)
	{
		if (name == HirearchyData[i]->m_Name)
		{
			boneIndex = HirearchyData[i]->BoneIndex;
			break;
		}
	}
	return boneIndex;

}

Vector2 FbxLoader::GetUV(FbxMesh* pmesh, int i, int j)
{
	///  UV
	float* lUVs = NULL;
	FbxStringList lUVNames;
	pmesh->GetUVSetNames(lUVNames);
	const char* lUVName = NULL;
	if (lUVNames.GetCount())
	{
		lUVName = lUVNames[0];
	}

	FbxVector2 pUVs;
	bool bUnMappedUV;
	pmesh->GetPolygonVertexUV(i, j, lUVName, pUVs, bUnMappedUV);

	Vector2 uv;
	uv.x = static_cast<float>(pUVs.mData[0]);
	uv.y = static_cast<float>(1.0f - pUVs.mData[1]);

	return uv;
}

Vector3 FbxLoader::GetNormal(FbxMesh* pmesh, int i, int j)
{
	/// Normal
	FbxVector4 normal;
	pmesh->GetPolygonVertexNormal(i, j, normal);

	Vector3 nor;

	nor.x = static_cast<float>(normal.mData[0]);
	nor.y = static_cast<float>(normal.mData[1]);
	nor.z = static_cast<float>(normal.mData[2]);

	return nor;
}

Vector3 FbxLoader::GetNormaldata(FbxMesh* pmesh, int indexcount, int vertexcount)
{
	Vector3 result;

	if (pmesh->GetElementNormalCount() < 1)
	{
		cout << "Binormal data not existence" << endl;
		return result;
	}

	FbxGeometryElementNormal* vertexNormal = pmesh->GetElementNormal(0);

	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(indexcount).mData[0]);
			result.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(indexcount).mData[1]);
			result.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(indexcount).mData[2]);

			return result;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(indexcount);

			result.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
			return result;
		}
		break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(indexcount).mData[0]);
			result.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(indexcount).mData[1]);
			result.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(indexcount).mData[2]);

			return result;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(indexcount);
			result.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
			return result;
		}
		break;
		}
	}
	break;
	}

	return result;
}

Vector3 FbxLoader::GetBiNormal(FbxMesh* pmesh, int indexcount, int vertexcount)
{
	Vector3 result;

	if (pmesh->GetElementBinormalCount() < 1)
	{
		cout << "Binormal data not existence" << endl;
		return result;
	}

	FbxGeometryElementBinormal* vertexBiNormal = pmesh->GetElementBinormal(0);

	switch (vertexBiNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (vertexBiNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(indexcount).mData[0]);
			result.y = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(indexcount).mData[1]);
			result.z = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(indexcount).mData[2]);

			return result;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexBiNormal->GetIndexArray().GetAt(indexcount);

			result.x = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(index).mData[2]);
			return result;
		}
		break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexBiNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(indexcount).mData[0]);
			result.y = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(indexcount).mData[1]);
			result.z = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(indexcount).mData[2]);

			return result;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexBiNormal->GetIndexArray().GetAt(indexcount);
			result.x = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(index).mData[2]);
			return result;
		}
		break;
		}
	}
	break;
	}

	return result;
}

Vector3 FbxLoader::GetTangent(FbxMesh* pmesh, int indexcount, int vertexcount)
{
	Vector3 result;

	int tangentcount = pmesh->GetElementTangentCount();
	if (tangentcount < 1)
	{
		cout << "Tangent data not existence" << endl;
		return result;
	}

	FbxGeometryElementTangent* vertextangent = pmesh->GetElementTangent(0);

	switch (vertextangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (vertextangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertextangent->GetDirectArray().GetAt(indexcount).mData[0]);
			result.y = static_cast<float>(vertextangent->GetDirectArray().GetAt(indexcount).mData[1]);
			result.z = static_cast<float>(vertextangent->GetDirectArray().GetAt(indexcount).mData[2]);

			return result;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertextangent->GetIndexArray().GetAt(indexcount);

			result.x = static_cast<float>(vertextangent->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertextangent->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(vertextangent->GetDirectArray().GetAt(index).mData[2]);
			return result;
		}
		break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertextangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertextangent->GetDirectArray().GetAt(indexcount).mData[0]);
			result.y = static_cast<float>(vertextangent->GetDirectArray().GetAt(indexcount).mData[1]);
			result.z = static_cast<float>(vertextangent->GetDirectArray().GetAt(indexcount).mData[2]);

			return result;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertextangent->GetIndexArray().GetAt(indexcount);
			result.x = static_cast<float>(vertextangent->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertextangent->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(vertextangent->GetDirectArray().GetAt(index).mData[2]);
			return result;
		}
		break;
		}
	}
	break;
	}

	return result;
}

void FbxLoader::GetWorldMatrix(FbxNode* pnode, FbxData_Mesh* pdata, FbxPose* pose)
{
	// �ƽ��� ���� ������.
	FbxMatrix globalpos;
	globalpos = pnode->GetScene()->GetAnimationEvaluator()->GetNodeGlobalTransform(pnode);

	FbxAMatrix geometryTransform = GetGeometryTransformation(pnode);


	pdata->m_Worldm = ConvertFbxMattoDirxMat(geometryTransform) * ConvertFbxMattoDirxMat(globalpos);
}

void FbxLoader::GetMaterials(FbxNode* pnode, FbxData_Mesh* pdata, Fbx_MeshData* pmodel)
{
	int matcount = pnode->GetMaterialCount();

	for (int i = 0; i < matcount; i++)
	{
		Fbx_Material* temp = new Fbx_Material();
		FbxSurfaceMaterial* SurfaceMaterial = pnode->GetMaterial(i);
		GetMaterialAttribute(SurfaceMaterial, temp);					// ���� ���� �ε�
		GetMaterialTexture(SurfaceMaterial, pdata, pmodel);

		pdata->m_pMaterial = temp;
	}
}

void FbxLoader::GetMaterialAttribute(FbxSurfaceMaterial* pmat, Fbx_Material* matdata)
{
	FbxDouble3 double3;

	if (pmat->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		// Diffuse Color
		double3 = reinterpret_cast<FbxSurfacePhong*>(pmat)->Diffuse;
		matdata->Diffuse.x = static_cast<float>(double3.mData[0]);
		matdata->Diffuse.y = static_cast<float>(double3.mData[1]);
		matdata->Diffuse.z = static_cast<float>(double3.mData[2]);

		// Amibent Color
		double3 = reinterpret_cast<FbxSurfacePhong*>(pmat)->Ambient;
		matdata->Ambient.x = static_cast<float>(double3.mData[0]);
		matdata->Ambient.y = static_cast<float>(double3.mData[1]);
		matdata->Ambient.z = static_cast<float>(double3.mData[2]);

		// Specular Color
		double3 = reinterpret_cast<FbxSurfacePhong*>(pmat)->Specular;
		matdata->Specular.x = static_cast<float>(double3.mData[0]);
		matdata->Specular.y = static_cast<float>(double3.mData[1]);
		matdata->Specular.z = static_cast<float>(double3.mData[2]);
	}
	else if (pmat->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		// Diffuse Color
		double3 = reinterpret_cast<FbxSurfaceLambert*>(pmat)->Diffuse;
		matdata->Diffuse.x = static_cast<float>(double3.mData[0]);
		matdata->Diffuse.y = static_cast<float>(double3.mData[1]);
		matdata->Diffuse.z = static_cast<float>(double3.mData[2]);

		// Amibent Color
		double3 = reinterpret_cast<FbxSurfaceLambert*>(pmat)->Ambient;
		matdata->Ambient.x = static_cast<float>(double3.mData[0]);
		matdata->Ambient.y = static_cast<float>(double3.mData[1]);
		matdata->Ambient.z = static_cast<float>(double3.mData[2]);
	}
}

void FbxLoader::GetMaterialTexture(FbxSurfaceMaterial* pmat, FbxData_Mesh* pdata, Fbx_MeshData* pmodel)
{
	unsigned int textureindex = 0;
	FbxProperty property;

	FBXSDK_FOR_EACH_TEXTURE(textureindex)
	{
		property = pmat->FindProperty(FbxLayerElement::sTextureChannelNames[textureindex]);

		if (property.IsValid())
		{
			unsigned int textureCount = property.GetSrcObjectCount<FbxTexture>();
			for (unsigned int i = 0; i < textureCount; i++)
			{
				FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(i);

				if (layeredTexture)
				{
					cout << "���� ���̾ ���Ե� �ؽ��İ� �����ϴ�." << endl;
				}
				else
				{
					FbxTexture* texture = property.GetSrcObject<FbxTexture>(i);
					if (texture)
					{
						string textureType = property.GetNameAsCStr();
						FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);

						if (fileTexture)
						{
							// ���⼭ �������� ���� ������ �����´�
							if (textureType == "DiffuseColor")
							{
								string name = fileTexture->GetFileName();

								const TCHAR* pname = ConvertStringToTCHAR(name);

								TCHAR tempfname[256];
								GetFileName(pname, tempfname);				// ���� ���ϸ� �޾ƿ���

								string name2 = ConvertTCharToString(tempfname); // �ҷ��� ���� �̸�

								// �ߺ� �ؽ��� �˻�
								bool bcompare = false;
								int count = pmodel->m_Texfilename.size();
								for (int i = 0; i < count; i++)
								{
									// ���� �ߺ��� ������
									if (pmodel->m_Texfilename[i] == name2)
									{
										pdata->m_TextureCount = i;	//
										pdata->m_TextureOn = true;
										bcompare = true;
										break;
									}
								}
								if (bcompare == true)
								{
									// // // // // 
								}
								else
								{
									// �ؽ��� ī��Ʈ �߰�
									pdata->m_TextureCount = pmodel->m_TexCount;	//
									pmodel->m_TexCount += 1;
									pdata->m_TextureOn = true;
									pmodel->m_Texfilename.push_back(name2);
									pmodel->m_LinkNodeNameList.push_back(pdata->m_NodeName);
								}
							}
							else if (textureType == "Bump")
							{
								string name = fileTexture->GetFileName();

								const TCHAR* pname = ConvertStringToTCHAR(name);

								TCHAR tempfname[256];
								GetFileName(pname, tempfname);				// ���� ���ϸ� �޾ƿ���

								string name2 = ConvertTCharToString(tempfname); // �ҷ��� ���� �̸�

								// �ߺ� �ؽ��� �˻�
								bool bcompare = false;
								int count = pmodel->m_Norfilename.size();
								for (int i = 0; i < count; i++)
								{
									// ���� �ߺ��� ������
									if (pmodel->m_Norfilename[i] == name2)
									{
										pdata->m_NormalCount = i;	//
										pdata->m_NormalMapOn = true;
										bcompare = true;
										break;
									}
								}
								if (bcompare == true)
								{
									// // // // // 
								}
								else
								{
									// �ؽ��� ī��Ʈ �߰�
									pdata->m_NormalCount = pmodel->m_NorCount;	//
									pdata->m_NormalMapOn = true;
									pmodel->m_NorCount++;
									pmodel->m_Norfilename.push_back(name2);
								}
							
							}
							else if (textureType == "TransparentColor")	// MaskMap
							{
								string name = fileTexture->GetFileName();

								const TCHAR* pname = ConvertStringToTCHAR(name);

								TCHAR tempfname[256];
								GetFileName(pname, tempfname);				// ���� ���ϸ� �޾ƿ���

								string name2 = ConvertTCharToString(tempfname); // �ҷ��� ���� �̸�

								// �ؽ��� ī��Ʈ �߰�
								pdata->m_MaskMapCount = pmodel->m_MaskCount;	//
								pdata->m_MaskMapOn = true;
								pmodel->m_MaskCount++;
								pmodel->m_MaskMapfilename.push_back(name2);
							}
							else if (textureType == "SpecularColor")
							{
								string name = fileTexture->GetFileName();

								const TCHAR* pname = ConvertStringToTCHAR(name);

								TCHAR tempfname[256];
								GetFileName(pname, tempfname);				// ���� ���ϸ� �޾ƿ���

								string name2 = ConvertTCharToString(tempfname); // �ҷ��� ���� �̸�

								// �ߺ� �ؽ��� �˻�
								bool bcompare = false;
								int count = pmodel->m_ORMfilename.size();
								for (int i = 0; i < count; i++)
								{
									// ���� �ߺ��� ������
									if (pmodel->m_ORMfilename[i] == name2)
									{
										pdata->m_ORMapCount = i;	//
										pdata->m_ORMMapOn = true;
										bcompare = true;
										break;
									}
								}
								if (bcompare == true)
								{
									// // // // // 
								}
								else
								{
									// �ؽ��� ī��Ʈ �߰�
									pdata->m_ORMapCount = pmodel->m_ORMCount;	//
									pdata->m_ORMMapOn = true;
									pmodel->m_ORMCount++;
									pmodel->m_ORMfilename.push_back(name2);
								}
							}
						}
						else
						{
							// �ؽ��İ� ������ 100���� �ӽ�...
							pdata->m_TextureCount = 100;
							pdata->m_NormalCount = 100;
							pdata->m_MaskMapCount = 100;
							pdata->m_ORMapCount = 100;
							pdata->m_TextureOn = false;
							pdata->m_NormalMapOn = false;
							pdata->m_MaskMapOn = false;
							pdata->m_ORMMapOn = false;
						}
					}
				}
			}

		}
	}
}

Matrix FbxLoader::ConvertFbxMattoDirxMat(FbxMatrix fbxmat)
{
	Matrix mat;

	mat.m[0][0] = static_cast<float>(fbxmat.mData[0].mData[0]);
	mat.m[0][1] = static_cast<float>(fbxmat.mData[0].mData[1]);
	mat.m[0][2] = static_cast<float>(fbxmat.mData[0].mData[2]);
	mat.m[0][3] = static_cast<float>(fbxmat.mData[0].mData[3]);

	mat.m[1][0] = static_cast<float>(fbxmat.mData[1].mData[0]);
	mat.m[1][1] = static_cast<float>(fbxmat.mData[1].mData[1]);
	mat.m[1][2] = static_cast<float>(fbxmat.mData[1].mData[2]);
	mat.m[1][3] = static_cast<float>(fbxmat.mData[1].mData[3]);

	mat.m[2][0] = static_cast<float>(fbxmat.mData[2].mData[0]);
	mat.m[2][1] = static_cast<float>(fbxmat.mData[2].mData[1]);
	mat.m[2][2] = static_cast<float>(fbxmat.mData[2].mData[2]);
	mat.m[2][3] = static_cast<float>(fbxmat.mData[2].mData[3]);

	mat.m[3][0] = static_cast<float>(fbxmat.mData[3].mData[0]);
	mat.m[3][1] = static_cast<float>(fbxmat.mData[3].mData[1]);
	mat.m[3][2] = static_cast<float>(fbxmat.mData[3].mData[2]);
	mat.m[3][3] = static_cast<float>(fbxmat.mData[3].mData[3]);

	return mat;
}

string FbxLoader::GetNodeName(FbxNode* pNode)
{
	string name = pNode->GetName();

	return name;
}

int FbxLoader::GetPolygonCnt(FbxNode* pNode)
{
	FbxMesh* mesh = pNode->GetMesh();
	int cnt = mesh->GetPolygonCount();

	return cnt;
}

void FbxLoader::SetModelInfo()
{
	/// ���� �߰� ����
}

void FbxLoader::GetFileName(const TCHAR* FullPath, TCHAR* FileName)
{
	std::wstring name;		// ���ϸ� ����� �ӽú���
	name = FullPath;
	size_t pos = name.find_last_of('\\');
	if (pos <= 0)
	{
		pos = name.find_last_of('/');
	}
	_tcscpy(FileName, &name[pos + 1]);
}

void FbxLoader::GetPath(const TCHAR* FullPathName, TCHAR* Path)
{
	std::wstring path;		// ��� ����� �ӽ� ����
	path = FullPathName;
	size_t pos = path.find_last_of('\\');
	if (pos <= 0)
	{
		pos = path.find_last_of('/');
	}
	_tcsncpy(Path, FullPathName, pos + 1);
}

TCHAR* FbxLoader::ConvertStringToTCHAR(string& s)
{
	string tstr;
	const char* all = s.c_str();
	int len = 1 + strlen(all);
	wchar_t* t = new wchar_t[len];
	if (NULL == t) throw std::bad_alloc();
	mbstowcs(t, all, len);
	return (TCHAR*)t;

}

std::string FbxLoader::ConvertTCharToString(const TCHAR* ptsz)
{
	int len = wcslen((wchar_t*)ptsz);
	char* psz = new char[2 * len + 1];
	wcstombs(psz, (wchar_t*)ptsz, 2 * len + 1);
	std::string s = psz;
	delete[] psz;
	return s;

}

FbxAMatrix FbxLoader::GetGeometryTransformation(FbxNode* pnode)
{
	if (!pnode)
	{
		throw std::exception("Null for mesh geometry");
	}

	const FbxVector4 lT = pnode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pnode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pnode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

Vector2 FbxLoader::ConvertVec2(FbxVector2 vec2)
{
	Vector2 ReVec;
	ReVec.x = static_cast<float>(vec2.mData[0]);
	ReVec.y = static_cast<float>(vec2.mData[1]);

	return ReVec;
}

Vector3 FbxLoader::ConvertVec3(FbxVector4 vec3)
{
	Vector3 ReVec;
	ReVec.x = static_cast<float>(vec3.mData[0]);
	ReVec.y = static_cast<float>(vec3.mData[1]);
	ReVec.z = static_cast<float>(vec3.mData[2]);

	return ReVec;

}

Vector4 FbxLoader::ConvertVec4(FbxVector4 vec4)
{
	Vector4 ReVec;
	ReVec.x = static_cast<float>(vec4.mData[0]);
	ReVec.y = static_cast<float>(vec4.mData[1]);
	ReVec.z = static_cast<float>(vec4.mData[2]);
	ReVec.w = static_cast<float>(vec4.mData[3]);

	return ReVec;

}

void FbxLoader::SortWeightData(vector<BoneIndexWeight>& bonelist)
{
	int cnt = bonelist.size();

	for (int i = 0; i < cnt; i++)
	{
		int weightcount = bonelist[i].boneIndexWeights.size();
		// ����Ʈ ���� 4�� �ʰ��� ��
		if (weightcount > 4)
		{
			for (int j = 0; j < weightcount - 1; j++)
			{
				for (int k = 1; k < weightcount - j; k++)
				{
					if (bonelist[i].boneIndexWeights[k - 1].second < bonelist[i].boneIndexWeights[k].second)
						std::swap(bonelist[i].boneIndexWeights[k - 1], bonelist[i].boneIndexWeights[k]);
				}
			}

			bonelist[i].boneIndexWeights[3].second =
				1.0f - bonelist[i].boneIndexWeights[0].second
				- bonelist[i].boneIndexWeights[1].second
				- bonelist[i].boneIndexWeights[2].second;

		}
		if (weightcount < 4)
		{
			int zeroweightcnt = 4 - bonelist[i].boneIndexWeights.size();
			for (int j = 0; j < zeroweightcnt; j++)
			{
				bonelist[i].boneIndexWeights.push_back(std::make_pair<unsigned int, float>(0, 0));
			}
		}
	}
}

void FbxLoader::ReleaseFbxLoader()
{
	m_Ios->Destroy();
	m_Scene->Destroy();
	m_FbxImporter->Destroy();
	m_fbxMgr->Destroy();

	m_ImportDatalist.clear();

	/// �� ���� ����°� �� �������� �𸣰���
}
