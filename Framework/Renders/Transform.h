#pragma once

class Transform
{
private:
	using ThisClass = Transform;
public:
	explicit Transform(Matrix * InMatrix = nullptr, int InRegisterIndex = 0);
	~Transform();
	Transform * GetParent() const { return Parent; }
	void SetParent(Transform * InParent) { Parent = InParent; }
	void Tick();
	void BindToGPU(int InRegisterIndex = 0) const;
public:
	void SetWorldPosition(const Vector & InPosition);
	void SetWorldRotation(const Matrix & InRotation);
	void SetWorldRotation(const Quaternion & InQuat);
	void SetWorldRotation(const Vector & ZYXEulerRadian);
	void SetScale(const Vector & InScale);

	Matrix GetWorldMatrix() const;
	Matrix GetMatrix() const;
	const Matrix* GetMatrixRef() const;
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
	void SetTransform(const Matrix & InMatrix);
	void AddTransform(const Matrix & InMatrix);
	void AddTransform(const Transform & InTransform);
	void AddTransform(const Transform * InTransform);
	void SetTRS(const Vector& Pos, const Quaternion& Rot, const Vector& Scale);

private :
	void UpdateMatrix();
	bool bWorldTFAllocated = false;
	Transform * Parent = nullptr; 
	Matrix * Tf;
	ConstantBuffer * CBuffer;

private:
	Vector Position;						// World Position
	Vector EulerAngleInDegree;
	Vector EulerAngleInRadian;
	Matrix RotationMat = Matrix::Identity;	// World Rotation
	Quaternion Rotation;

	Vector Scale = {1,1,1};;
};