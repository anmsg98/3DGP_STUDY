#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	}
	m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_fMovingSpeed = 0.0f;
	m_fMovingRange = 0.0f;

	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 0.05f;
}
CGameObject::~CGameObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}
void CGameObject::AddRef()
{ 
	m_nReferences++;
}

void CGameObject::Release()
{
	if (--m_nReferences <= 0) 
	{
		delete this;
	}
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}
}

void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}


void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader)
	{
		m_pShader->Release();
	}

	m_pShader = pShader;

	if (m_pShader)
	{
		m_pShader->AddRef();
	}
}

void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::Animate(float fTimeElapsed)
{
	
}

void CGameObject::OnPrepareRender()
{
	
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();
	UpdateShaderVariables(pd3dCommandList);
	if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
	//���� ��ü�� �����ϴ� ��� �޽��� �������Ѵ�.
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}


void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World{};

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	// ��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�.
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::ReleaseShaderVariables()
{

}

XMFLOAT3 CGameObject::GetPosition()
{
	return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);

}

XMFLOAT3 CGameObject::GetLook()
{
	// ���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�.
	return Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33));
}

XMFLOAT3 CGameObject::GetUp()
{
	// ���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�.
	return Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23));
}

XMFLOAT3 CGameObject::GetRight()
{
	// ���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�.
	return Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13));
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::MoveStrafe(float fDistance)
{
	// ���� ��ü�� ���� x-�� �������� �̵��Ѵ�. 
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();

	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	// ���� ��ü�� ���� y-�� �������� �̵��Ѵ�. 
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();

	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	// ���� ��ü�� ���� z-�� �������� �̵��Ѵ�.
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();

	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{

}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}



CEnemyObject::CEnemyObject(int nMeshes) : CGameObject(nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}

CEnemyObject::~CEnemyObject()
{

}

void CGameObject::SetRotationSpeed(float fRotationSpeed)
{ 
	m_fRotationSpeed = fRotationSpeed;
}

void CGameObject::SetRotationAxis(XMFLOAT3 xmf3RotationAxis)
{ 
	m_xmf3RotationAxis = xmf3RotationAxis;
}

void CEnemyObject::SetRotationSpeed(float fRotationSpeed)
{
	m_fRotationSpeed = fRotationSpeed;
}

void CEnemyObject::SetRotationAxis(XMFLOAT3 xmf3RotationAxis)
{
	m_xmf3RotationAxis = xmf3RotationAxis;
}

void CEnemyObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);

		m_fElapsedTimes += fTimeElapsed;
		if (m_fElapsedTimes >= 0.0f && m_fElapsedTimes <= 1.0f) {
			m_xmf4x4World._42 += 0.03f;
		}
		else if (m_fElapsedTimes >= 1.0f && m_fElapsedTimes <= 2.0f)
			;
		else if (m_fElapsedTimes >= 2.0f && m_fElapsedTimes <= 3.0f) {
			m_xmf4x4World._42 -= 0.03f;
		}
		else if (m_fElapsedTimes >= 3.0f)
			m_fElapsedTimes = -1.0f;
}

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int
	nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4
	xmf4Color) : CGameObject(0)
{
	//������ ����� ���� ���� ����, ������ ũ���̴�.
	m_nWidth = nWidth;
	m_nLength = nLength;
	/*���� ��ü�� ���� �޽����� �迭�� ���� ���̴�. nBlockWidth, nBlockLength�� ���� �޽� �ϳ��� ����, ���� ũ
	���̴�. cxQuadsPerBlock, czQuadsPerBlock�� ���� �޽��� ���� ����� ���� ���� �簢���� �����̴�.*/
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	//xmf3Scale�� ������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ����.
	m_xmf3Scale = xmf3Scale;
	//������ ����� ���� ���� �����Ѵ�.
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);
	//�������� ���� ����, ���� �������� ���� �޽��� �� ���� �ִ� ���� ��Ÿ����.
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;
	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� �����̴�.
	m_nMeshes = cxBlocks * czBlocks;
	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� ���� ������ �迭�� �����Ѵ�.
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = NULL;
	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� ���� ��ġ(��ǥ)�̴�.
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� �����Ͽ� ���� �޽��� �����Ѵ�.
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart,
			zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}
	//������ �������ϱ� ���� ���̴��� �����Ѵ�.
	CTerrainShader *pShader = new CTerrainShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}



void CGameObject::UpdateBoundingBox()
{
	if (m_ppMeshes)
	{
		m_ppMeshes[0]->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

CBulletObject::CBulletObject(float fEffectiveRange)
{
	m_fBulletEffectiveRange = fEffectiveRange;
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetFirePosition(XMFLOAT3 xmf3FirePosition)
{
	m_xmf3FirePosition = xmf3FirePosition;
	SetPosition(xmf3FirePosition);
}

void CBulletObject::Animate(float fElapsedTime)
{
	float fDistance = m_fMovingSpeed * fElapsedTime;
#ifdef _WITH_VECTOR_OPERATION
	XMFLOAT3 xmf3Position = GetPosition();

	m_fRotationAngle += m_fRotationSpeed * fElapsedTime;
	if (m_fRotationAngle > 360.0f) m_fRotationAngle = m_fRotationAngle - 360.0f;

	XMFLOAT4X4 mtxRotate1 = Matrix4x4::RotationYawPitchRoll(0.0f, m_fRotationAngle, 0.0f);

	XMFLOAT3 xmf3RotationAxis = Vector3::CrossProduct(m_xmf3RotationAxis, m_xmf3MovingDirection, true);
	float fDotProduct = Vector3::DotProduct(m_xmf3RotationAxis, m_xmf3MovingDirection);
	float fRotationAngle = ::IsEqual(fDotProduct, 1.0f) ? 0.0f : (float)XMConvertToDegrees(acos(fDotProduct));
	XMFLOAT4X4 mtxRotate2 = Matrix4x4::RotationAxis(xmf3RotationAxis, fRotationAngle);

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate1, mtxRotate2);

	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
#else
	m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, m_fRotationSpeed * fElapsedTime, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
#endif

	UpdateBoundingBox();

	if (Vector3::Distance(m_xmf3FirePosition, GetPosition()) > m_fBulletEffectiveRange) SetActive(false);
}

