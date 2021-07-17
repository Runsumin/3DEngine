///
///
///
/// FBX 모델 자체 포맷 생성 클래스
/// [2021/02/09 RUNSUMIN]
#pragma once
#include "DxDefine.h"
#include "FbxData.h"
#include "Fbx_MeshData.h"
#include "MeshData_FBX_generated.h"

// serialize, deserialize
#include <flatbuffers/flexbuffers.h>
#undef min	//define되있지않지만 undef하지않으면 컴파일에러가 발생한다
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"

class FbxDataExporter
{
public:
	FbxDataExporter();
	~FbxDataExporter();
public:
	// 한번 로드한 데이터는 들고있자.. key, value
	std::map<std::string, Fbx_MeshData*> m_ImportDatalist;

public:
	void ExportFbxData(string filename, Fbx_MeshData* inputdata);	
	void ExportFbxData_Flex(string filename, Fbx_MeshData* inputdata);	
	Fbx_MeshData* ImportFbxMetafile(string filename);
	Mat_Meta ChangeMatrixdataToMat(Matrix mat);

	string detachstring_fbx(string name);

	bool FindBinaryfile(string filename);

	Fbx_MeshData* SetFbxBinFile(string filename);

	void DeleteImportDataList();

	void SetTextrueDate();

};

