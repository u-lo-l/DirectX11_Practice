#include "framework.h"
#include "Camera.h"

const Vector Camera::WORLD_FORWARD = {0,0,1};
const Vector Camera::WORLD_UP = { 0,1,0 };
const Vector Camera::WORLD_RIGHT = { 1,0,0 };

Camera::Camera()
{
	D3DXMatrixIdentity(&RotationMat);
	D3DXMatrixIdentity(&ViewMat);

	SetRotationMat();
	SetViewMat();
}

Camera::~Camera()
= default;

Camera::Camera(const Camera& Other)
{
}

Camera::Camera(const Camera&& Other) noexcept
{
}

Camera& Camera::operator=(const Camera& Other)
{
	return *this;
}

void Camera::operator=(Camera&& Other) const noexcept
{
}

void Camera::Tick()
{
	const float DeltaTime = Sdt::Timer::Get()->GetDeltaTime();
	if (Sdt::Mouse::Get()->IsPress(MouseButton::Right) == false)
	{
		return;
	}
	if (Keyboard::Get()->IsPressed('W') == true)
	{
		Position += Forward * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed('S') == true)
	{
		Position -= Forward * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed('D') == true)
	{
		Position += Right * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed('A') == true)
	{
		Position -= Right * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed('E') == true)
	{
		Position += Up * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed('Q') == true)
	{
		Position -= Up * MoveSpeed * DeltaTime;
	}
	SetViewMat();

	const Vector Delta = Sdt::Mouse::Get()->GetMoveDelta();
	EulerAngle.x += Delta.y * RotationSpeed * DeltaTime;
	EulerAngle.y += Delta.x * RotationSpeed * DeltaTime;
	EulerAngle.z = 0;

	SetRotationMat();
}

const Vector& Camera::GetPosition() const
{
	return Position;
}

const Matrix& Camera::GetViewMatrix() const
{
	return ViewMat;
}

void Camera::SetPosition(float X, float Y, float Z)
{
	Position = { X, Y, Z };
}

void Camera::SetPosition(const Vector& Vec)
{
	Position = Vec;

	SetViewMat();
}

const Vector& Camera::GetEulerAngle() const
{
	return EulerAngle;
}

void Camera::SetRotation(float r, float p, float y)
{
	EulerAngle = { r, p, y };
}

void Camera::SetRotation(const Vector& InEuler)
{
	EulerAngle = InEuler;
	SetRotationMat();
}

void Camera::SetViewMat()
{
	const Vector At = this->At();
	D3DXMatrixLookAtLH(&ViewMat, &Position, &At, &Up);
}

void Camera::SetRotationMat()
{
	/***
	  DirectX의 D3DXMatrixRotationYawPitchRoll 함수는 Yaw, Pitch, Roll에 해당하는 회전을 한 번에 계산합니다.
	  Yaw: Y축 회전 (𝐸𝑢𝑙𝑒𝑟𝐴𝑛𝑔𝑙𝑒.𝑧)
	  Pitch: X축 회전 (𝐸𝑢𝑙𝑒𝑟𝐴𝑛𝑔𝑙𝑒.𝑦)
	  Roll: Z축 회전 (𝐸𝑢𝑙𝑒𝑟𝐴𝑛𝑔𝑙𝑒.𝑥)
	  일반적으로 Z축이 위를 가리키는 좌표계와, DX에서 Z축이 전방을 가리키는 좌표계 사이의 차를 고려한 회전이라고 여겨진다.
	  왜 roll pitch yaw를 이따구로 정의해놨는가...
	  일반적으로(항공) Yaw가 Z축 회전이고, 좌우(위로 향하는 축을 기준)로 회전한다.
	  Computer Graphics에서는 위를 향하는 축이 Y축이라 이를 Yaw축이라고 하는 듯 하다.
	 ***/
	D3DXMatrixRotationYawPitchRoll(&RotationMat, EulerAngle.y, EulerAngle.x, EulerAngle.z);
	D3DXVec3TransformNormal(&Forward, &Camera::WORLD_FORWARD, &RotationMat);
	D3DXVec3TransformNormal(&Up, &Camera::WORLD_UP, &RotationMat);
	D3DXVec3TransformNormal(&Right, &Camera::WORLD_RIGHT, &RotationMat);
}

const Vector& Camera::At() const
{
	return Position + Forward;
}