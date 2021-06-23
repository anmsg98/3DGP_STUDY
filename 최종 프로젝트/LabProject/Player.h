#pragma once

#include "GameObject.h"
#include "Camera.h"

#define DIR_FORWARD		0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT		0x04
#define DIR_RIGHT		0x08
#define DIR_UP			0x10
#define DIR_DOWN		0x20

class CPlayer : public CGameObject
{
public:
	CPlayer(int nMeshes = 1);
	virtual ~CPlayer();
	int life = 3;
	bool boom = false;
protected:
	// 플레이어의 위치 벡터, x-축(Right), y-축(Up), z-축(Look) 벡터이다.
	XMFLOAT3 m_xmf3Position{};
	XMFLOAT3 m_xmf3Right{ XMFLOAT3(1.0f, 0.0f, 0.0f) };
	XMFLOAT3 m_xmf3Up{ XMFLOAT3(0.0f, 1.0f, 0.0f) };
	XMFLOAT3 m_xmf3Look{ XMFLOAT3(0.0f, 0.0f, 1.0f) };

	// 플레이어가 로컬 x-축(Right), y-축(Up), z-축(Look)으로 얼마만큼 회전했는가를 나타낸다.
	float m_fPitch{};
	float m_fYaw{};
	float m_fRoll{};

	// 플레이어의 이동 속도를 나타내는 벡터이다.
	XMFLOAT3 m_xmf3Velocity{};

	// 플레이어에 작용하는 중력을 나타내는 벡터이다.
	XMFLOAT3 m_xmf3Gravity{};
	
	// xz-평면에서 (한 프레임 동안) 플레이어의 이동 속력의 최대값을 나타낸다.
	float m_fMaxVelocityXZ{};
	
	//y-축 방향으로 (한 프레임 동안) 플레이어의 이동 속력의 최대값을 나타낸다.
	float m_fMaxVelocityY{};
	
	// 플레이어에 작용하는 마찰력을 나타낸다.
	float m_fFriction{};

	// 플레이어의 위치가 바뀔 때마다 호출되는 OnPlayerUpdateCallback() 함수에서 사용하는 데이터이다.
	LPVOID m_pPlayerUpdatedContext{};

	// 카메라의 위치가 바뀔 때마다 호출되는 OnCameraUpdateCallback() 함수에서 사용하는 데이터이다.
	LPVOID m_pCameraUpdatedContext{};
	
	// 플레이어에 현재 설정된 카메라이다.
	CCamera* m_pCamera{};

public:
	// 플레이어의 위치를 xmf3Position 위치로 설정한다.
	// xmf3Position 벡터에서 현재 플레이어의 위치 벡터를 빼면 현재 플레이어의 위치에서 xmf3Position 방향으로의 벡터가 된다.
	// 현재 플레이어의 위치에서 이 벡터 만큼 이동한다.
	void SetPosition(XMFLOAT3& xmf3Position);
	XMFLOAT3 GetPosition();

	XMFLOAT3 GetLookVector();
	XMFLOAT3 GetUpVector();
	XMFLOAT3 GetRightVector();

	void SetFriction(float fFriction);
	void SetGravity(XMFLOAT3& xmf3Gravity);
	void SetMaxVelocityXZ(float fMaxVelocity);
	void SetMaxVelocityY(float fMaxVelocity);
	void SetVelocity(XMFLOAT3& xmf3Velocity);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	XMFLOAT3& GetVelocity();
	float GetYaw();
	float GetPitch();
	float GetRoll();

	void SetCamera(CCamera* pCamera);
	CCamera* GetCamera();

	// 플레이어를 이동하는 함수이다.
	void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);

	// 플레이어를 회전하는 함수이다.
	void Rotate(float x, float y, float z);

	// 플레이어의 위치와 회전 정보를 경과 시간에 따라 갱신하는 함수이다.
	void Update(float fTimeElapsed);
	
	// 플레이어의 위치가 바뀔 때마다 호출되는 함수와 그 함수에서 사용하는 정보를 설정하는 함수이다.
	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	void SetPlayerUpdatedContext(LPVOID pContext);

	// 카메라의 위치가 바뀔 때마다 호출되는 함수와 그 함수에서 사용하는 정보를 설정하는 함수이다.
	virtual void OnCameraUpdateCallback(float fTimeElapsed);
	void SetCameraUpdatedContext(LPVOID pContext);
	
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	
	// 카메라를 변경하기 위하여 호출하는 함수이다.
	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	// 플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수이다.
	virtual void OnPrepareRender();

	// 플레이어의 카메라가 3인칭 카메라일 때 플레이어(메쉬)를 렌더링한다.
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};

#define BULLETS					30


class CTerrainPlayer : public CPlayer
{
public:
	CTerrainPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int nMeshes = 1);
	virtual ~CTerrainPlayer();
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);
	float						m_fBulletEffectiveRange = 2000.0f;
	virtual void OnUpdateTransform();
	virtual void OnPrepareRender();
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	CBulletObject* m_ppBullets[BULLETS];

	void FireBullet(CGameObject* pSelectedObject);
};
