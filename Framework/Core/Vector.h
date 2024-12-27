#pragma once

class Matrix;
class Quaternion;

//////////////////////////////////////////////////////////////////////////
///@brief 3D 벡터
//////////////////////////////////////////////////////////////////////////
class Vector
{
public:
	Vector(void);
	Vector(float value);
	Vector(float x, float y, float z);

	Vector operator -(void);

	bool operator ==(const Vector& value2) const;
	bool operator !=(const Vector& value2) const;

	Vector operator +(const Vector& value2) const;
	Vector operator -(const Vector& value2) const;
	Vector operator *(const Vector& value2) const;
	Vector operator *(const float& scaleFactor) const;
	Vector operator /(const Vector& value2) const;
	Vector operator /(const float& divider) const;

	void operator +=(const Vector& value2);
	void operator -=(const Vector& value2);
	void operator *=(const Vector& value2);
	void operator *=(const float& scaleFactor);
	void operator /=(const Vector& value2);
	void operator /=(const float& divider);


	//////////////////////////////////////////////////////////////////////////
	///@brief float형의 *연산 처리
	///@param scalefactor : 값
	///@param value2 : 벡터
	///@return 결과 벡터
	//////////////////////////////////////////////////////////////////////////
	friend Vector operator *(const float scaleFactor, const Vector& value2)
	{
		return value2 * scaleFactor;
	}
	//////////////////////////////////////////////////////////////////////////


	std::wstring ToString();
	D3DXVECTOR3 ToDX();

	float Length();
	float LengthSquared();

	void Normalize();

	static Vector Add(Vector value1, Vector value2);
	static Vector Divide(Vector value1, Vector value2);
	static Vector Divide(Vector value1, float value2);
	static Vector Multiply(Vector value1, Vector value2);
	static Vector Multiply(Vector value1, float scaleFactor);
	static Vector Subtract(Vector value1, Vector value2);

	static Vector Negative(Vector value);

	static Vector Barycentric(Vector value1, Vector value2, Vector value3, float amount1, float amount2);

	static float Distance(Vector value1, Vector value2);
	static float DistanceSquared(Vector value1, Vector value2);

	static float Dot(Vector value1, Vector value2);
	static Vector Normalize(Vector value);

	static Vector Cross(Vector vector1, Vector vector2);
	static Vector Reflect(Vector vector, Vector normal);

	static Vector Min(Vector value1, Vector value2);
	static Vector Max(Vector value1, Vector value2);
	static Vector Clamp(Vector value1, Vector min, Vector max);

	static Vector Lerp(Vector value1, Vector value2, float amount);
	static Vector SmoothStep(Vector value1, Vector value2, float amount);
	static Vector CatmullRom(Vector value1, Vector value2, Vector value3, Vector value4, float amount);
	static Vector Hermite(Vector value1, Vector tangent1, Vector value2, Vector tangent2, float amount);

	static Vector Transform(Vector position, Matrix matrix);
	static Vector Transform(Vector value, Quaternion rotation);
	static Vector TransformNormal(Vector normal, Matrix matrix);
	static Vector TransformCoord(Vector position, Matrix matrix);

public:
	const static Vector Zero;///< 0.0f, 0.0f, 0.0f
	const static Vector One;///< 1.0f, 1.0f, 1.0f

	const static Vector UnitX;///< 1.0f, 0.0f, 0.0f
	const static Vector UnitY;///< 0.0f, 1.0f, 0.0f
	const static Vector UnitZ;///< 0.0f, 0.0f, 1.0f

	const static Vector Up;///< 0.0f, 1.0f, 0.0f
	const static Vector Down;///< 0.0f, -1.0f, 0.0f
	const static Vector Right;///< 1.0f, 0.0f, 0.0f
	const static Vector Left;///< -1.0f, 0.0f, 0.0f
	const static Vector Forward;///< 0.0f, 0.0f, -1.0f
	const static Vector Backward;///< 0.0f, 0.0f, 1.0f

	float X;///< X
	float Y;///< Y
	float Z;///< Z
};