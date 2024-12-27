#include "framework.h"
#include "Camera.h"

Camera::Camera()
{
	RotationMat = Matrix::Identity;
	ViewMat = Matrix::Identity;

	SetRotationMat();
	SetViewMat();
}

Camera::~Camera()
= default;


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
	EulerAngle.X += Delta.Y * RotationSpeed * DeltaTime;
	EulerAngle.Y += Delta.X * RotationSpeed * DeltaTime;
	EulerAngle.Z = 0;

	SetRotationMat();
}

const Vector& Camera::GetPosition() const
{
	return Position;
}

Matrix& Camera::GetViewMatrix()
{
	return ViewMat;
}

void Camera::SetPosition(float X, float Y, float Z)
{
	Position = { X, Y, Z };
	SetViewMat();
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

void Camera::SetRotation(float R, float P, float Y)
{
	EulerAngle = { R, P, Y };
	SetRotationMat();
}

void Camera::SetRotation(const Vector& InEuler)
{
	EulerAngle = InEuler;
	SetRotationMat();
}

void Camera::SetMoveSpeed( float InSpeed )
{
	MoveSpeed = InSpeed;
}

void Camera::SetRopSpeed( float InSpeed )
{
	RotationSpeed = InSpeed;
}

void Camera::SetViewMat()
{
	ViewMat = Matrix::CreateLookAt(Position, At(), Up);
}

void Camera::SetRotationMat()
{
	Matrix X, Y, Z;
	X = Matrix::CreateRotationX(EulerAngle.X);
	Y = Matrix::CreateRotationY(EulerAngle.Y);
	Z = Matrix::CreateRotationZ(EulerAngle.Z);

	RotationMat =  X * Y * Z;
	
	Forward = Vector::TransformNormal(Vector::Forward, RotationMat);
	Up = Vector::TransformNormal(Vector::Up, RotationMat);
	Right = Vector::TransformNormal(Vector::Right, RotationMat);
}

Vector Camera::At() const
{
	return Position + Forward;
}