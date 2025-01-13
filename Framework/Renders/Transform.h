#pragma once


/**
 * Translate Rotation Scale 담당할 클래스
 */
class Transform
{
private:
	using ThisClass = Transform;
public:
	Transform();
	~Transform();

	void Tick();
	void Render(const Shader * InShader);
public:
	Vector GetForward() const;
	Vector GetUp() const;
	Vector GetRight() const;

	const Vector & GetPosition() const;
	const Vector & GetScale() const;
	const Vector & GetRotationInRadian() const;
	const Vector & GetRotationInDegree() const;

	void SetPosition(const Vector & InPosition);
	void SetRotation(const Vector & InEulerAngle);
	void SetScale(const Vector & InScale);

private:
	void UpdateWorldMatrix();
private :
	bool bDirty = true;
	
private :
	struct CBufferDesc
	{
		Matrix World;
	};
private:
	ConstantBuffer * CBuffer;
	IECB_t * ECB_CBuffer;

private:
	Vector Position;
	Vector EulerAngleInDegree;
	Vector EulerAngleInRadian;
	Vector Scale;
	CBufferDesc CBufferData;
};
