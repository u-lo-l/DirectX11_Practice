#pragma once

class Vector2D;
class Vector;
class Matrix;
class Quaternion;

//////////////////////////////////////////////////////////////////////////
///@brief 4D 벡터
//////////////////////////////////////////////////////////////////////////
class Vector4
{
public:
	Vector4(void);
	Vector4(float x, float y, float z, float w);
	Vector4(Vector2D value, float z, float w);
	Vector4(Vector value, float w);
	explicit Vector4(float value);

	Vector4 operator -(void);

	bool operator ==(const Vector4& value2) const;
	bool operator !=(const Vector4& value2) const;

	Vector4 operator +(const Vector4& value2) const;
	Vector4 operator -(const Vector4& value2) const;
	Vector4 operator *(const Vector4& value2) const;
	Vector4 operator *(const float& scaleFactor) const;
	Vector4 operator /(const Vector4& value2) const;
	Vector4 operator /(const float& divider) const;

	void operator +=(const Vector4& value2);
	void operator -=(const Vector4& value2);
	void operator *=(const Vector4& value2);
	void operator *=(const float& scaleFactor);
	void operator /=(const Vector4& value2);
	void operator /=(const float& divider);


	//////////////////////////////////////////////////////////////////////////
	///@brief float형의 *연산 처리
	///@param scalefactor : 값
	///@param value2 : 벡터
	///@return 결과 벡터
	//////////////////////////////////////////////////////////////////////////
	friend Vector4 operator *(const float scaleFactor, const Vector4& value2)
	{
		return value2 * scaleFactor;
	}
	//////////////////////////////////////////////////////////////////////////


	std::wstring ToString();
	struct D3DXVECTOR4 ToDX();

	float Length();
	float LengthSquared();

	void Normalize();

	static Vector4 Add(Vector4 value1, Vector4 value2);
	static Vector4 Divide(Vector4 value1, Vector4 value2);
	static Vector4 Divide(Vector4 value1, float divider);
	static Vector4 Multiply(Vector4 value1, Vector4 value2);
	static Vector4 Multiply(Vector4 value1, float scaleFactor);
	static Vector4 Subtract(Vector4 value1, Vector4 value2);

	static Vector4 Negative(Vector4 value);


	static Vector4 Barycentric(Vector4 value1, Vector4 value2, Vector4 value3, float amount1, float amount2);

	static float Distance(Vector4 value1, Vector4 value2);
	static float DistanceSquared(Vector4 value1, Vector4 value2);

	static float Dot(Vector4 value1, Vector4 value2);
	static Vector4 Normalize(Vector4 value);

	static Vector4 Min(Vector4 value1, Vector4 value2);
	static Vector4 Max(Vector4 value1, Vector4 value2);
	static Vector4 Clamp(Vector4 value1, Vector4 min, Vector4 max);

	static Vector4 Lerp(Vector4 value1, Vector4 value2, float amount);
	static Vector4 SmoothStep(Vector4 value1, Vector4 value2, float amount);
	static Vector4 CatmullRom(Vector4 value1, Vector4 value2, Vector4 value3, Vector4 value4, float amount);
	static Vector4 Hermite(Vector4 value1, Vector4 tangent1, Vector4 value2, Vector4 tangent2, float amount);

	static Vector4 Transform(Vector2D position, Matrix matrix);
	static Vector4 Transform(Vector position, Matrix matrix);
	static Vector4 Transform(Vector4 position, Matrix matrix);

	static Vector4 Transform(Vector2D value, Quaternion rotation);
	static Vector4 Transform(Vector value, Quaternion rotation);
	static Vector4 Transform(Vector4 value, Quaternion rotation);

public:
	const static Vector4 Zero;///< 0.0f, 0.0f, 0.0f, 0.0f
	const static Vector4 One;///< 1.0f, 1.0f, 1.0f, 1.0f

	const static Vector4 UnitX;///< 1.0f, 0.0f, 0.0f, 0.0f
	const static Vector4 UnitY;///< 0.0f, 1.0f, 0.0f, 0.0f
	const static Vector4 UnitZ;///< 0.0f, 0.0f, 1.0f, 0.0f
	const static Vector4 UnitW;///< 0.0f, 0.0f, 1.0f, 1.0f

	float X;///< X
	float Y;///< Y
	float Z;///< Z
	float W;///< W
};