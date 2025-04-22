#pragma once

/**
 * Translate Rotation Scale 담당할 클래스
 */
// class Transform
// {
// private:
// 	using ThisClass = Transform;
// public:
// 	explicit Transform(const Matrix * InMatrix = nullptr);
// 	~Transform();
//
// 	void Tick();
// 	void BindToGPU();
// public:
// 	Vector GetForward() const;
// 	Vector GetUp() const;
// 	Vector GetRight() const;
//
// 	const Vector & GetPosition() const;
// 	const Vector & GetScale() const;
// 	const Vector & GetRotationInRadian() const;
// 	const Vector & GetRotationInDegree() const;
// 	const Matrix & GetRotationMatrix() const;
// 	const Quaternion & GetQuaternion() const;
//
// 	void SetPosition(const Vector & InPosition);
// 	void SetRotation(const Vector & InEulerAngleInDegree);
// 	void SetRotation(const Matrix & InMatrix);
// 	void SetScale(const Vector & InScale);
// 	void SetTRS( const Transform & InTransform );
// 	void SetTRS( const Transform * InTransform );
// 	void SetTRS( const Vector & InPosition, const Quaternion & InRotation, const Vector & InScale );
//
// public:
// 	const Matrix & GetMatrix() const {return WorldTransform_Data.World; }
// 	
// public:
// 	void UpdateWorldMatrix();
// 	
// private :
// 	struct CBufferDesc
// 	{
// 		Matrix World;
// 	};
// private:
// 	ConstantBuffer * CBuffer;
//
// private:
// 	Vector Position;
// 	
// 	Vector EulerAngleInDegree;
// 	Vector EulerAngleInRadian;
// 	Matrix RotationMat;
// 	Quaternion Rotation;
// 	
// 	Vector Scale;
// 	CBufferDesc WorldTransform_Data;
// };

class Transform
{
private:
	using ThisClass = Transform;
public:
	explicit Transform(const Matrix * InMatrix = nullptr);
	~Transform();

	void Tick();
	void BindToGPU() const;
public:
	void SetWorldPosition(const Vector & InPosition);
	void SetWorldRotation(const Matrix & InRotation);
	void SetWorldRotation(const Quaternion & InQuat);
	void SetWorldRotation(const Vector & ZYXEulerRadian);
	void SetScale(const Vector & InScale);

	const Matrix & GetMatrix() const;
	const Vector & GetWorldPosition() const;
	const Matrix & GetWorldRotationMat() const;
	const Quaternion & GetWorldRotation() const;
	const Vector & GetWorldZYXEulerAngle() const;
	const Vector & GetWorldZYXEulerAngleInDegree() const;
	const Vector & GetScale() const;
	Vector GetForward() const;
	Vector GetUp() const;
	Vector GetRight() const;
	
	void AddWorldTranslation(const Vector & InTranslation);
	void AddLocalTranslation(const Vector & InTranslation);
	void AddWorldRotation(const Matrix & InRotationMat);
	void AddLocalRotation(const Matrix & InRotationMat);
	void AddWorldRotation(const Quaternion & InRotation);
	void AddLocalRotation(const Quaternion & InRotation);
	void AddWorldRotation(const Vector & InZYXEulerRadian);
	void AddLocalRotation(const Vector & InZYXEulerRadian);
	void AddScale(const Vector & InScale);
	void SetTRS(const Vector& Pos, const Quaternion& Rot, const Vector& Scale);

private :
	void UpdateMatrix();
	struct CBufferDesc
	{
		Matrix World;
	} CBufferData;
	ConstantBuffer * CBuffer;

private:
	Vector Position;	// World Position
	
	Vector EulerAngleInDegree;
	Vector EulerAngleInRadian;
	Matrix RotationMat;	// World Rotation
	Quaternion Rotation;
	
	Vector Scale = {1,1,1};;
};