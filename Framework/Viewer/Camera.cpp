#include "framework.h"
#include "Camera.h"

Camera::Camera()
{
	Tf = new Transform();
	Proj = new Perspective(D3D::GetDesc().Width, D3D::GetDesc().Height, 0.1f, 10000.f, Math::Pi / 4);
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

	static float Roll = 0.f;
	static float Pitch = 0.f;
	static float Yaw = 0.f;
	ImGui::SliderFloat("RollTEST", &Roll, -Math::Pi, Math::Pi, "%.2f");
	ImGui::SliderFloat("PitchTEST", &Pitch, -Math::Pi * 0.45f, Math::Pi * 0.45f, "%.2f");
	ImGui::SliderFloat("YawTEST", &Yaw, -Math::Pi, Math::Pi, "%.2f");

	if (sdt::Mouse::Get()->IsPress(MouseButton::Right) == false)
	{
		Vector Euler = Tf->GetRotationInRadian();
		const Matrix Rotation = Matrix::CreateFromEulerAngleInRadian({30 * Math::DegToRadian, 0, 0});
		Matrix DeltaRotation = Matrix::CreateFromEulerAngleInRadian({Roll, Pitch, Yaw});
		const Matrix DeltaYaw = Matrix::CreateRotationZ(Yaw); 
		const Matrix DeltaPitch = Matrix::CreateRotationY(Pitch); 
		const Matrix DeltaRoll = Matrix::CreateRotationX(Roll); 
		Tf->SetRotation(Rotation * DeltaYaw * DeltaPitch * DeltaRoll);
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
	// const Vector Delta = sdt::Mouse::Get()->GetMoveDelta() * DeltaTime * 1;
	// Vector Euler = Tf->GetRotationInRadian();
	// const Matrix Rotation = Matrix::CreateFromEulerAngleInRadian(Euler);
	// Matrix DeltaRotation = Matrix::CreateFromEulerAngleInRadian({Roll, Pitch, Yaw});
	// Tf->SetRotation(Rotation * DeltaRotation);
	Tf->SetPosition(Tf->GetPosition() + DeltaPosition);
}

const Vector& Camera::GetPosition() const
{
	return Tf->GetPosition();
}

Matrix Camera::GetViewMatrix() const
{
	return Matrix::Invert(Tf->GetMatrix(), true);
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