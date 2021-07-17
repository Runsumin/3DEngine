///
///
///
///
///
///
/// [2021/06/16/RUNSUMIN]
#pragma once
#include "Model.h"
#include "Map.h"
#include "Player.h"
class Gameprocess
{
public:
	Gameprocess();
	~Gameprocess();

private:
	const int m_ScreenWidth = 1920;
	const int m_ScreenHeight = 1080;

	HWND m_hWnd;
	MSG m_Msg;
	GRAPHICENGINE::ICameraInterface* m_pMainCamera;
	GRAPHICENGINE::IGridInterface* m_pGrid;
	GRAPHICENGINE::ITerrain_Interface* m_pTerrain;
	GRAPHICENGINE::IUiInterface* m_pUI;
	GRAPHICENGINE::ISkyBoxInterface* m_pSkybox;
	GRAPHICENGINE::IDirectionLightInterface* m_pDirLit;

	vector<GRAPHICENGINE::IModelInterface*> m_Modellist;
	bool m_bWireFrame;
	bool m_AniChange;

	Model_default* m_Obelisk;
	Model_default* m_Slate;
	Model_default* m_pKnight;
	Model_default* m_pHouse;

	// Map
	Map* m_pMap;
	// Player
	Player* m_pPlayer;

private:
	Vector4 m_BackBufferCol;
	float EngineTime;
	float m_FPS;
	
public:
	HRESULT Initialize(HINSTANCE hinstance);
	void EngineSetting();
	void GameLoop();
	void Finalize();

	void UpdateAll();
	void RenderAll();
	void SetLight();

public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

