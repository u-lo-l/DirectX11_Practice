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
	template<class TShaderType>
	void BindCBufferToGPU(const TShaderType * InShader);
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
	// IECB_t * ECB_CBuffer;

private:
	Vector Position;
	Vector EulerAngleInDegree;
	Vector EulerAngleInRadian;
	Vector Scale;
	CBufferDesc WorldTransform_Data;

#pragma region Instancing
	const Matrix * ref_WorldMatrix;
#pragma endregion Instancing
};

template<class TShaderType>
void Transform::BindCBufferToGPU( const TShaderType * InShader )
{
	ASSERT(CBuffer != nullptr, "CBuffer Not Valid")
	ASSERT(InShader != nullptr, "Shader Not Valid")

	// if (ECB_CBuffer == nullptr)
	// 	ECB_CBuffer = InShader->AsConstantBuffer("CB_World");
	CBuffer->BindToGPU();
	// CHECK(ECB_CBuffer->SetConstantBuffer(*CBuffer) >= 0);
}