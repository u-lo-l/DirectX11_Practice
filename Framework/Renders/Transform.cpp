#include "framework.h"
#include "Transform.h"

Transform::Transform(Matrix * InMatrix, int InRegisterIndex)
	: Tf(InMatrix)
{
	CBuffer = new ConstantBuffer
	(
		ShaderType::ALL,
		InRegisterIndex,
		nullptr,
		"WorldTransform",
		sizeof(Matrix),
		false
	);

	if (!!InMatrix)
	{
		InMatrix->Decompose(Position, Rotation, Scale);
		EulerAngleInDegree = Rotation.ToEulerAnglesInDegrees();
		EulerAngleInRadian= Rotation.ToEulerAnglesInRadian();
		RotationMat = Matrix::CreateFromQuaternion(Rotation);
		UpdateMatrix();
	}
	else
	{
		bWorldTFAllocated = true;
		RotationMat = Matrix::Identity;
		Tf = new Matrix(
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		);
	}
}

Transform::~Transform()
{
	if (bWorldTFAllocated)
		SAFE_DELETE(Tf);
	SAFE_DELETE(CBuffer);
}

void Transform::Tick()
{
	ASSERT(!!Tf, "World Transform Not Assigned")
	Matrix World = GetWorldMatrix();
	CBuffer->UpdateData(&World, sizeof(Matrix));
}

void Transform::BindToGPU(const int InRegisterIndex) const
{
	CBuffer->BindToGPU(InRegisterIndex);
}

void Transform::SetWorldPosition(const Vector& InPosition)
{
	Position = InPosition;
	UpdateMatrix();
}

void Transform::SetWorldRotation(const Matrix& InRotation)
{
	Quaternion Quat = Quaternion::CreateFromRotationMatrix(InRotation);
	SetWorldRotation(Quat);
}

void Transform::SetWorldRotation(const Quaternion& InQuat)
{
	Rotation = InQuat;
	RotationMat = Matrix::CreateFromQuaternion(InQuat);
	EulerAngleInDegree = InQuat.ToEulerAnglesInDegrees();
	EulerAngleInRadian = InQuat.ToEulerAnglesInRadian();
	UpdateMatrix();
}

void Transform::SetWorldRotation(const Vector& ZYXEulerRadian)
{
	Quaternion Quat = Quaternion::CreateFromEulerAngleInRadian(ZYXEulerRadian);
	SetWorldRotation(Quat);
}

void Transform::SetScale(const Vector& InScale)
{
	Scale = InScale;
	UpdateMatrix();
}

Matrix Transform::GetWorldMatrix() const
{
	Matrix ParentWorld = (Parent == nullptr) ? Matrix::Identity : Parent->GetMatrix();
	return ParentWorld * (*Tf);
}

Matrix Transform::GetMatrix() const
{
	ASSERT(!!Tf, "World Transform Not Assigned")
	return *Tf;
}

const Matrix * Transform::GetMatrixRef() const
{
	ASSERT(!!Tf, "World Transform Not Assigned")
	return Tf;
}

const Vector & Transform::GetWorldPosition() const
{
	return Position;
}

const Matrix & Transform::GetWorldRotationMat() const
{
	return RotationMat;
}

const Quaternion & Transform::GetWorldRotation() const
{
	return Rotation;
}

const Vector & Transform::GetWorldZYXEulerAngle() const
{
	return EulerAngleInRadian;
}

const Vector& Transform::GetWorldZYXEulerAngleInDegree() const
{
	return EulerAngleInDegree;
}

const Vector & Transform::GetScale() const
{
	return Scale;
}

Vector Transform::GetForward() const
{
	ASSERT(!!Tf, "World Transform Not Assigned")
	return Vector::Normalize(Tf->Forward());
}

Vector Transform::GetUp() const
{
	ASSERT(!!Tf, "World Transform Not Assigned")
	return Vector::Normalize(Tf->Up());
}

Vector Transform::GetRight() const
{
	ASSERT(!!Tf, "World Transform Not Assigned")
	return Vector::Normalize(Tf->Right());
}

void Transform::AddWorldTranslation(const Vector& InTranslation)
{
	Position += InTranslation;
	UpdateMatrix();
}

void Transform::AddLocalTranslation(const Vector& InTranslation)
{
	Position += InTranslation.Z * RotationMat.Forward() + InTranslation.Y * RotationMat.Up() + InTranslation.X * RotationMat.Right();
	UpdateMatrix();
}

void Transform::AddWorldRotation(const Matrix& InRotationMat)
{
	const Quaternion DeltaRotation = Quaternion::CreateFromRotationMatrix(InRotationMat);
	AddWorldRotation(DeltaRotation);
}

void Transform::AddLocalRotation(const Matrix& InRotationMat)
{
	const Quaternion DeltaRotation = Quaternion::CreateFromRotationMatrix(InRotationMat);
	AddLocalRotation(DeltaRotation);
}

void Transform::AddWorldRotation(const Quaternion& InRotation)
{
	Rotation = InRotation * Rotation;
	EulerAngleInDegree = Rotation.ToEulerAnglesInDegrees();
	EulerAngleInRadian= Rotation.ToEulerAnglesInRadian();
	RotationMat = Matrix::CreateFromQuaternion(Rotation);
	UpdateMatrix();
}

void Transform::AddLocalRotation(const Quaternion& InRotation)
{
	Rotation = Rotation * InRotation;
	EulerAngleInDegree = Rotation.ToEulerAnglesInDegrees();
	EulerAngleInRadian= Rotation.ToEulerAnglesInRadian();
	RotationMat = Matrix::CreateFromQuaternion(Rotation);
	UpdateMatrix();
}

void Transform::AddWorldRotation(const Vector& InZYXEulerRadian)
{
	const Quaternion DeltaRotation = Quaternion::CreateFromEulerAngleInRadian(InZYXEulerRadian);
	AddWorldRotation(DeltaRotation);
}

void Transform::AddLocalRotation(const Vector& InZYXEulerRadian)
{
	const Quaternion DeltaRotation = Quaternion::CreateFromEulerAngleInRadian(InZYXEulerRadian);
	AddLocalRotation(DeltaRotation);
}

void Transform::AddScale(const Vector& InScale)
{
	Scale *= InScale;
	UpdateMatrix();
}

void Transform::SetTransform(const Matrix& InMatrix)
{
	Vector Scale;
	Quaternion Rotation;
	Vector Position;
	InMatrix.Decompose(Scale, Rotation, Position);

	this->SetTRS(Position, Rotation, Scale);
}

void Transform::AddTransform(const Matrix& InMatrix)
{
	Vector Translation;
	Quaternion Rotation;
	Vector Scale;
	InMatrix.Decompose(Scale, Rotation, Translation);
	Position += Translation;
	this->Rotation *= Rotation;
	RotationMat = Matrix::CreateFromQuaternion(this->Rotation);
	EulerAngleInDegree = Rotation.ToEulerAnglesInDegrees();
	EulerAngleInRadian = Rotation.ToEulerAnglesInRadian();
	this->Scale *= Scale;
	UpdateMatrix();
}

void Transform::AddTransform(const Transform& InTransform)
{
	AddTransform(InTransform.GetMatrix());
}

void Transform::AddTransform(const Transform* InTransform)
{
	AddTransform(InTransform->GetMatrix());
}

void Transform::SetTRS(const Vector& Pos, const Quaternion& Rot, const Vector& Scale)
{
	Position = Pos;
	Rotation = Rot;
	RotationMat = Matrix::CreateFromQuaternion(Rot);
	EulerAngleInDegree = Rotation.ToEulerAnglesInDegrees();
	EulerAngleInRadian = Rotation.ToEulerAnglesInRadian();
	this->Scale = Scale;
	UpdateMatrix();
}

void Transform::UpdateMatrix()
{
	ASSERT(!!Tf, "World Transform Not Assigned")
	const Matrix Translation = Matrix::CreateTranslation(Position);
	const Matrix Scale = Matrix::CreateScale(this->Scale);
	*Tf = Scale * RotationMat * Translation;
}
