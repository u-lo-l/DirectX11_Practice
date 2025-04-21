#include "framework.h"
#include "Camera.h"

Camera::Camera()
{
	Tf = new Transform();
	Proj = new Perspective(D3D::GetDesc().Width, D3D::GetDesc().Height, 0.1f, 1000.f, Math::Pi / 4);
}

Camera::~Camera()
{
	SAFE_DELETE(Tf);
	SAFE_DELETE(Proj);
}


void Camera::Tick()
{
	const float DeltaTime = sdt::SystemTimer::Get()->GetDeltaTime();
	Vector DeltaPosition;
	const Vector & Forward = Tf->GetForward();
	const Vector & Right = Tf->GetRight();
	const Vector & Up = Tf->GetUp();

	if (sdt::Mouse::Get()->IsPress(MouseButton::Right) == false)
	{
		return;
	}
	if (Keyboard::Get()->IsPressed('W') == true)
	{
		DeltaPosition += Forward * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed('S') == true)
	{
		DeltaPosition -= Forward * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed('D') == true)
	{
		DeltaPosition += Right * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed('A') == true)
	{
		DeltaPosition -= Right * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed('E') == true)
	{
		DeltaPosition += Up * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed('Q') == true)
	{
		DeltaPosition -= Up * MoveSpeed * DeltaTime;
	}
	if (Keyboard::Get()->IsPressed(VK_LSHIFT) == true)
	{
		MoveSpeed = Math::Clamp(MoveSpeed + 0.1f, DefaultMoveSpeed, MaxMoveSpeed);
	}
	else
	{
		MoveSpeed = DefaultMoveSpeed;
	}

	const Vector Delta = sdt::Mouse::Get()->GetMoveDelta();
	Vector Euler = Tf->GetRotationInDegree();
	Euler.X += Delta.Y * RotationSpeed * DeltaTime;
	Euler.Y += -Delta.X * RotationSpeed * DeltaTime;
	Euler.Z += 0;

	Tf->SetRotation(Euler);
	Tf->SetPosition(Tf->GetPosition() + DeltaPosition);
}

const Vector& Camera::GetPosition() const
{
	return Tf->GetPosition();
}

Matrix Camera::GetViewMatrix() const
{
	Matrix Mat = Tf->GetMatrix();
	Mat.Invert(true);
	return Mat;
}

Matrix Camera::GetProjectionMatrix() const
{
	return Proj->GetMatrix();
}

void Camera::SetPosition(float X, float Y, float Z) const
{
	Tf->SetPosition({X, Y, Z});
}

void Camera::SetPosition(const Vector& Vec) const
{
	Tf->SetPosition(Vec);
}

const Vector& Camera::GetEulerAngleInDegree() const
{
	return Tf->GetRotationInDegree();
}

const Vector& Camera::GetEulerAngleInRadian() const
{
	return Tf->GetRotationInRadian();
}

void Camera::SetRotation(float R, float P, float Y) const
{
	Tf->SetRotation({R, P, Y});
}

void Camera::SetRotation(const Vector & InEuler) const
{
	Tf->SetRotation(InEuler);
}

void Camera::SetMoveSpeed( float InSpeed )
{
	MoveSpeed = InSpeed;
}

void Camera::SetRopSpeed( float InSpeed )
{
	RotationSpeed = InSpeed;
}

void Camera::SetPerspective(float Width, float Height, float Near, float Far, float VFOV) const
{
	Proj->Set(Width, Height, Near, Far, VFOV);
}

Vector Camera::At() const
{
	return Tf->GetPosition() + Tf->GetForward();
}