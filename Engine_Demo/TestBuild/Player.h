///
///
///
///
///
/// Player Generate
/// [2021/06/17/RUNSUMIN]
#pragma once
class Player
{
public:
	Player();
	~Player();
public:
	enum class state
	{
		IDLE, WALK, RUN, ATTACK
	};
public:
	void InitPlayer(HWND hwnd);
	void SetMaterial();
	void Update(float time);
public:
	GRAPHICENGINE::IModelInterface* m_pModel;
private:
	// Material
	vector<GRAPHICENGINE::IMaterialInterface*> m_pDevilORMList;

	void KeyInput();
	void PlayerAniChage();
	void PlayerTransFormChange();
	//악마 움직임...
	state m_PlayerState;
	state m_LatePlayerState;

	// 방향키 입력
	int m_Idle;
	int m_Walk;
	int m_Run;
	int m_Attack;

	Vector3 m_Position;
	Vector3 m_Rotation;
	Vector3 m_Scale;
	Vector3 m_Dir;
	float m_Velocity;

	bool m_bOnce;
	bool m_bRun;

	bool m_b;
	float m_Angle;
};

