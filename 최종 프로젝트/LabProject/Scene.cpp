#include "stdafx.h"
#include "Player.h"
#include "Scene.h"
#include "Shader.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

CScene::CScene()
{

}

CScene::~CScene()
{

}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, void* pContext)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다.
	XMFLOAT3 xmf3Scale(8.0f, 2.0f, 8.0f);
	XMFLOAT4 xmf4Color(0.2f, 0.0f, 0.0f, 0.0f);
	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다.
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메
	쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("Resource/HeightMap.raw"), 257, 257, 17,
		17, xmf3Scale, xmf4Color);
#else
//지형을 하나의 격자 메쉬(257x257)로 생성한다.
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("Resource/HeightMap.raw"), 257, 257, 257,
		257, xmf3Scale, xmf4Color);
#endif
	m_nShaders = 1;
	m_pShaders = new CObjectsShader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pTerrain;
	float fTerrainWidth = pTerrain->GetWidth(), fTerrainLength = pTerrain->GetLength();
	float fxPitch = 12.0f * 21.f;
	float fyPitch = 12.0f * 21.f;
	float fzPitch = 12.0f * 21.f;
	//직육면체를 지형 표면에 그리고 지형보다 높은 위치에 일정한 간격으로 배치한다.
	int xObjects = int(fTerrainWidth / fxPitch), yObjects = 1, zObjects =
		int(fTerrainLength / fzPitch);
	m_nObjects = xObjects * yObjects * zObjects;
	m_ppObjects = new CGameObject * [m_nObjects];
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		24.0f, 50.0f, 1.0f);
	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CEnemyObject* pRotatingObject = NULL;
	
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CEnemyObject(1);
				pRotatingObject->SetMesh(0, pCubeMesh);
				float xPosition = x * fxPitch;
				float zPosition = z * fzPitch;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition)-20.0f;
				pRotatingObject->SetPosition(xPosition, fHeight + (y * 10.0f * fyPitch) +
					6.0f, zPosition);

				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(36.0f * (i % 10) + 36.0f);
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature)
	{
		m_pd3dGraphicsRootSignature->Release();
	}

	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].ReleaseShaderVariables();
		m_pShaders[i].ReleaseObjects();
	}

	if (m_pShaders)
	{
		delete[] m_pShaders;
	}

	if (m_pTerrain) delete m_pTerrain;

}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	if (gameover == false) {
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->Animate(fTimeElapsed);
			m_ppObjects[j]->UpdateBoundingBox();
		}

		m_pPlayer->UpdateBoundingBox();
		m_pPlayer->Animate(fTimeElapsed);
		CheckObjectByPlayer();
		CheckObjectByBullet();
	}
}
void CScene::CheckObjectByPlayer()
{	
	for (int i = 0; i < m_nObjects; i++) {
		if (m_ppObjects[i]->m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB) && m_ppObjects[i]->crashing)
		{
			m_ppObjects[i]->crashing = false;
			m_ppObjects[i]->m_xmf4x4World._11 = 0.0f;
			m_ppObjects[i]->m_xmf4x4World._22 = 0.0f;
			m_ppObjects[i]->m_xmf4x4World._33 = 0.0f;
			m_pPlayer->life -= 1;
		}
		if (m_pPlayer->life == 0) {
			PlaySound(NULL, 0, 0);
			PlaySound(TEXT("Resource/boom.wav"), 0, SND_FILENAME | SND_ASYNC);
			gameover = true;
			m_pPlayer->boom = true;
		}
	}
	
}

void CScene::CheckObjectByBullet()
{
	CBulletObject** ppBullets = ((CTerrainPlayer*)m_pPlayer)->m_ppBullets;
	for (int i = 0; i < m_nObjects; i++)
	{
		for (int j = 0; j < BULLETS; j++)
		{
			if (ppBullets[j]->m_bActive && m_ppObjects[i]->m_xmOOBB.Intersects(ppBullets[j]->m_xmOOBB))
			{
				m_ppObjects[i]->m_xmf4x4World._11 = 0.0f;
				m_ppObjects[i]->m_xmf4x4World._22 = 0.0f;
				m_ppObjects[i]->m_xmf4x4World._33 = 0.0f;
				ppBullets[j]->m_bActive = false;
			}
		}
	}
	
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	
		pCamera->SetViewportsAndScissorRects(pd3dCommandList);
		pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
		pCamera->UpdateShaderVariables(pd3dCommandList);
			if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
			for (int i = 0; i < m_nShaders; i++)
			{
				m_pShaders[i].Render(pd3dCommandList, pCamera);
			}

			for (int j = 0; j < m_nObjects; j++)
			{
				if (m_ppObjects[j] && m_ppObjects[j]->m_bActive)
				{
					m_ppObjects[j]->Render(pd3dCommandList, pCamera);
				}
			}
}

void CScene::ReleaseUploadBuffers()	
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].ReleaseUploadBuffers();
	}
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature{};
	D3D12_ROOT_PARAMETER pd3dRootParameters[2]{};

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |	// IA단계를 허용 
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |		// 헐 쉐이더가 접근할 수 없음
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |		// 도메인 쉐이더가 접근할 수 없음
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |	// 기하 쉐이더가 접근할 수 없음
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;		// 픽셀 쉐이더가 접근할 수 없음

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc{};

	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob{};
	ID3DBlob* pd3dErrorBlob{};

	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	
	if (pd3dSignatureBlob)
	{
		pd3dSignatureBlob->Release();
	}

	if (pd3dErrorBlob)
	{
		pd3dErrorBlob->Release();
	}

	return pd3dGraphicsRootSignature;
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return m_pd3dGraphicsRootSignature;
}
