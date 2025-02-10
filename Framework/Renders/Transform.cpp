#include "framework.h"
#include "Transform.h"

#ifdef DO_DEBUG
Transform::Transform( const string & MetaData )
: CBuffer(nullptr), ECB_CBuffer(nullptr)
, Position{0,0,0}, EulerAngleInDegree{0,0,0}, EulerAngleInRadian{0,0,0}, Scale{1,1,1}
{
	CBufferData.World= Matrix::Identity;
	CBuffer = new ConstantBuffer(&CBufferData, MetaData, sizeof(ThisClass::CBufferData));
}
#else
Transform::Transform()
 : CBuffer(nullptr), ECB_CBuffer(nullptr)
 , Position{0,0,0}, EulerAngleInDegree{0,0,0}, EulerAngleInRadian{0,0,0}, Scale{1,1,1}
 , ref_WorldMatrix(nullptr)
{
	CBufferData.World= Matrix::Identity;
	CBuffer = new ConstantBuffer(&CBufferData, "Transform World Mat", sizeof(ThisClass::CBufferData));
}

Transform::Transform(const Matrix * InMatrix )
	: CBuffer(nullptr), ECB_CBuffer(nullptr)
	, Position{0,0,0}, EulerAngleInDegree{0,0,0}, EulerAngleInRadian{0,0,0}, Scale{1,1,1}
	, ref_WorldMatrix(InMatrix)
{
	CBufferData.World= Matrix::Identity;
	CBuffer = new ConstantBuffer(&CBufferData, "Transform World Mat", sizeof(ThisClass::CBufferData));
}
#endif

Transform::~Transform()
{
	SAFE_DELETE(CBuffer);
}

// void Transform::Tick()
// {
// 	if (bTransformChanged == false)
// 		return ;
// 	UpdateWorldMatrix();
// 	
// 	bTransformChanged = false;
// }

void Transform::BindCBufferToGPU( const Shader * InShader )
{
	ASSERT(CBuffer != nullptr, "CBuffer Not Valid")
	ASSERT(InShader != nullptr, "Shader Not Valid")

	if (ECB_CBuffer == nullptr)
		ECB_CBuffer = InShader->AsConstantBuffer("CB_World");
	
	CBuffer->BindToGPU();
	CHECK(ECB_CBuffer->SetConstantBuffer(*CBuffer) >= 0);
}

Vector Transform::GetForward() const
{
	return CBufferData.World.Forward();
}

Vector Transform::GetUp() const
{
	return CBufferData.World.Up();
}

Vector Transform::GetRight() const
{
	return CBufferData.World.Right();
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
	EulerAngleInRadian = InRotation.ToEulerAngles();
	EulerAngleInDegree = { Math::ToDegrees(EulerAngleInRadian.X), Math::ToDegrees(EulerAngleInRadian.Y), Math::ToDegrees(EulerAngleInRadian.Z)};
	UpdateWorldMatrix();
}

void Transform::UpdateWorldMatrix()
{
	const Matrix Translation = Matrix::CreateTranslation(Position);
	const Matrix Rotation = Matrix::CreateFromYawPitchRoll(EulerAngleInRadian.Z, EulerAngleInRadian.Y, EulerAngleInRadian.X);
	const Matrix Scale = Matrix::CreateScale(this->Scale);

	CBufferData.World = Scale * Rotation * Translation;

	if (ref_WorldMatrix != nullptr)
		memcpy((void *)ref_WorldMatrix, &CBufferData.World, sizeof(Matrix));
}
