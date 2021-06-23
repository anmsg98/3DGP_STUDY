#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

CPlayer::CPlayer(int nMeshes) : CGameObject(nMeshes)
{
	m_pCamera = NULL;

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera)
	{
		delete m_pCamera;
	}
}

void CPlayer::SetPosition(XMFLOAT3& xmf3Position)
{
	Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false);
}

XMFLOAT3 CPlayer::GetPosition()
{
	return m_xmf3Position;
}

XMFLOAT3 CPlayer::GetLookVector()
{ 
	return m_xmf3Look;
}

XMFLOAT3 CPlayer::GetUpVector()
{
	return m_xmf3Up;
}

XMFLOAT3 CPlayer::GetRightVector()
{ 
	return m_xmf3Right;
}

void CPlayer::SetFriction(float fFriction)
{ 
	m_fFriction = fFriction;
}

void CPlayer::SetGravity(XMFLOAT3& xmf3Gravity)
{ 
	m_xmf3Gravity = xmf3Gravity;
}

void CPlayer::SetMaxVelocityXZ(float fMaxVelocity)
{ 
	m_fMaxVelocityXZ = fMaxVelocity; 
}

void CPlayer::SetMaxVelocityY(float fMaxVelocity)
{ 
	m_fMaxVelocityY = fMaxVelocity; 
}

void CPlayer::SetVelocity(XMFLOAT3& xmf3Velocity)
{ 
	m_xmf3Velocity = xmf3Velocity;
}

XMFLOAT3& CPlayer::GetVelocity()
{ 
	return m_xmf3Velocity;
}

float CPlayer::GetYaw()
{ 
	return m_fYaw;
}

float CPlayer::GetPitch()
{ 
	return m_fPitch;
}

float CPlayer::GetRoll()
{
	return m_fRoll;
}

void CPlayer::SetCamera(CCamera* pCamera)
{ 
	m_pCamera = pCamera;
}

CCamera* CPlayer::GetCamera()
{ 
	return m_pCamera; 
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	// �÷��̾��� ��ġ�� �����ϴ� �Լ��̴�.
	// �÷��̾��� ��ġ�� �⺻������ ����ڰ� �÷��̾ �̵��ϱ� ���� Ű���带 ���� �� ����ȴ�.
	// �÷��̾��� �̵� ����(dwDirection)�� ���� �÷��̾ fDistance ��ŭ �̵��Ѵ�.
	
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift{};

		// ȭ��ǥ Ű ���衯�� ������ ���� z-�� �������� �̵�(����)�Ѵ�. ���顯�� ������ �ݴ� �������� �̵��Ѵ�.
		if (dwDirection & DIR_FORWARD)
		{
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		}

		if (dwDirection & DIR_BACKWARD)
		{
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		}

		// ȭ��ǥ Ű ���桯�� ������ ���� x-�� �������� �̵��Ѵ�. ���硯�� ������ �ݴ� �������� �̵��Ѵ�.
		if (dwDirection & DIR_RIGHT)
		{
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		}

		if (dwDirection & DIR_LEFT)
		{
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		}

		// ��Page Up���� ������ ���� y-�� �������� �̵��Ѵ�. ��Page Down���� ������ �ݴ� �������� �̵��Ѵ�.
		if (dwDirection & DIR_UP)
		{
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		}

		if (dwDirection & DIR_DOWN)
		{
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);
		}

		if (boom == false) {
			Move(xmf3Shift, bUpdateVelocity);
		}
		
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	// bUpdateVelocity�� true�̸� �÷��̾ �̵����� �ʰ� �ӵ� ���͸� �����Ѵ�.
	if (bUpdateVelocity)
	{
		// �÷��̾��� �ӵ� ���͸� xmf3Shift ���͸�ŭ �����Ѵ�.
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		// �÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�.
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);

		// �÷��̾��� ��ġ�� ����Ǿ����Ƿ� ī�޶��� ��ġ�� xmf3Shift ���͸�ŭ �̵��Ѵ�.
		if (m_pCamera)
		{
				m_pCamera->Move(xmf3Shift);
		}
	}
}

void CPlayer::Move(float fxOffset, float fyOffset, float fzOffset)
{

}

void CPlayer::Rotate(float x, float y, float z)
{
	// �÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�.
	DWORD nCameraMode = m_pCamera->GetMode();

	// 1��Ī ī�޶� �Ǵ� 3��Ī ī�޶��� ��� �÷��̾��� ȸ���� �ణ�� ������ ������.
	if ((nCameraMode == FIRST_PERSON_CAMERA) || (nCameraMode == THIRD_PERSON_CAMERA))
	{
		// ���� x-���� �߽����� ȸ���ϴ� ���� ���� �յڷ� ���̴� ���ۿ� �ش��Ѵ�.
		// �׷��Ƿ� x-���� �߽����� ȸ���ϴ� ������ -89.0~+89.0�� ���̷� �����Ѵ�.
		// x�� ������ m_fPitch���� ���� ȸ���ϴ� �����̹Ƿ� x��ŭ ȸ���� ���� 
		// Pitch�� +89�� ���� ũ�ų� -89�� ���� ������ m_fPitch�� +89�� �Ǵ� -89���� �ǵ��� ȸ������(x)�� �����Ѵ�.
		if (x != 0.0f)
		{
			m_fPitch += x;

			if (m_fPitch > +89.0f)
			{ 
				x -= (m_fPitch - 89.0f);
				m_fPitch = +89.0f;
			}
			
			if (m_fPitch < -89.0f)
			{ 
				x -= (m_fPitch + 89.0f);
				m_fPitch = -89.0f;
			}
		}

		if (y != 0.0f)
		{
			// ���� y-���� �߽����� ȸ���ϴ� ���� ������ ������ ���̹Ƿ� ȸ�� ������ ������ ����.
			m_fYaw += y;

			if (m_fYaw > 360.0f)
			{
				m_fYaw -= 360.0f;
			}

			if (m_fYaw < 0.0f)
			{
				m_fYaw += 360.0f;
			}
		}

		if (z != 0.0f)
		{
			// ���� z-���� �߽����� ȸ���ϴ� ���� ������ �¿�� ����̴� ���̹Ƿ� ȸ�� ������ -20.0~+20.0�� ���̷� ���ѵȴ�.
			// z�� ������ m_fRoll���� ���� ȸ���ϴ� �����̹Ƿ� z��ŭ ȸ���� ����
			// m_fRoll�� +20�� ���� ũ�ų� -20������ ������ m_fRoll�� +20�� �Ǵ� -20���� �ǵ��� ȸ������(z)�� �����Ѵ�.
			m_fRoll += z;

			if (m_fRoll > +20.0f)
			{ 
				z -= (m_fRoll - 20.0f); 
				m_fRoll = +20.0f;
			}

			if (m_fRoll < -20.0f)
			{
				z -= (m_fRoll + 20.0f);
				m_fRoll = -20.0f;
			}
		}

		// ī�޶� x, y, z ��ŭ ȸ���Ѵ�. �÷��̾ ȸ���ϸ� ī�޶� ȸ���ϰ� �ȴ�.
		m_pCamera->Rotate(x, y, z);

		// �÷��̾ ȸ���Ѵ�. 1��Ī ī�޶� �Ǵ� 3��Ī ī�޶󿡼� �÷��̾��� ȸ���� ���� y-�࿡���� �Ͼ��.
		// �÷��̾��� ���� y-��(Up ����)�� �������� ���� z-��(Look ����)�� ���� x-��(Right ����)�� ȸ����Ų��.
		// �⺻������ Up ���͸� �������� ȸ���ϴ� ���� �÷��̾ �ȹٷ� ���ִ� ���� �����Ѵٴ� �ǹ��̴�.
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));

			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCameraMode == SPACESHIP_CAMERA)
	{
		// �����̽�-�� ī�޶󿡼� �÷��̾��� ȸ���� ȸ�� ������ ������ ����. �׸��� ��� ���� �߽����� ȸ���� �� �� �ִ�.
		m_pCamera->Rotate(x, y, z);

		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));

			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}

		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));

			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));

			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	// ȸ������ ���� �÷��̾��� ���� x-��, y-��, z-���� ���� �������� ���� �� �����Ƿ�
	// z-��(Look ����)�� ���������Ͽ� ���� �����ϰ� �������Ͱ� �ǵ��� �Ѵ�.
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float fTimeElapsed)
{
	// �� �Լ��� �� �����Ӹ��� ȣ��ȴ�. �÷��̾��� �ӵ� ���Ϳ� �߷°� ������ ���� �����Ѵ�. 
	// �÷��̾��� �ӵ� ���͸� �߷� ���Ϳ� ���Ѵ�.
	// �߷� ���Ϳ� fTimeElapsed�� ���ϴ� ���� �߷��� �ð��� ����ϵ��� �����Ѵٴ� �ǹ��̴�.
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	
	// �÷��̾��� �ӵ� ������ xz-������ ũ�⸦ ���Ѵ�.
	// �̰��� XZ-����� �ִ� �ӷº��� ũ�� �ӵ� ������ x�� z-���� ������ �����Ѵ�.
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;

	/*if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}*/

	// �÷��̾��� �ӵ� ������ y-������ ũ�⸦ ���Ѵ�.
	// �̰��� y-�� ������ �ִ� �ӷº��� ũ�� �ӵ� ������ y-���� ������ �����Ѵ�
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;

	if (fLength > m_fMaxVelocityY)
	{
		m_xmf3Velocity.y *= (fMaxVelocityY / fLength);
	}

	// �÷��̾ �ӵ� ���� ��ŭ ������ �̵��Ѵ�(ī�޶� �̵��� ���̴�). 
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);

	Move(xmf3Velocity, false);

	// �÷��̾��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�.
	// �÷��̾��� ���ο� ��ġ�� ��ȿ�� ��ġ�� �ƴ� ���� �ְ� �Ǵ� �÷��̾��� �浹 �˻� ���� ������ �ʿ䰡 �ִ�.
	// �̷��� ��Ȳ���� �÷��̾��� ��ġ�� ��ȿ�� ��ġ�� �ٽ� ������ �� �ִ�.
	if (m_pPlayerUpdatedContext)
	{
		OnPlayerUpdateCallback(fTimeElapsed);
	}

	DWORD nCameraMode = m_pCamera->GetMode();

	// �÷��̾��� ��ġ�� ����Ǿ����Ƿ� 3��Ī ī�޶� �����Ѵ�.
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		m_pCamera->Update(m_xmf3Position, fTimeElapsed);
	}

	// ī�޶��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�.
	if (m_pCameraUpdatedContext)
	{
		OnCameraUpdateCallback(fTimeElapsed);
	}

	// ī�޶� 3��Ī ī�޶��̸� ī�޶� ����� �÷��̾� ��ġ�� �ٶ󺸵��� �Ѵ�.
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		m_pCamera->SetLookAt(m_xmf3Position);
	}

	// ī�޶��� ī�޶� ��ȯ ����� �ٽ� �����Ѵ�.
	m_pCamera->RegenerateViewMatrix();

	// �÷��̾��� �ӵ� ���Ͱ� ������ ������ ������ �Ǿ�� �Ѵٸ� ���� ���͸� �����Ѵ�.
	// �ӵ� ������ �ݴ� ���� ���͸� ���ϰ� ���� ���ͷ� �����.
	// ���� ����� �ð��� ����ϵ��� �Ͽ� �������� ���Ѵ�.
	// ���� ���Ϳ� �������� ���Ͽ� ���� ���͸� ���Ѵ�.
	// �ӵ� ���Ϳ� ���� ���͸� ���Ͽ� �ӵ� ���͸� ���δ�.
	// �������� �ӷº��� ũ�� �ӷ��� 0�� �� ���̴�.
	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);

	if (fDeceleration > fLength)
	{
		fDeceleration = fLength;
	}

	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{

}

void CPlayer::SetPlayerUpdatedContext(LPVOID pContext)
{
	m_pPlayerUpdatedContext = pContext;
}


void CPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{

}

void CPlayer::SetCameraUpdatedContext(LPVOID pContext) 
{ 
	m_pCameraUpdatedContext = pContext;
}

void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	if (m_pCamera)
	{
		m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	}
}

void CPlayer::ReleaseShaderVariables()
{
	CGameObject::ReleaseShaderVariables();

	if (m_pCamera)
	{
		m_pCamera->ReleaseShaderVariables();
	}
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
}

CCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	// ī�޶� ������ �� ChangeCamera() �Լ����� ȣ��Ǵ� �Լ��̴�.
	// nCurrentCameraMode�� ���� ī�޶��� ����̰� nNewCameraMode�� ���� ������ ī�޶� ����̴�.
	// ���ο� ī�޶��� ��忡 ���� ī�޶� ���� �����Ѵ�.
	CCamera* pNewCamera{};

	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}

	// ���� ī�޶��� ��尡 �����̽�-�� ����� ī�޶��̰� ���ο� ī�޶� 1��Ī �Ǵ� 3��Ī ī�޶��̸�
	// �÷��̾��� Up ���͸� ������ǥ���� y-�� ���� ����(0, 1, 0)�� �ǵ��� �Ѵ�. ��, �ȹٷ� ������ �Ѵ�.
	// �׸��� �����̽�-�� ī�޶��� ��� �÷��̾��� �̵����� ������ ����. Ư��, y-�� ������ �������� �����Ӵ�.
	// �׷��Ƿ� �÷��̾��� ��ġ�� ����(��ġ ������ y-��ǥ�� 0���� ũ��)�� �� �� �ִ�.
	// �̶� ���ο� ī�޶� 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾��� ��ġ�� ������ �Ǿ�� �Ѵ�.
	// �׷��Ƿ� �÷��̾��� Right ���Ϳ� Look ������ y ���� 0���� �����.
	// ���� �÷��̾��� Right ���Ϳ� Look ���ʹ� �������Ͱ� �ƴϹǷ� ����ȭ�Ѵ�.
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;

		// Look ���Ϳ� ������ǥ���� z-��(0, 0, 1)�� �̷�� ����(����=cos)�� ����Ͽ� �÷��̾��� y-���� ȸ�� ���� m_fYaw�� �����Ѵ�.
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);

		if (m_xmf3Look.x < 0.0f)
		{
			m_fYaw = -m_fYaw;
		}
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		// ���ο� ī�޶��� ��尡 �����̽�-�� ����� ī�޶��̰� ���� ī�޶� ��尡 1��Ī �Ǵ� 3��Ī ī�޶��̸�
		// �÷��̾��� ���� ���� ���� ī�޶��� ���� ��� ���� �����.
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);

		// ���� ī�޶� ����ϴ� �÷��̾� ��ü�� �����Ѵ�.
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera)
	{
		delete m_pCamera;
	}

	return pNewCamera;
}

CCamera* CPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	return NULL;
}

void CPlayer::OnPrepareRender()
{
	// �÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ��̴�.
	// �÷��̾��� Right ���Ͱ� ���� ��ȯ ��� �� ù ��° �� ����, Up ���Ͱ� �� ��° �� ����, Look ���Ͱ� �� ��° �� ����, 
	// �÷��̾��� ��ġ ���Ͱ� �� ��° �� ���Ͱ� �ȴ�.
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	
	// ī�޶� ��尡 3��Ī�̸� �÷��̾� ��ü�� �������Ѵ�.
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		if (m_pShader)
		{
			m_pShader->Render(pd3dCommandList, pCamera);
		}

		CGameObject::Render(pd3dCommandList, pCamera);
	}
	
}

void CPlayer::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}


void CTerrainPlayer::OnUpdateTransform()
{
		m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
		m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
		m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
		m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
}

void CTerrainPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (boom == false) {
		CPlayer::Render(pd3dCommandList, pCamera);

		for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTerrainPlayer::FireBullet(CGameObject* pSelectedObject)
{
	if (pSelectedObject) LookAt(pSelectedObject->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));

	OnUpdateTransform();

	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < BULLETS; i++)
	{
		if (!m_ppBullets[i]->m_bActive)
		{
			pBulletObject = m_ppBullets[i];
			break;
		}
	}
	if (pBulletObject)
	{
		XMFLOAT3 xmf3Position = GetPosition();
		XMFLOAT3 xmf3Direction = GetUp();
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 6.0f, false));

		//		pBulletObject->LookAt(xmf3FirePosition, XMFLOAT3(0.0f, 1.0f, 0.0f));

		pBulletObject->m_xmf4x4World = m_xmf4x4World;

		pBulletObject->m_xmf4x4World._41 = xmf3FirePosition.x;
		pBulletObject->m_xmf4x4World._42 = xmf3FirePosition.y;
		pBulletObject->m_xmf4x4World._43 = xmf3FirePosition.z;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetActive(true);
	}
}

CTerrainPlayer::CTerrainPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int
	nMeshes) : CPlayer(nMeshes)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	//�÷��̾��� ��ġ�� ������ ���(y-�� ��ǥ�� ������ ���̺��� 1500 ����)�� �����Ѵ�. �÷��̾� ��ġ ������ y��ǥ�� ������ ���̺��� ũ�� �߷��� �ۿ��ϵ��� �÷��̾ �����Ͽ����Ƿ� �÷��̾�� ���������� �ϰ��ϰ� �ȴ�.*/
	float fHeight = pTerrain->GetHeight(pTerrain->GetWidth() * 0.5f,
		pTerrain->GetLength() * 0.5f);
	SetPosition(XMFLOAT3(pTerrain->GetWidth() * 0.5f, fHeight+100.0f,
		pTerrain->GetLength() * 0.5f));
	//�÷��̾��� ��ġ�� ����� �� ������ ������ ���� �÷��̾��� ��ġ�� ������ �� �ֵ��� �����Ѵ�.
	SetPlayerUpdatedContext(pTerrain);
	//ī�޶��� ��ġ�� ����� �� ������ ������ ���� ī�޶��� ��ġ�� ������ �� �ֵ��� �����Ѵ�.
	SetCameraUpdatedContext(pTerrain);
	CMesh* pAirplaneMesh = new CMesh(pd3dDevice, pd3dCommandList, "Resource/FlyerPlayership.txt", true);
	//CAirplaneMeshDiffused* pAirplaneMesh = new CAirplaneMeshDiffused(pd3dDevice, pd3dCommandList, 20.0f, 10.0f, 4.0f, RED_COLOR);
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	SetMesh(0, pAirplaneMesh);
	
	//�÷��̾ �������� ���̴��� �����Ѵ�.
	
	CCubeMeshDiffused* pBulletMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 4.0f, 4.0f, 4.0f);
	for (int i = 0; i < BULLETS; i++) 
	{
		m_ppBullets[i] = new CBulletObject(m_fBulletEffectiveRange);
		m_ppBullets[i]->SetMesh(0, pBulletMesh);
		XMFLOAT3 Up = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_ppBullets[i]->SetRotationAxis(Up);
		XMFLOAT3 SetPos = XMFLOAT3(pTerrain->GetWidth() * 0.5f, fHeight, pTerrain->GetLength() * 0.5f);
		m_ppBullets[i]->SetPosition(SetPos);
		m_ppBullets[i]->SetRotationSpeed(360.0f);
		m_ppBullets[i]->SetMovingSpeed(1000.0f);
		m_ppBullets[i]->SetActive(false);
	}
	
	CPlayerShader* pShader = new CPlayerShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetShader(pShader);
}

CTerrainPlayer::~CTerrainPlayer()
{
	for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]) delete m_ppBullets[i];
}

void CTerrainPlayer::Animate(float fTimeElapsed)
{
	for (int i = 0; i < BULLETS; i++)
	{
		m_ppBullets[i]->Animate(fTimeElapsed);
	}
}
void CTerrainPlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();
	m_xmf4x4World._11 = m_xmf4x4World._11 * 10;
	m_xmf4x4World._22 = m_xmf4x4World._22 * 10;
	m_xmf4x4World._33 = m_xmf4x4World._33 * 10;
}
CCamera* CTerrainPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		//3��Ī ī�޶��� �� �÷��̾ y-�� �������� �߷��� �ۿ��Ѵ�.
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -200.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);
	return(m_pCamera);
}

void CTerrainPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pPlayerUpdatedContext;
	/*�������� �÷��̾��� ���� ��ġ (x, z)�� ���� ����(y)�� ���Ѵ�. �׸��� �÷��̾� �޽��� ���̰� 12�̰� �÷��̾���
	�߽��� ������ü�� ����̹Ƿ� y ���� �޽��� ������ ������ ���ϸ� �÷��̾��� ��ġ�� �ȴ�.*/
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) +
		6.0f;
	/*�÷��̾��� ��ġ ������ y-���� �����̸�(���� ���, �߷��� ����Ǵ� ���) �÷��̾��� ��ġ ������ y-���� ����
	�۾����� �ȴ�. �̶� �÷��̾��� ���� ��ġ ������ y ���� ������ ����(������ ������ ���� + 6)���� ������ �÷��̾�
	�� �Ϻΰ� ���� �Ʒ��� �ְ� �ȴ�. �̷��� ��츦 �����Ϸ��� �÷��̾��� �ӵ� ������ y ���� 0���� ����� �÷��̾�
	�� ��ġ ������ y-���� ������ ����(������ ������ ���� + 6)�� �����Ѵ�. �׷��� �÷��̾�� �׻� ���� ���� �ְ� ��
	��.*/
	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}
}

void CTerrainPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	/*���� �ʿ��� ī�޶��� ���� ��ġ (x, z)�� ���� ������ ����(y ��)�� ���Ѵ�. �� ���� ī�޶��� ��ġ ������ y-�� ��
	�� ũ�� ī�޶� ������ �Ʒ��� �ְ� �ȴ�. �̷��� �Ǹ� ���� �׸��� ���ʰ� ���� ������ �׷����� �ʴ� ��찡 �߻�
	�Ѵ�(ī�޶� ���� �ȿ� �����Ƿ� �ﰢ���� ���ε� ������ �ٲ��). �̷��� ��찡 �߻����� �ʵ��� ī�޶��� ��ġ ��
	���� y-���� �ּҰ��� (������ ���� + 5)�� �����Ѵ�. ī�޶��� ��ġ ������ y-���� �ּҰ��� ������ ��� ��ġ����
	ī�޶� ���� �Ʒ��� ��ġ���� �ʵ��� �����ؾ� �Ѵ�.*/
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pCameraUpdatedContext;
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) +
		5.0f;
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			//3��Ī ī�޶��� ��� ī�޶� ��ġ(y-��ǥ)�� ����Ǿ����Ƿ� ī�޶� �÷��̾ �ٶ󺸵��� �Ѵ�.
			CThirdPersonCamera *p3rdPersonCamera = (CThirdPersonCamera *)m_pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}
}