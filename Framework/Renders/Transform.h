#pragma once


/**
 * Translate Rotation Scale 담당할 클래스
 */
class Transform
{
private:
	using ThisClass = Transform;
public:
#ifdef DO_DEBUG
	explicit Transform( const string & MetaData = "Transform Matrix");
#else
	Transform();
#endif
	~Transform();

	void Tick();
	void BindCBufferToGPU(const Shader * InShader);
public:
	Vector GetForward() const;
	Vector GetUp() const;
	Vector GetRight() const;

	const Vector & GetPosition() const;
	const Vector & GetScale() const;
	const Vector & GetRotationInRadian() const;
	const Vector & GetRotationInDegree() const;

	void SetPosition(const Vector & InPosition);
	void SetRotation(const Vector & InEulerAngleInDegree);
	void SetScale(const Vector & InScale);
	void SetTRS(const Transform & InTransform);
	void SetTRS( const Transform * InTransform );

public:
	const Matrix & GetMatrix() const {return CBufferData.World; }
	
private:
	void UpdateWorldMatrix();
	bool bTransformChanged = true;
	
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
