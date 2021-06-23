#pragma once
#include "Timer.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	CPlayer* m_pPlayer = NULL;
	CGameObject** m_ppObjects = NULL;
protected:
	bool gameover = false;
	CHeightMapTerrain* m_pTerrain = NULL;
public:
	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }

protected:
	// 배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다.
	CObjectsShader* m_pShaders{};
	int m_nShaders{};
	int m_nObjects{};
	// 루트 시그너쳐를 나타내는 인터페이스 포인터이다.
	ID3D12RootSignature* m_pd3dGraphicsRootSignature{};

public:
	// 씬에서 마우스와 키보드 메시지를 처리한다.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void CheckObjectByPlayer();
	void CheckObjectByBullet();
	void ReleaseUploadBuffers();

	// 그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
};
