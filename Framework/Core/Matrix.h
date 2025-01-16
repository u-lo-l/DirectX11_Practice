// ReSharper disable CppNonExplicitConversionOperator
#pragma once
#include <assimp/matrix4x4.h>
class Vector;
class Quaternion;
class Plane;
//////////////////////////////////////////////////////////////////////////
///@brief 행렬 ( 왼손좌표게를 위한 행우선 )
//////////////////////////////////////////////////////////////////////////
class Matrix
{
public:
	Matrix();
	Matrix(float m11, float m12, float m13, float m14,
		   float m21, float m22, float m23, float m24,
		   float m31, float m32, float m33, float m34,
		   float m41, float m42, float m43, float m44);
#ifdef AI_MATRIX4X4_H_INC
	// ReSharper disable once CppNonExplicitConvertingConstructor
	Matrix(const aiMatrix4x4& InAiMat4X4);
#endif
	// Getter, Setter
	Vector Up() const;				void Up( const Vector & value);
	Vector Down() const;			void Down( const Vector & value);
	Vector Right() const;			void Right( const Vector & value);
	Vector Left() const;			void Left( const Vector & value);
	Vector Forward() const;			void Forward( const Vector & value);
	Vector Backward() const;		void Backward( const Vector & value);
	Vector Translate() const;		void Translate( const Vector & value);
	Matrix operator -() const;
	void Transpose();
	void Invert();
	bool operator ==(const Matrix& matrix2) const;
	bool operator !=(const Matrix& matrix2) const;
	Matrix operator +(const Matrix& matrix2) const;
	Matrix operator -(const Matrix& matrix2) const;
	Matrix operator *(const Matrix& matrix2) const;
	Matrix operator *(const float& scaleFactor) const;
	Matrix operator /(const Matrix& matrix2) const;
	Matrix operator /(const float& divider) const;
	void operator +=(const Matrix& matrix2);
	void operator -=(const Matrix& matrix2);
	void operator *=(const Matrix& matrix2);
	void operator *=(const float& scaleFactor);
	void operator /=(const Matrix& matrix2);
	void operator /=(const float& divider);
	operator float *();
	operator const float *() const;
	friend Matrix operator *(const float ScaleFactor, const Matrix& Matrix2)
	{
		return Matrix2 * ScaleFactor;
	}
	std::wstring ToWString();
	std::wstring ToStringRow1();
	std::wstring ToStringRow2();
	std::wstring ToStringRow3();
	std::wstring ToStringRow4();
	float Determinant();
	bool Decompose(Vector& scale, Quaternion& rotation, Vector& translation) const;
	bool DecomposeUniformScale(float& scale, Quaternion& rotation, Vector& translation);
	static Matrix Add(const Matrix & matrix1, const Matrix & matrix2);
	static Matrix Divide(const Matrix & matrix1, const Matrix & matrix2 );
	static Matrix Divide(const Matrix & matrix1, float divider);
	static Matrix Multiply(const Matrix & matrix1, const Matrix & matrix2);
	static Matrix Multiply(const Matrix & matrix1, float scaleFactor);
	static Matrix Negative(const Matrix & matrix);
	void Negative();
	static Matrix CreateBillboard(Vector objectPosition, Vector cameraPosition, Vector cameraUpVector, Vector* cameraForwardVector = NULL);
	static Matrix CreateTranslation( const Vector & position);
	static Matrix CreateTranslation(float xPosition, float yPosition, float zPosition);
	static Matrix CreateScale( const Vector & scales);
	static Matrix CreateScale(float xScale, float yScale, float zScale);
	static Matrix CreateScale(float scale);
	static Matrix CreateRotationX(float radians);
	static Matrix CreateRotationY(float radians);
	static Matrix CreateRotationZ(float radians);
	static void CreateFromEulerAngle( Matrix & OutMat, const Vector & EulerInRad );
	static Matrix CreateFromAxisAngle(Vector axis, float angle);
	static Matrix CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance);
	static Matrix CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
	static Matrix CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance);
	static Matrix CreateLookAt(Vector cameraPosition, Vector cameraTarget, Vector cameraUpVector);
	static void CreateLookAt(Matrix & outMatrix, const Vector & cameraPosition, const Vector & cameraTarget, const Vector & cameraUpVector);
	static Matrix CreateWorld(Vector position, Vector forward, Vector up);
	static Matrix CreateFromQuaternion(Quaternion quaternion);
	static Matrix CreateFromYawPitchRoll(float yaw, float pitch, float roll);
	static Matrix CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane);
	static Matrix CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);
	static Matrix Transform(const Matrix & value, const Quaternion & rotation);
	static Matrix Transpose(const Matrix & matrix);
	
	static Matrix Invert(const Matrix & matrix);
	static Matrix Lerp(const Matrix & matrix1, const Matrix & matrix2, float amount);
private:
	struct CanonicalBasis
	{
		Vector Row0;
		Vector Row1;
		Vector Row2;
	};
	struct VectorBasis
	{
		Vector * Element0;
		Vector * Element1;
		Vector * Element2;
	};
public:
	const static Matrix Identity;

	union
	{
		struct
		{
			float M11, M12, M13, M14;
			float M21, M22, M23, M24;
			float M31, M32, M33, M34;
			float M41, M42, M43, M44;
		};
		float M[16];
	};
};
