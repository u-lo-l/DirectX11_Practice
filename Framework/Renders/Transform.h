#pragma once

/**
 * Translate Rotation Scale 담당할 클래스
 */
class Transform
{
private:
	using ThisClass = Transform;
public:
	explicit Transform(const Matrix * InMatrix = nullptr);
	~Transform();

	void Tick();
	void BindToGPU();
public:
	Vector GetForward() const;
	Vector GetUp() const;
	Vector GetRight() const;

	const Vector & GetPosition() const;
	const Vector & GetScale() const;
	const Vector & GetRotationInRadian() const;
	const Vector & GetRotationInDegree() const;
	const Matrix & GetRotationMatrix() const;
	const Quaternion & GetQuaternion() const;

	void SetPosition(const Vector & InPosition);
	void SetRotation(const Vector & InEulerAngleInDegree);
	void SetRotation(const Matrix & InMatrix);
	void SetScale(const Vector & InScale);
	void SetTRS( const Transform & InTransform );
	void SetTRS( const Transform * InTransform );
	void SetTRS( const Vector & InPosition, const Quaternion & InRotation, const Vector & InScale );

public:
	const Matrix & GetMatrix() const {return WorldTransform_Data.World; }
	
public:
	void UpdateWorldMatrix();
	
private :
	struct CBufferDesc
	{
		Matrix World;
	};
private:
	ConstantBuffer * CBuffer;

private:
	Vector Position;
	
	Vector EulerAngleInDegree;
	Vector EulerAngleInRadian;
	Matrix RotationMat;
	Quaternion Rotation;
	
	Vector Scale;
	CBufferDesc WorldTransform_Data;
};