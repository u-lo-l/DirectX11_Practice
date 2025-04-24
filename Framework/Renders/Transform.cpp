#include "framework.h"
#include "Transform.h"

Transform::Transform(Matrix * InMatrix)
	: WorldTF(InMatrix)
	, CBuffer(nullptr), Position{0,0,0}, EulerAngleInDegree{0,0,0}, EulerAngleInRadian{0,0,0}
	, Scale{1,1,1}
{
	if (!WorldTF)
	{
		bShouldWorldTFDeleted = true;
		WorldTF = new Matrix();
		*WorldTF = Matrix::Identity;
	}
	CBuffer = new ConstantBuffer(
		ShaderType::VertexShader,
		ShaderSlot::VS_World,
		WorldTF,
		"Transform World Mat",
		sizeof(Matrix),
		false
	);
}

Transform::~Transform()
{
	if (!bShouldWorldTFDeleted)
		SAFE_DELETE(WorldTF);
	SAFE_DELETE(CBuffer);
}

void Transform::Tick()
{
	CBuffer->UpdateData( WorldTF, sizeof(Matrix));
}

void Transform::BindToGPU()
{
	ASSERT(CBuffer != nullptr, "CBuffer Not Valid")
	CBuffer->BindToGPU();
}

Vector Transform::GetForward() const
{
	return WorldTF->Forward();
}

Vector Transform::GetUp() const
{
	return WorldTF->Up();
}

Vector Transform::GetRight() const
{
	return WorldTF->Right();
}

const Vector & Transform::GetPosition() const
{
	return Position;
}

const Vector & Transform::GetScale() const
{
	return Scale;
}

const Vector & Transform::GetRotationInRadian() const
{
	return EulerAngleInRadian;
}

const Vector & Transform::GetRotationInDegree() const
{
	return EulerAngleInDegree;
}

void Transform::SetPosition( const Vector & InPosition )
{
	Position = InPosition;
	UpdateWorldMatrix();
}

void Transform::SetRotation( const Vector & InEulerAngleInDegree )
{
	EulerAngleInDegree = InEulerAngleInDegree;
	EulerAngleInRadian = {Math::ToRadians(EulerAngleInDegree.X), Math::ToRadians(EulerAngleInDegree.Y), Math::ToRadians(EulerAngleInDegree.Z)};
	UpdateWorldMatrix();
}

void Transform::SetScale( const Vector & InScale )
{
	Scale = InScale;
	UpdateWorldMatrix();
}

void Transform::SetTRS( const Transform & InTransform )
{
	Position = InTransform.Position;
	EulerAngleInDegree = InTransform.EulerAngleInDegree;
	EulerAngleInRadian = InTransform.EulerAngleInRadian;
	Scale = InTransform.Scale;
	UpdateWorldMatrix();
}


void Transform::SetTRS( const Transform * InTransform )
{
	Position = InTransform->Position;
	EulerAngleInDegree = InTransform->EulerAngleInDegree;
	EulerAngleInRadian = InTransform->EulerAngleInRadian;
	Scale = InTransform->Scale;
	UpdateWorldMatrix();
}

void Transform::SetTRS( const Vector & InPosition, const Quaternion & InRotation, const Vector & InScale )
{
	Position = InPosition;
	Scale = InScale;
	EulerAngleInRadian = InRotation.ToEulerAnglesInRadian();
	EulerAngleInDegree = { Math::ToDegrees(EulerAngleInRadian.X), Math::ToDegrees(EulerAngleInRadian.Y), Math::ToDegrees(EulerAngleInRadian.Z)};
	UpdateWorldMatrix();
}

void Transform::UpdateWorldMatrix()
{
	const Matrix Translation = Matrix::CreateTranslation(Position);
	const Matrix Rotation = Matrix::CreateFromYawPitchRoll(EulerAngleInRadian.Z, EulerAngleInRadian.Y, EulerAngleInRadian.X);
	const Matrix Scale = Matrix::CreateScale(this->Scale);

	*WorldTF = Scale * Rotation * Translation;
}
