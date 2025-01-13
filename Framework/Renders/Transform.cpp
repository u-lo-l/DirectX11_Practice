#include "framework.h"
#include "Transform.h"

Transform::Transform()
 : CBuffer(nullptr), ECB_CBuffer(nullptr)
 , Position{0,0,0}, EulerAngleInDegree{0,0,0}, EulerAngleInRadian{0,0,0}, Scale{1,1,1}
{
	CBufferData.World= Matrix::Identity;
	CBuffer = new ConstantBuffer(&CBufferData, "Transform World Mat", sizeof(ThisClass::CBufferData));
}

Transform::~Transform()
{
	SAFE_DELETE(CBuffer);
}

void Transform::Tick()
{
	if (bDirty == false)
		return ;
	bDirty = true;

	
	UpdateWorldMatrix();
}

void Transform::Render( const Shader * InShader )
{
	ASSERT(CBuffer != nullptr, "CBuffer Not Valid");
	ASSERT(InShader != nullptr, "Shader Not Valid");

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
	bDirty = true;
}

void Transform::SetRotation( const Vector & InEulerAngleInDegree )
{
	EulerAngleInDegree = InEulerAngleInDegree;
	EulerAngleInRadian = {Math::ToRadians(EulerAngleInDegree.X), Math::ToRadians(EulerAngleInDegree.Y), Math::ToRadians(EulerAngleInDegree.Z)};
	bDirty = true;
}

void Transform::SetScale( const Vector & InScale )
{
	Scale = InScale;
	bDirty = true;
}


void Transform::UpdateWorldMatrix()
{
	const Matrix Translation = Matrix::CreateTranslation(Position);
	const Matrix Rotation = Matrix::CreateFromYawPitchRoll(EulerAngleInRadian.Y, EulerAngleInRadian.X, EulerAngleInRadian.Z);
	const Matrix Scale = Matrix::CreateScale(this->Scale);

	CBufferData.World = Scale * Rotation * Translation;
}
