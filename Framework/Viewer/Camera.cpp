#include "framework.h"
#include "Camera.h"

Camera::Camera()
{
	Tf = new Transform();
	Proj = new Perspective(D3D::GetDesc().Width, D3D::GetDesc().Height, 0.1f, 10000.f, Math::Pi / 4);
	ViewFrustum = new Frustum(
		Proj->GetAspect(),
		Proj->GetFOV(),
		Proj->GetNear(),
		Proj->GetFar(),
		Tf->GetWorldPosition(),
		Tf->GetForward(),
		Tf->GetUp(),
		Tf->GetRight()
	);
}

Camera::~Camera()
{
	SAFE_DELETE(Tf);
	SAFE_DELETE(Proj);
	SAFE_DELETE(ViewFrustum);
}

void Camera::Tick()
{
	const float DeltaTime = sdt::SystemTimer::Get()->GetDeltaTime();
	Vector DeltaPosition = {0, 0, 0};

	if (sdt::Mouse::Get()->IsPress(MouseButton::Right) == false)
	{
		return;
	}
	if (Keyboard::Get()->IsPressed('W') == true)
	{
		DeltaPosition += Vector::Forward;
	}
	if (Keyboard::Get()->IsPressed('S') == true)
	{
		DeltaPosition -= Vector::Forward;
	}
	if (Keyboard::Get()->IsPressed('D') == true)
	{
		DeltaPosition += Vector::Right;
	}
	if (Keyboard::Get()->IsPressed('A') == true)
	{
		DeltaPosition -= Vector::Right;
	}
	if (Keyboard::Get()->IsPressed('E') == true)
	{
		DeltaPosition += Vector::Up;
	}
	if (Keyboard::Get()->IsPressed('Q') == true)
	{
		DeltaPosition -= Vector::Up;
	}
	if (Keyboard::Get()->IsPressed(VK_LSHIFT) == true)
	{
		MoveSpeed = Math::Clamp(MoveSpeed + 0.1f, DefaultMoveSpeed, MaxMoveSpeed);
	}
	else
	{
		MoveSpeed = DefaultMoveSpeed;
	}
	DeltaPosition *= MoveSpeed * DeltaTime;
	const Vector & Delta = sdt::Mouse::Get()->GetMoveDelta();
	const float Roll = Delta.Y * RotationSpeed * DeltaTime;
	const float Pitch = Delta.X * RotationSpeed * DeltaTime;
	Quaternion QuatY = Quaternion::CreateFromAxisAngle(Vector::Up, Pitch);
	Quaternion QuatX = Quaternion::CreateFromAxisAngle(Vector::Right, Roll);
	
	Tf->AddWorldRotation(QuatY);
	Tf->AddLocalRotation(QuatX);
	Tf->AddLocalTranslation(DeltaPosition);
	
	ViewFrustum->UpdateTransform(
		Tf->GetWorldPosition(),
		Tf->GetForward(),
		Tf->GetUp(),
		Tf->GetRight()
	);
}

const Vector& Camera::GetPosition() const
{
	return Tf->GetWorldPosition();
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
	Tf->SetWorldPosition({X, Y, Z});
	ViewFrustum->UpdateTransform(Tf->GetWorldPosition(), Tf->GetForward(), Tf->GetUp(), Tf->GetRight());
}

void Camera::SetPosition(const Vector& Vec) const
{
	Tf->SetWorldPosition(Vec);
	ViewFrustum->UpdateTransform(Tf->GetWorldPosition(), Tf->GetForward(), Tf->GetUp(), Tf->GetRight());
}

const Vector& Camera::GetEulerAngleInDegree() const
{
	return Tf->GetWorldZYXEulerAngleInDegree();
}

const Vector& Camera::GetEulerAngleInRadian() const
{
	return Tf->GetWorldZYXEulerAngle();
}

void Camera::SetRotation(float R, float P, float Y) const
{
	Vector EulerDegree = {R, P, Y};
	Tf->SetWorldRotation(EulerDegree * Math::DegToRadian);
	ViewFrustum->UpdateTransform(Tf->GetWorldPosition(), Tf->GetForward(), Tf->GetUp(), Tf->GetRight());
}

void Camera::SetRotation(const Vector & InEulerRadian) const
{
	Tf->SetWorldRotation(InEulerRadian);
	ViewFrustum->UpdateTransform(Tf->GetWorldPosition(), Tf->GetForward(), Tf->GetUp(), Tf->GetRight());
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
	ViewFrustum->UpdateShape(
		Proj->GetAspect(),
		Proj->GetFOV(),
		Proj->GetNear(),
		Proj->GetFar(),
		Tf->GetWorldPosition(),
		Tf->GetForward(),
		Tf->GetUp(),
		Tf->GetRight()
	);
}

const Frustum* Camera::GetViewFrustum() const
{
	return ViewFrustum;
}

const Projection* Camera::GetProjection() const
{
	return Proj;
}

Vector Camera::GetForward() const
{
	return Tf->GetForward();
}

Vector Camera::GetRight() const
{
	return Tf->GetRight();
}

Vector Camera::GetUp() const
{
	return Tf->GetUp();
}

Vector Camera::At() const
{
	return Tf->GetWorldPosition() + Tf->GetForward();
}
