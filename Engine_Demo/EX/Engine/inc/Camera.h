 //
//
//
// 카메라 클래스 생성
// [2020/08/03 RUNSUMIN]
#pragma once
#include "DxDefine.h"
#include "D3DDevice.h"
#include "DrawText_Debug.h"
#include "EngineInterface.h"

class Camera : public GRAPHICENGINE::ICameraInterface
{
public:
	Camera(Vector3 pos, Vector3 rot);
	~Camera();

public:

	Vector3 m_Position;
	Vector3 m_Rotation;
	Vector3 m_Scale;

	// 투영 변환 정보
	float m_Fov;
	float m_AspectRatio;
	float m_Near;
	float m_Far;


private:
	// 리셋용 저장
	Vector3 m_PosBk;
	Vector3 m_RoatationBk;
	Vector3 m_ScaleBk;


	// 렌더타겟 전용 저장.
	Vector3 m_PosRtBk;
	Vector3 m_RoatationRtBk;
	Vector3 m_ScaleRtBk;
	float ViewW;
	float ViewH;
	// 카메라속도
	float m_CameraSpeed;
	GRAPHICENGINE::ICameraInterface::Mode  m_CameraMode;
	// 행렬
	Matrix m_View;
	Matrix m_Proj;
	Matrix m_Transform;

public:
	void ResetCamera();
	void RestoreCamera();
	void BackUpCameraData();
	void SetCameraMatrixData();
	void GetCameraData_FromGameEngine(Matrix view, Matrix proj);
	void CameraInfo(int width, int height);


	///////////////////////////////////////////

	virtual void InitializeCamera()	override;
	virtual	void UpdateCamera(float dTime) override;
	virtual void SetCamerSRTData(Vector3 pos, Vector3 rot, Vector3 scl) override;
	virtual void ChangeCameraMode(ICameraInterface::Mode emode) override;
	virtual void SetCameraSpeed(float speed) override;
	virtual void CameraInfo() override;
	virtual void ReleaseCamera() override;
	virtual Matrix GetCameraViewMat() override;
	virtual Matrix GetCameraProjMat() override;
	virtual Vector3 GetCameraPos() override;
	virtual Vector3 GetCameraRot() override;
	virtual Vector3 GetCameraScl() override;

};



