#include "FbxDataExporter.h"
#include <fstream>
#include <iostream>
FbxDataExporter::FbxDataExporter()
{

}

FbxDataExporter::~FbxDataExporter()
{

}
void FbxDataExporter::ExportFbxData(string filename, Fbx_MeshData* inputdata)
{
	/// Flatbuffer builder Generate
	flatbuffers::FlatBufferBuilder builder;

	/// SerializeData
	vector<vector<flatbuffers::Offset<Vertex_Meta>>>		output_vertex;			//vertex
	vector<vector<flatbuffers::Offset<Index_Meta>>>			output_Index;			//index
	vector<vector<flatbuffers::Offset<Material_Meta>>>		output_Material;		//Material
	vector<vector<flatbuffers::Offset<OneNodeData_Meta>>>	output_NodeData;		//Nodedata
	vector<flatbuffers::Offset<Bone_Meta>>					output_Bone;			//bone
	vector<flatbuffers::Offset<AnimationData_Meta>>			output_AnimationData;	//animation
	vector<vector<flatbuffers::Offset<OneFrameList_Meta>>>	output_KeyFrameData;	//keyframe

	/// Mesh
	int count = inputdata->m_pFbxDataList.size();
	output_vertex.resize(count);
	output_Index.resize(count);
	output_Material.resize(count);
	output_NodeData.resize(count);
	for (int i = 0; i < count; i++)
	{
		/// Vertex
		int VertexCount = inputdata->m_pFbxDataList[i]->m_pVertexList.size();
		for (int j = 0; j < VertexCount; j++)
		{
			Vector4 posdata = inputdata->m_pFbxDataList[i]->m_pVertexList[j].Pos;
			Vector4 WeightData = inputdata->m_pFbxDataList[i]->m_pVertexList[j].Weight;
			DWORD indexdata[4];
			for (int k = 0; k < 4; k++)
			{
				indexdata[k] = inputdata->m_pFbxDataList[i]->m_pVertexList[j].index[k];
			}
			Vector3 nordata = inputdata->m_pFbxDataList[i]->m_pVertexList[j].Nor;
			Vector3 binordata = inputdata->m_pFbxDataList[i]->m_pVertexList[j].Binor;
			Vector3 tangentdata = inputdata->m_pFbxDataList[i]->m_pVertexList[j].Tangent;
			Vector2 uvdata = inputdata->m_pFbxDataList[i]->m_pVertexList[j].Uv;

			auto pos = Vec4(posdata.x, posdata.y, posdata.z, posdata.w);
			auto weight = Vec4(WeightData.x, WeightData.y, WeightData.z, WeightData.w);
			auto index = Vec4(indexdata[0], indexdata[1], indexdata[2], indexdata[3]);
			auto nor = Vec3(nordata.x, nordata.y, nordata.z);
			auto binor = Vec3(binordata.x, binordata.y, binordata.z);
			auto tangent = Vec3(tangentdata.x, tangentdata.y, tangentdata.z);
			auto uv = Vec2(uvdata.x, uvdata.y);

			auto vertexdata = CreateVertex_Meta(builder, &pos, &weight, &index, &nor, &binor, &tangent, &uv);
			output_vertex[i].push_back(vertexdata);
		}

		/// Index
		int Indexcount = inputdata->m_pFbxDataList[i]->m_pIndexList.size();
		for (int i = 0; i < Indexcount; i++)
		{
			auto index_a = inputdata->m_pFbxDataList[i]->m_pIndexList[i].A;
			auto index_b = inputdata->m_pFbxDataList[i]->m_pIndexList[i].B;
			auto index_c = inputdata->m_pFbxDataList[i]->m_pIndexList[i].C;

			auto indexdata = CreateIndex_Meta(builder, index_a, index_b, index_c);
			output_Index[i].push_back(indexdata);
		}

		/// Material
		Fbx_Material matdata;
		matdata.Diffuse = inputdata->m_pFbxDataList[i]->m_pMaterial->Diffuse;
		matdata.Ambient = inputdata->m_pFbxDataList[i]->m_pMaterial->Ambient;
		matdata.Specular = inputdata->m_pFbxDataList[i]->m_pMaterial->Specular;
		matdata.MaterialNum = inputdata->m_pFbxDataList[i]->m_pMaterial->MaterialNum;

		auto dif = Vec4(matdata.Diffuse.x, matdata.Diffuse.y, matdata.Diffuse.z, matdata.Diffuse.w);
		auto amb = Vec4(matdata.Ambient.x, matdata.Ambient.y, matdata.Ambient.z, matdata.Ambient.w);
		auto spc = Vec4(matdata.Specular.x, matdata.Specular.y, matdata.Specular.z, matdata.Specular.w);
		auto matnum = matdata.MaterialNum;

		auto materialdata = CreateMaterial_Meta(builder, &dif, &amb, &spc, matnum);
		output_Material[i].push_back(materialdata);

		/// Nodedata

		auto nodename = builder.CreateString(inputdata->m_pFbxDataList[i]->m_NodeName);
		auto worldm = ChangeMatrixdataToMat(inputdata->m_pFbxDataList[i]->m_Worldm);
		auto globalbindingm = ChangeMatrixdataToMat(inputdata->m_pFbxDataList[i]->m_GlobalBindPos);
		auto binding = inputdata->m_pFbxDataList[i]->m_isBinding;
		auto texon = inputdata->m_pFbxDataList[i]->m_TextureOn;
		auto noron = inputdata->m_pFbxDataList[i]->m_NormalMapOn;
		auto maskon = inputdata->m_pFbxDataList[i]->m_MaskMapOn;

		auto texcount = inputdata->m_pFbxDataList[i]->m_TextureCount;
		auto norcount = inputdata->m_pFbxDataList[i]->m_NormalCount;
		auto maskcount = inputdata->m_pFbxDataList[i]->m_MaskMapCount;
		auto polycount = inputdata->m_pFbxDataList[i]->m_PolygonCount;
		auto vertexcount = inputdata->m_pFbxDataList[i]->m_VertexCount;
		auto indexcount = inputdata->m_pFbxDataList[i]->m_IndexCount;

		auto nodedata = CreateOneNodeData_Meta(builder, nodename, &worldm, &globalbindingm, binding, texon, noron, maskon,
			texcount, norcount, maskcount, polycount, vertexcount, indexcount);

		output_NodeData[i].push_back(nodedata);
	}

	/// Bone  - All
	int BoneCount = inputdata->m_pAllBoneList.size();
	for (int i = 0; i < BoneCount; i++)
	{
		auto name = builder.CreateString(inputdata->m_pAllBoneList[i]->m_Name);
		auto parentname = builder.CreateString(inputdata->m_pAllBoneList[i]->m_Parentname);
		auto boneindex = inputdata->m_pAllBoneList[i]->BoneIndex;
		auto localtm = ChangeMatrixdataToMat(inputdata->m_pAllBoneList[i]->LocalTm);
		auto globaltm = ChangeMatrixdataToMat(inputdata->m_pAllBoneList[i]->GlobalTM);
		auto globalbindingtm = ChangeMatrixdataToMat(inputdata->m_pAllBoneList[i]->GlobalBindingTm);

		auto bonedata = CreateBone_Meta(builder, name, parentname, boneindex, &localtm, &globaltm, &globalbindingtm);
		output_Bone.push_back(bonedata);
	}

	/// Animation - 일단 하나만... 여러 애니메이션 로드도 만들어야됨.
	int anicount = inputdata->m_pAniDataList.size();
	for (int i = 0; i < anicount; i++)
	{
		auto framepersecond = inputdata->m_pAniDataList[i]->FramesPerSecond;
		auto totalframe = inputdata->m_pAniDataList[i]->TotalFrames;
		auto starttime = inputdata->m_pAniDataList[i]->StartTime;
		auto endtime = inputdata->m_pAniDataList[i]->EndTime;
		auto totaltime = inputdata->m_pAniDataList[i]->TotalTime;
		auto name = builder.CreateString(inputdata->m_pAniDataList[i]->Name);
		auto boneoffset = ChangeMatrixdataToMat(inputdata->m_pAniDataList[i]->BoneOffSet);


		int aniframelistcount = inputdata->m_pAniDataList[i]->AnimationList.size();
		for (int j = 0; j < aniframelistcount; j++)
		{
			int frmcount = inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList.size();
			output_KeyFrameData.resize(aniframelistcount);
			for (int k = 0; k < frmcount; k++)
			{
				KeyFrameData oneframedata;
				oneframedata.Time = inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Time;
				oneframedata.Trans = inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Trans;
				oneframedata.Rot_Quat = inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Rot_Quat;
				oneframedata.Scale = inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Scale;

				auto time = oneframedata.Time;
				auto trans = Vec3(oneframedata.Trans.x, oneframedata.Trans.y, oneframedata.Trans.z);
				auto Rot_quat = Vec4(oneframedata.Rot_Quat.x, oneframedata.Rot_Quat.y, oneframedata.Rot_Quat.z, oneframedata.Rot_Quat.w);
				auto Scale = Vec3(oneframedata.Scale.x, oneframedata.Scale.y, oneframedata.Scale.z);

				auto OneFrame = CreateOneFrameList_Meta(builder, time, &trans, &Rot_quat, &Scale);
				output_KeyFrameData[j].push_back(OneFrame);
			}
		}

		auto anidata = CreateAnimationData_Meta(builder, framepersecond, totalframe, starttime, endtime, totaltime, name, &boneoffset);	
		output_AnimationData.push_back(anidata);
	}

	//auto FinalData = CreateMeshData_Meta(builder, )
}

void FbxDataExporter::ExportFbxData_Flex(string filename, Fbx_MeshData* inputdata)
{
	// 먼저 리스트에 있는지 검사...
	flexbuffers::Builder builder;

	builder.Map([&] {
		builder.Vector("MeshData", [&] {
			builder.String(inputdata->m_ModelName);
			builder.Float(inputdata->m_BiggstX);
			builder.Float(inputdata->m_BiggstY);
			builder.Float(inputdata->m_BiggstZ);
			builder.Float(inputdata->m_leastX);
			builder.Float(inputdata->m_leastY);
			builder.Float(inputdata->m_leastZ);
			int Nodecount = inputdata->m_pFbxDataList.size();
			builder.Int(Nodecount);
			for (int i = 0; i < Nodecount; i++)
			{
				/// Vertex
				int VertexCount = inputdata->m_pFbxDataList[i]->m_pVertexList.size();
				builder.Int(VertexCount);
				for (int j = 0; j < VertexCount; j++)
				{
					// pos
					builder.TypedVector([&] {
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Pos.x);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Pos.y);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Pos.z);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Pos.w);
						});
					// weight
					builder.TypedVector([&] {
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Weight.x);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Weight.y);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Weight.z);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Weight.w);
						});
					// weightindex
					builder.TypedVector([&] {
						builder.Int(inputdata->m_pFbxDataList[i]->m_pVertexList[j].index[0]);
						builder.Int(inputdata->m_pFbxDataList[i]->m_pVertexList[j].index[1]);
						builder.Int(inputdata->m_pFbxDataList[i]->m_pVertexList[j].index[2]);
						builder.Int(inputdata->m_pFbxDataList[i]->m_pVertexList[j].index[3]);
						});
					// Normal
					builder.TypedVector([&] {
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Nor.x);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Nor.y);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Nor.z);
						});
					// Binormal
					builder.TypedVector([&] {
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Binor.x);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Binor.y);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Binor.z);
						});
					// tangent
					builder.TypedVector([&] {
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Tangent.x);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Tangent.y);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Tangent.z);
						});
					// uv
					builder.TypedVector([&] {
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Uv.x);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pVertexList[j].Uv.y);
						});
				}

				/// Index
				int indexcount = inputdata->m_pFbxDataList[i]->m_pIndexList.size();
				builder.Int(indexcount);
				for (int j = 0; j < indexcount; j++)
				{
					builder.TypedVector([&] {
						builder.Int(inputdata->m_pFbxDataList[i]->m_pIndexList[j].A);
						builder.Int(inputdata->m_pFbxDataList[i]->m_pIndexList[j].B);
						builder.Int(inputdata->m_pFbxDataList[i]->m_pIndexList[j].C);
						});
				}
				/// Meterial
				// diffuse
				if (inputdata->m_pFbxDataList[i]->m_pMaterial != NULL)
				{
					builder.Int(0);
					builder.TypedVector([&] {
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Diffuse.x);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Diffuse.y);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Diffuse.z);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Diffuse.w);
						});
					// ambient
					builder.TypedVector([&] {
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Ambient.x);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Ambient.y);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Ambient.z);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Ambient.w);
						});
					// Specular
					builder.TypedVector([&] {
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Specular.x);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Specular.y);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Specular.z);
						builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->Specular.w);
						});
					// Material Number
					builder.Float(inputdata->m_pFbxDataList[i]->m_pMaterial->MaterialNum);

				}
				else
				{
					builder.Int(1);
				}
				/// NodeData
				// NodeName
				builder.String(inputdata->m_pFbxDataList[i]->m_NodeName);
				// state
				builder.Bool(inputdata->m_pFbxDataList[i]->m_isBinding);
				builder.Bool(inputdata->m_pFbxDataList[i]->m_TextureOn);
				builder.Bool(inputdata->m_pFbxDataList[i]->m_NormalMapOn);
				builder.Bool(inputdata->m_pFbxDataList[i]->m_MaskMapOn);
				// count
				builder.Int(inputdata->m_pFbxDataList[i]->m_TextureCount);
				builder.Int(inputdata->m_pFbxDataList[i]->m_NormalCount);
				builder.Int(inputdata->m_pFbxDataList[i]->m_MaskMapCount);
				builder.Int(inputdata->m_pFbxDataList[i]->m_PolygonCount);
				builder.Int(inputdata->m_pFbxDataList[i]->m_VertexCount);
				builder.Int(inputdata->m_pFbxDataList[i]->m_IndexCount);
				// WorldTm _ Row
				builder.TypedVector([&] {
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[0][0]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[0][1]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[0][2]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[0][3]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[1][0]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[1][1]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[1][2]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[1][3]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[2][0]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[2][1]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[2][2]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[2][3]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[3][0]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[3][1]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[3][2]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_Worldm.m[3][3]);
					});

				// GlobalBindingTm _ Row
				builder.TypedVector([&] {
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[0][0]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[0][1]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[0][2]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[0][3]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[1][0]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[1][1]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[1][2]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[1][3]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[2][0]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[2][1]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[2][2]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[2][3]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[3][0]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[3][1]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[3][2]);
					builder.Float(inputdata->m_pFbxDataList[i]->m_GlobalBindPos.m[3][3]);
					});
			}
		});

		/// Bone
		builder.Vector("BoneData", [&] {
			int BoneCount = inputdata->m_pAllBoneList.size();
			builder.Int(BoneCount);
			for (int i = 0; i < BoneCount; i++)
			{
				// BoneName
				builder.String(inputdata->m_pAllBoneList[i]->m_Name);
				builder.String(inputdata->m_pAllBoneList[i]->m_Parentname);
				builder.Int(inputdata->m_pAllBoneList[i]->BoneIndex);

				// LocalTM _ Row
				builder.TypedVector([&] {
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[0][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[0][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[0][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[0][3]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[1][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[1][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[1][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[1][3]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[2][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[2][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[2][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[2][3]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[3][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[3][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[3][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->LocalTm.m[3][3]);
					});

				// GlobalTM _ Row
				builder.TypedVector([&] {
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[0][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[0][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[0][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[0][3]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[1][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[1][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[1][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[1][3]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[2][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[2][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[2][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[2][3]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[3][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[3][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[3][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalTM.m[3][3]);
					});
				// GlobalTM _ Row
				builder.TypedVector([&] {
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[0][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[0][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[0][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[0][3]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[1][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[1][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[1][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[1][3]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[2][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[2][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[2][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[2][3]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[3][0]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[3][1]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[3][2]);
					builder.Float(inputdata->m_pAllBoneList[i]->GlobalBindingTm.m[3][3]);
					});

			}
			});

		/// Animation
		builder.Vector("AnimationData", [&] {
			int anicount = inputdata->m_pAniDataList.size();
			builder.Int(anicount);
			for (int i = 0; i < anicount; i++)
			{
				/// Animation Data
				// FramePerSecond
				builder.Float(inputdata->m_pAniDataList[i]->FramesPerSecond);
				// TotalFrames
				builder.Int(inputdata->m_pAniDataList[i]->TotalFrames);
				// StartTime
				builder.Float(inputdata->m_pAniDataList[i]->StartTime);
				// EndTime
				builder.Float(inputdata->m_pAniDataList[i]->EndTime);
				// TotalTime
				builder.Float(inputdata->m_pAniDataList[i]->TotalTime);
				// AnimationName
				builder.String(inputdata->m_pAniDataList[i]->Name);
				// BoneOffSet _ Row
				builder.TypedVector([&] {
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[0][0]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[0][1]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[0][2]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[0][3]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[1][0]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[1][1]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[1][2]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[1][3]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[2][0]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[2][1]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[2][2]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[2][3]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[3][0]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[3][1]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[3][2]);
					builder.Float(inputdata->m_pAniDataList[i]->BoneOffSet.m[3][3]);
					});

				// Animation Count
				int aniframelistcount = inputdata->m_pAniDataList[i]->AnimationList.size();
				builder.Int(aniframelistcount);
				for (int j = 0; j < aniframelistcount; j++)
				{ 
					// FrameCount
					int frmcount = inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList.size();
					builder.Int(frmcount);
					for (int k = 0; k < frmcount; k++)
					{
						// Time
						builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Time);
						// Position
						builder.TypedVector([&] {
							builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Trans.x);
							builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Trans.y);
							builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Trans.z);
							});
						// Rotaion_Quat
						builder.TypedVector([&] {
							builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Rot_Quat.x);
							builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Rot_Quat.y);
							builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Rot_Quat.z);
							builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Rot_Quat.w);
							});
						// Scale
						builder.TypedVector([&] {
							builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Scale.x);
							builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Scale.y);
							builder.Float(inputdata->m_pAniDataList[i]->AnimationList[j]->FrmList[k].Scale.z);
							});
					}

				}
			}
			});

		/// Texture_String
		builder.Vector("Texture", [&] {
			builder.String(inputdata->m_TexPath);
			int texcount = inputdata->m_Texfilename.size();
			builder.Int(texcount);
			for (int i = 0; i < texcount; i++)
			{
				builder.String(inputdata->m_Texfilename[i]);
			}
			int norcount = inputdata->m_Norfilename.size();
			builder.Int(norcount);
			for (int i = 0; i < norcount; i++)
			{
				builder.String(inputdata->m_Norfilename[i]);
			}
			int maskcount = inputdata->m_MaskMapfilename.size();
			builder.Int(maskcount);
			for (int i = 0; i < maskcount; i++)
			{
				builder.String(inputdata->m_MaskMapfilename[i]);
			}
			int linknodecount = inputdata->m_LinkNodeNameList.size();
			builder.Int(linknodecount);
			for (int i = 0; i < linknodecount; i++)
			{
				builder.String(inputdata->m_LinkNodeNameList[i]);
			}
			int ORMCount = inputdata->m_ORMfilename.size();
			builder.Int(ORMCount);
			for (int i = 0; i < ORMCount; i++)
			{
				builder.String(inputdata->m_ORMfilename[i]);
			}
			});



		/// MeshFilterSet
		builder.Vector("Meshfilter", [&] {
			switch (inputdata->m_MeshFilterSort)
			{
			case MeshFilter::eMeshfiltersort::SkinnedMesh:
				builder.Int(0);
				break;
			case MeshFilter::eMeshfiltersort::StaticMesh:
				builder.Int(1);
				break;
			}
			});
	});

	builder.Finish();
	vector<uint8_t> objectdata = builder.GetBuffer();
	string exportfilename = detachstring_fbx(inputdata->m_ModelName);
	string exportfilepath = inputdata->m_TexPath;
	string final = exportfilepath + exportfilename;
	flatbuffers::SaveFile(final.c_str(), reinterpret_cast<const char*>(objectdata.data()), objectdata.size(), true);

	m_ImportDatalist.insert(std::pair<std::string, Fbx_MeshData*>(final, inputdata));
}

Fbx_MeshData* FbxDataExporter::ImportFbxMetafile(string filename)
{
	// Genarate New MeshData_Pointer
	Fbx_MeshData* inputdata = nullptr;

	// 먼저 리스트에 있는지 검사...
	std::map<std::string, Fbx_MeshData*>::iterator it;
	for (it = m_ImportDatalist.begin(); it != m_ImportDatalist.end(); it++)
	{
		string name = it->first;

		if (name == filename)
		{
			inputdata = it->second;

			return inputdata;
		}
	}

	string data;

	flatbuffers::LoadFile(filename.c_str(), true, &data);

	flexbuffers::Builder builder;

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();

	auto Meshdatalist = m["MeshData"].AsVector();
	auto Bonedatalist = m["BoneData"].AsVector();
	auto Animationdatalist = m["AnimationData"].AsVector();
	auto TextureList = m["Texture"].AsVector();
	auto MeshFilter = m["Meshfilter"].AsVector();

	inputdata = new Fbx_MeshData();

	/// Mesh
	int Meshcnt = Meshdatalist.size();
	// ModelName
	inputdata->m_ModelName = Meshdatalist[0].AsString().c_str();
	inputdata->m_BiggstX = Meshdatalist[1].AsFloat();
	inputdata->m_BiggstY = Meshdatalist[2].AsFloat();
	inputdata->m_BiggstZ = Meshdatalist[3].AsFloat();
	inputdata->m_leastX = Meshdatalist[4].AsFloat();
	inputdata->m_leastY = Meshdatalist[5].AsFloat();
	inputdata->m_leastZ = Meshdatalist[6].AsFloat();
	for(int Meshindex = 7; Meshindex < Meshcnt; Meshindex++)
	{
		// Node count
		int NodeCount = Meshdatalist[Meshindex].AsInt64();
		Meshindex++;
		for (int i = 0; i < NodeCount; i++)
		{
			FbxData_Mesh* MeshData = new FbxData_Mesh();
			/// Vertex
			int vertexcount = Meshdatalist[Meshindex].AsInt64();
			Meshindex++;
			for (int j = 0; j < vertexcount; j++)
			{
				Vertex vertex;
				// Pos
				vertex.Pos.x = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
				vertex.Pos.y = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
				vertex.Pos.z = Meshdatalist[Meshindex].AsTypedVector()[2].AsFloat();
				vertex.Pos.w = Meshdatalist[Meshindex].AsTypedVector()[3].AsFloat();
				Meshindex++;
				// Weight
				vertex.Weight.x = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
				vertex.Weight.y = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
				vertex.Weight.z = Meshdatalist[Meshindex].AsTypedVector()[2].AsFloat();
				vertex.Weight.w = Meshdatalist[Meshindex].AsTypedVector()[3].AsFloat();
				Meshindex++;
				// Weightindex
				vertex.index[0] = Meshdatalist[Meshindex].AsTypedVector()[0].AsInt64();
				vertex.index[1] = Meshdatalist[Meshindex].AsTypedVector()[1].AsInt64();
				vertex.index[2] = Meshdatalist[Meshindex].AsTypedVector()[2].AsInt64();
				vertex.index[3] = Meshdatalist[Meshindex].AsTypedVector()[3].AsInt64();
				Meshindex++;
				// Normal
				vertex.Nor.x = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
				vertex.Nor.y = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
				vertex.Nor.z = Meshdatalist[Meshindex].AsTypedVector()[2].AsFloat();
				Meshindex++;
				// Binormal
				vertex.Binor.x = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
				vertex.Binor.y = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
				vertex.Binor.z = Meshdatalist[Meshindex].AsTypedVector()[2].AsFloat();
				Meshindex++;
				// Tangent
				vertex.Tangent.x = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
				vertex.Tangent.y = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
				vertex.Tangent.z = Meshdatalist[Meshindex].AsTypedVector()[2].AsFloat();
				Meshindex++;
				// UV
				vertex.Uv.x = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
				vertex.Uv.y = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
				Meshindex++;
				MeshData->m_pVertexList.push_back(vertex);
			}

			/// Index
			int IndexCount = Meshdatalist[Meshindex].AsInt64();
			Meshindex++;
			for (int i = 0; i < IndexCount; i++)
			{
				// Index
				Index IndexData;
				IndexData.A = Meshdatalist[Meshindex].AsTypedVector()[0].AsInt64();
				IndexData.B = Meshdatalist[Meshindex].AsTypedVector()[1].AsInt64();
				IndexData.C = Meshdatalist[Meshindex].AsTypedVector()[2].AsInt64();
				Meshindex++;
				MeshData->m_pIndexList.push_back(IndexData);
			}
			/// Meterial _ Generate
			Fbx_Material* Mat = new Fbx_Material();
			if (Meshdatalist[Meshindex].AsInt64() == 0)
			{
				Meshindex++;
				// Diffuse
				Mat->Diffuse.x = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
				Mat->Diffuse.y = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
				Mat->Diffuse.z = Meshdatalist[Meshindex].AsTypedVector()[2].AsFloat();
				Meshindex++;
				// Ambient
				Mat->Ambient.x = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
				Mat->Ambient.y = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
				Mat->Ambient.z = Meshdatalist[Meshindex].AsTypedVector()[2].AsFloat();
				Meshindex++;
				// Specular
				Mat->Specular.x = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
				Mat->Specular.y = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
				Mat->Specular.z = Meshdatalist[Meshindex].AsTypedVector()[2].AsFloat();
				Meshindex++;
				// MaterialNumber
				Mat->MaterialNum = Meshdatalist[Meshindex].AsFloat();
				Meshindex++;

			}
			else
			{
				Meshindex++;
			}
			MeshData->m_pMaterial = Mat;

			/// NodeData
			// Nodename
			MeshData->m_NodeName = Meshdatalist[Meshindex].AsString().c_str();
			Meshindex++;
			// state - binding
			MeshData->m_isBinding = Meshdatalist[Meshindex].AsBool();
			Meshindex++;
			// state - Texture
			MeshData->m_TextureOn = Meshdatalist[Meshindex].AsBool();
			Meshindex++;
			// state - Normal
			MeshData->m_NormalMapOn = Meshdatalist[Meshindex].AsBool();
			Meshindex++;
			// state - MaskMap
			MeshData->m_MaskMapOn = Meshdatalist[Meshindex].AsBool();
			Meshindex++;

			// Count - Texture
			MeshData->m_TextureCount = Meshdatalist[Meshindex].AsInt64();
			Meshindex++;
			// Count - Normal
			MeshData->m_NormalCount = Meshdatalist[Meshindex].AsInt64();
			Meshindex++;
			// Count - Mask
			MeshData->m_MaskMapCount = Meshdatalist[Meshindex].AsInt64();
			Meshindex++;
			// Count - Polygon
			MeshData->m_PolygonCount = Meshdatalist[Meshindex].AsInt64();
			Meshindex++;
			// Count - VertexCount
			MeshData->m_VertexCount = Meshdatalist[Meshindex].AsInt64();
			Meshindex++;
			// Count - IndexCount
			MeshData->m_IndexCount = Meshdatalist[Meshindex].AsInt64();
			Meshindex++;

			// WorldTm _ Row
			MeshData->m_Worldm.m[0][0] = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
			MeshData->m_Worldm.m[0][1] = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
			MeshData->m_Worldm.m[0][2] = Meshdatalist[Meshindex].AsTypedVector()[2].AsFloat();
			MeshData->m_Worldm.m[0][3] = Meshdatalist[Meshindex].AsTypedVector()[3].AsFloat();
			MeshData->m_Worldm.m[1][0] = Meshdatalist[Meshindex].AsTypedVector()[4].AsFloat();
			MeshData->m_Worldm.m[1][1] = Meshdatalist[Meshindex].AsTypedVector()[5].AsFloat();
			MeshData->m_Worldm.m[1][2] = Meshdatalist[Meshindex].AsTypedVector()[6].AsFloat();
			MeshData->m_Worldm.m[1][3] = Meshdatalist[Meshindex].AsTypedVector()[7].AsFloat();
			MeshData->m_Worldm.m[2][0] = Meshdatalist[Meshindex].AsTypedVector()[8].AsFloat();
			MeshData->m_Worldm.m[2][1] = Meshdatalist[Meshindex].AsTypedVector()[9].AsFloat();
			MeshData->m_Worldm.m[2][2] = Meshdatalist[Meshindex].AsTypedVector()[10].AsFloat();
			MeshData->m_Worldm.m[2][3] = Meshdatalist[Meshindex].AsTypedVector()[11].AsFloat();
			MeshData->m_Worldm.m[3][0] = Meshdatalist[Meshindex].AsTypedVector()[12].AsFloat();
			MeshData->m_Worldm.m[3][1] = Meshdatalist[Meshindex].AsTypedVector()[13].AsFloat();
			MeshData->m_Worldm.m[3][2] = Meshdatalist[Meshindex].AsTypedVector()[14].AsFloat();
			MeshData->m_Worldm.m[3][3] = Meshdatalist[Meshindex].AsTypedVector()[15].AsFloat();
			Meshindex++;

			// GlobalBindingTm _ Row
			MeshData->m_GlobalBindPos.m[0][0] = Meshdatalist[Meshindex].AsTypedVector()[0].AsFloat();
			MeshData->m_GlobalBindPos.m[0][1] = Meshdatalist[Meshindex].AsTypedVector()[1].AsFloat();
			MeshData->m_GlobalBindPos.m[0][2] = Meshdatalist[Meshindex].AsTypedVector()[2].AsFloat();
			MeshData->m_GlobalBindPos.m[0][3] = Meshdatalist[Meshindex].AsTypedVector()[3].AsFloat();
			MeshData->m_GlobalBindPos.m[1][0] = Meshdatalist[Meshindex].AsTypedVector()[4].AsFloat();
			MeshData->m_GlobalBindPos.m[1][1] = Meshdatalist[Meshindex].AsTypedVector()[5].AsFloat();
			MeshData->m_GlobalBindPos.m[1][2] = Meshdatalist[Meshindex].AsTypedVector()[6].AsFloat();
			MeshData->m_GlobalBindPos.m[1][3] = Meshdatalist[Meshindex].AsTypedVector()[7].AsFloat();
			MeshData->m_GlobalBindPos.m[2][0] = Meshdatalist[Meshindex].AsTypedVector()[8].AsFloat();
			MeshData->m_GlobalBindPos.m[2][1] = Meshdatalist[Meshindex].AsTypedVector()[9].AsFloat();
			MeshData->m_GlobalBindPos.m[2][2] = Meshdatalist[Meshindex].AsTypedVector()[10].AsFloat();
			MeshData->m_GlobalBindPos.m[2][3] = Meshdatalist[Meshindex].AsTypedVector()[11].AsFloat();
			MeshData->m_GlobalBindPos.m[3][0] = Meshdatalist[Meshindex].AsTypedVector()[12].AsFloat();
			MeshData->m_GlobalBindPos.m[3][1] = Meshdatalist[Meshindex].AsTypedVector()[13].AsFloat();
			MeshData->m_GlobalBindPos.m[3][2] = Meshdatalist[Meshindex].AsTypedVector()[14].AsFloat();
			MeshData->m_GlobalBindPos.m[3][3] = Meshdatalist[Meshindex].AsTypedVector()[15].AsFloat();
			Meshindex++;

			inputdata->m_pFbxDataList.push_back(MeshData);
		}
	}


	/// Bone
	// 데이터 로드...
	int bonecnt = Bonedatalist.size();
	for (int index = 0; index < bonecnt; index++)
	{
		int bonesize = Bonedatalist[index].AsInt64();
		index++;
		for (int j = 0; j < bonesize; j++)
		{
			Bone* pbone = new Bone();
			// Name
			pbone->m_Name = Bonedatalist[index].AsString().c_str();
			index++;
			// ParentName
			pbone->m_Parentname = Bonedatalist[index].AsString().c_str();
			index++;
			// BoneIndex
			pbone->BoneIndex = Bonedatalist[index].AsInt64();
			index++;
			// LocalTM
			pbone->LocalTm.m[0][0] = Bonedatalist[index].AsTypedVector()[0].AsFloat();
			pbone->LocalTm.m[0][1] = Bonedatalist[index].AsTypedVector()[1].AsFloat();
			pbone->LocalTm.m[0][2] = Bonedatalist[index].AsTypedVector()[2].AsFloat();
			pbone->LocalTm.m[0][3] = Bonedatalist[index].AsTypedVector()[3].AsFloat();
			pbone->LocalTm.m[1][0] = Bonedatalist[index].AsTypedVector()[4].AsFloat();
			pbone->LocalTm.m[1][1] = Bonedatalist[index].AsTypedVector()[5].AsFloat();
			pbone->LocalTm.m[1][2] = Bonedatalist[index].AsTypedVector()[6].AsFloat();
			pbone->LocalTm.m[1][3] = Bonedatalist[index].AsTypedVector()[7].AsFloat();
			pbone->LocalTm.m[2][0] = Bonedatalist[index].AsTypedVector()[8].AsFloat();
			pbone->LocalTm.m[2][1] = Bonedatalist[index].AsTypedVector()[9].AsFloat();
			pbone->LocalTm.m[2][2] = Bonedatalist[index].AsTypedVector()[10].AsFloat();
			pbone->LocalTm.m[2][3] = Bonedatalist[index].AsTypedVector()[11].AsFloat();
			pbone->LocalTm.m[3][0] = Bonedatalist[index].AsTypedVector()[12].AsFloat();
			pbone->LocalTm.m[3][1] = Bonedatalist[index].AsTypedVector()[13].AsFloat();
			pbone->LocalTm.m[3][2] = Bonedatalist[index].AsTypedVector()[14].AsFloat();
			pbone->LocalTm.m[3][3] = Bonedatalist[index].AsTypedVector()[15].AsFloat();
			index++;
			// GlobalTM
			pbone->GlobalTM.m[0][0] = Bonedatalist[index].AsTypedVector()[0].AsFloat();
			pbone->GlobalTM.m[0][1] = Bonedatalist[index].AsTypedVector()[1].AsFloat();
			pbone->GlobalTM.m[0][2] = Bonedatalist[index].AsTypedVector()[2].AsFloat();
			pbone->GlobalTM.m[0][3] = Bonedatalist[index].AsTypedVector()[3].AsFloat();
			pbone->GlobalTM.m[1][0] = Bonedatalist[index].AsTypedVector()[4].AsFloat();
			pbone->GlobalTM.m[1][1] = Bonedatalist[index].AsTypedVector()[5].AsFloat();
			pbone->GlobalTM.m[1][2] = Bonedatalist[index].AsTypedVector()[6].AsFloat();
			pbone->GlobalTM.m[1][3] = Bonedatalist[index].AsTypedVector()[7].AsFloat();
			pbone->GlobalTM.m[2][0] = Bonedatalist[index].AsTypedVector()[8].AsFloat();
			pbone->GlobalTM.m[2][1] = Bonedatalist[index].AsTypedVector()[9].AsFloat();
			pbone->GlobalTM.m[2][2] = Bonedatalist[index].AsTypedVector()[10].AsFloat();
			pbone->GlobalTM.m[2][3] = Bonedatalist[index].AsTypedVector()[11].AsFloat();
			pbone->GlobalTM.m[3][0] = Bonedatalist[index].AsTypedVector()[12].AsFloat();
			pbone->GlobalTM.m[3][1] = Bonedatalist[index].AsTypedVector()[13].AsFloat();
			pbone->GlobalTM.m[3][2] = Bonedatalist[index].AsTypedVector()[14].AsFloat();
			pbone->GlobalTM.m[3][3] = Bonedatalist[index].AsTypedVector()[15].AsFloat();
			index++;
			// GlobalBindingTM
			pbone->GlobalBindingTm.m[0][0] = Bonedatalist[index].AsTypedVector()[0].AsFloat();
			pbone->GlobalBindingTm.m[0][1] = Bonedatalist[index].AsTypedVector()[1].AsFloat();
			pbone->GlobalBindingTm.m[0][2] = Bonedatalist[index].AsTypedVector()[2].AsFloat();
			pbone->GlobalBindingTm.m[0][3] = Bonedatalist[index].AsTypedVector()[3].AsFloat();
			pbone->GlobalBindingTm.m[1][0] = Bonedatalist[index].AsTypedVector()[4].AsFloat();
			pbone->GlobalBindingTm.m[1][1] = Bonedatalist[index].AsTypedVector()[5].AsFloat();
			pbone->GlobalBindingTm.m[1][2] = Bonedatalist[index].AsTypedVector()[6].AsFloat();
			pbone->GlobalBindingTm.m[1][3] = Bonedatalist[index].AsTypedVector()[7].AsFloat();
			pbone->GlobalBindingTm.m[2][0] = Bonedatalist[index].AsTypedVector()[8].AsFloat();
			pbone->GlobalBindingTm.m[2][1] = Bonedatalist[index].AsTypedVector()[9].AsFloat();
			pbone->GlobalBindingTm.m[2][2] = Bonedatalist[index].AsTypedVector()[10].AsFloat();
			pbone->GlobalBindingTm.m[2][3] = Bonedatalist[index].AsTypedVector()[11].AsFloat();
			pbone->GlobalBindingTm.m[3][0] = Bonedatalist[index].AsTypedVector()[12].AsFloat();
			pbone->GlobalBindingTm.m[3][1] = Bonedatalist[index].AsTypedVector()[13].AsFloat();
			pbone->GlobalBindingTm.m[3][2] = Bonedatalist[index].AsTypedVector()[14].AsFloat();
			pbone->GlobalBindingTm.m[3][3] = Bonedatalist[index].AsTypedVector()[15].AsFloat();
			index++;
			inputdata->m_pAllBoneList.push_back(pbone);
		}

	}

	/// Animation
	int Anicnt = Animationdatalist.size();
	for (int index = 0; index < Anicnt; index++)
	{
		int animationcount = Animationdatalist[index].AsInt64();
		index++;
		for (int i = 0; i < animationcount; i++)
		{
			AnimationData* pAnidata = new AnimationData();
			pAnidata->FramesPerSecond = Animationdatalist[index].AsFloat();
			index++;
			pAnidata->TotalFrames = Animationdatalist[index].AsInt64();
			index++;
			pAnidata->StartTime = Animationdatalist[index].AsFloat();
			index++;
			pAnidata->EndTime = Animationdatalist[index].AsFloat();
			index++;
			pAnidata->TotalTime = Animationdatalist[index].AsFloat();
			index++;
			pAnidata->Name = Animationdatalist[index].AsString().c_str();
			index++;
			// boneoffset
			pAnidata->BoneOffSet.m[0][0] = Bonedatalist[index].AsTypedVector()[0].AsFloat();
			pAnidata->BoneOffSet.m[0][1] = Bonedatalist[index].AsTypedVector()[1].AsFloat();
			pAnidata->BoneOffSet.m[0][2] = Bonedatalist[index].AsTypedVector()[2].AsFloat();
			pAnidata->BoneOffSet.m[0][3] = Bonedatalist[index].AsTypedVector()[3].AsFloat();
			pAnidata->BoneOffSet.m[1][0] = Bonedatalist[index].AsTypedVector()[4].AsFloat();
			pAnidata->BoneOffSet.m[1][1] = Bonedatalist[index].AsTypedVector()[5].AsFloat();
			pAnidata->BoneOffSet.m[1][2] = Bonedatalist[index].AsTypedVector()[6].AsFloat();
			pAnidata->BoneOffSet.m[1][3] = Bonedatalist[index].AsTypedVector()[7].AsFloat();
			pAnidata->BoneOffSet.m[2][0] = Bonedatalist[index].AsTypedVector()[8].AsFloat();
			pAnidata->BoneOffSet.m[2][1] = Bonedatalist[index].AsTypedVector()[9].AsFloat();
			pAnidata->BoneOffSet.m[2][2] = Bonedatalist[index].AsTypedVector()[10].AsFloat();
			pAnidata->BoneOffSet.m[2][3] = Bonedatalist[index].AsTypedVector()[11].AsFloat();
			pAnidata->BoneOffSet.m[3][0] = Bonedatalist[index].AsTypedVector()[12].AsFloat();
			pAnidata->BoneOffSet.m[3][1] = Bonedatalist[index].AsTypedVector()[13].AsFloat();
			pAnidata->BoneOffSet.m[3][2] = Bonedatalist[index].AsTypedVector()[14].AsFloat();
			pAnidata->BoneOffSet.m[3][3] = Bonedatalist[index].AsTypedVector()[15].AsFloat();
			index++;
			// Framelist
			int animatrixcount = Animationdatalist[index].AsInt64();
			index++;
			for (int j = 0; j < animatrixcount; j++)
			{
				OneFrameList* pFramelist = new OneFrameList();
				int Framelistcount = Animationdatalist[index].AsInt64();
				index++;
				for (int k = 0; k < Framelistcount; k++)
				{
					KeyFrameData currKey;
					currKey.Time = Animationdatalist[index].AsFloat();
					index++;
					currKey.Trans.x = Animationdatalist[index].AsTypedVector()[0].AsFloat();
					currKey.Trans.y = Animationdatalist[index].AsTypedVector()[1].AsFloat();
					currKey.Trans.z = Animationdatalist[index].AsTypedVector()[2].AsFloat();
					index++;
					currKey.Rot_Quat.x = Animationdatalist[index].AsTypedVector()[0].AsFloat();
					currKey.Rot_Quat.y = Animationdatalist[index].AsTypedVector()[1].AsFloat();
					currKey.Rot_Quat.z = Animationdatalist[index].AsTypedVector()[2].AsFloat();
					currKey.Rot_Quat.w = Animationdatalist[index].AsTypedVector()[3].AsFloat();
					index++;
					currKey.Scale.x = Animationdatalist[index].AsTypedVector()[0].AsFloat();
					currKey.Scale.y = Animationdatalist[index].AsTypedVector()[1].AsFloat();
					currKey.Scale.z = Animationdatalist[index].AsTypedVector()[2].AsFloat();
					index++;
					pFramelist->FrmList.push_back(currKey);
				}
				pAnidata->AnimationList.push_back(pFramelist);
			}
			inputdata->m_pAniDataList.push_back(pAnidata);
		}

	}
	/// Texture_String
	int texturecount = TextureList.size();
	for (int index = 0; index < texturecount; index++)
	{
		inputdata->m_TexPath = TextureList[index].AsString().c_str();
		index++;
		// Texture
		int texcount = TextureList[index].AsInt64();
		index++;
		for (int i = 0; i < texcount; i++)
		{
			string texname = TextureList[index].AsString().c_str();
			inputdata->m_Texfilename.push_back(texname);
			index++;
		}

		// Normal
		int norcount = TextureList[index].AsInt64();
		index++;
		for (int i = 0; i < norcount; i++)
		{
			string norname = TextureList[index].AsString().c_str();
			inputdata->m_Norfilename.push_back(norname);
			index++;
		}

		// Mask
		int Maskcount = TextureList[index].AsInt64();
		index++;
		for (int i = 0; i < Maskcount; i++)
		{
			string maskname = TextureList[index].AsString().c_str();
			inputdata->m_MaskMapfilename.push_back(maskname);
			index++;
		}

		// LinkNodeName
		int LinkNodeCount = TextureList[index].AsInt64();
		index++;
		for (int i = 0; i < LinkNodeCount; i++)
		{
			string linknodename = TextureList[index].AsString().c_str();
			inputdata->m_LinkNodeNameList.push_back(linknodename);
			index++;
		}

		// ORM
		int ORMCount = TextureList[index].AsInt64();
		index++;
		for (int i = 0; i < ORMCount; i++)
		{
			string ORMMap = TextureList[index].AsString().c_str();
			inputdata->m_ORMfilename.push_back(ORMMap);
			index++;
		}

	}
	/// MeshFilter
	int Fliter = MeshFilter.size();
	int filterSet = MeshFilter[0].AsInt64();
	switch (filterSet)
	{
	case 0:
		inputdata->m_MeshFilterSort = MeshFilter::eMeshfiltersort::SkinnedMesh;
		break;
	case 1:
		inputdata->m_MeshFilterSort = MeshFilter::eMeshfiltersort::StaticMesh;
		break;
	}
	m_ImportDatalist.insert(std::pair<std::string, Fbx_MeshData*>(filename, inputdata));
	return inputdata;
}

Mat_Meta FbxDataExporter::ChangeMatrixdataToMat(Matrix mat)
{
	Mat_Meta exportmat;

	auto matdata = Mat_Meta(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3]
		, mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3]
		, mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3]
		, mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]);

	return matdata;
}

std::string FbxDataExporter::detachstring_fbx(string name)
{
	for (int i = 0; i < 3; i++)
	{
		name.pop_back();
	}
	string format = "bin";

	return name + format;
}

bool FbxDataExporter::FindBinaryfile(string filename)
{
	string data;

	bool b = flatbuffers::LoadFile(filename.c_str(), true, &data);

	return b;
}

Fbx_MeshData* FbxDataExporter::SetFbxBinFile(string filename)
{
	// Genarate New MeshData_Pointer
	Fbx_MeshData* inputdata = nullptr;

	// 리스트에 있는지 검사...
	std::map<std::string, Fbx_MeshData*>::iterator it;
	for (it = m_ImportDatalist.begin(); it != m_ImportDatalist.end(); it++)
	{
		string name = it->first;

		if (name == filename)
		{
			inputdata = it->second;

			return inputdata;
		}
	}

}

void FbxDataExporter::DeleteImportDataList()
{
	m_ImportDatalist.clear();
}

void FbxDataExporter::SetTextrueDate()
{
	std::map<std::string, Fbx_MeshData*>::iterator it;
	for (it = m_ImportDatalist.begin(); it != m_ImportDatalist.end(); it++)
	{
		it->second->SetMaterialDataAuto();
	}
}
