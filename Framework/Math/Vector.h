﻿#pragma once
#include <assimp/vector3.h>
#include <DirectXMath.h>

class Matrix;
class Quaternion;

class Vector
{
public:
	Vector();
	explicit Vector(float value);
	Vector(float x, float y, float z);
	Vector(const Vector& v);
	Vector& operator=(const Vector& v);

	explicit Vector(const aiVector3D & v);
	
	Vector operator -() const;

	operator float* ();
	operator const float* () const;

	operator DirectX::XMFLOAT3 ();
	operator const DirectX::XMFLOAT3 () const;

	bool operator ==(const Vector& value2) const;
	bool operator !=(const Vector& value2) const;

	Vector operator +(const Vector& value2) const;
	Vector operator -(const Vector& value2) const;
	Vector operator *(const Vector& value2) const;
	Vector operator *(const float& scaleFactor) const;
	Vector operator *(const Matrix& Matrix) const;
	Vector operator /(const Vector& value2) const;
	Vector operator /(const float& divider) const;

	void operator +=(const Vector& value2);
	void operator -=(const Vector& value2);
	void operator *=(const Vector& value2);
	void operator *=(const float& scaleFactor);
	void operator /=(const Vector& value2);
	void operator /=(const float& divider);

	friend Vector operator *(const float scaleFactor, const Vector& value2)
	{
		return value2 * scaleFactor;
	}

	std::wstring ToWString() const;
	std::string ToString() const;

	float Length() const;
	float LengthSquared() const;

	void Normalize();

	static Vector Add( const Vector & value1, const Vector & value2);
	static Vector Divide( const Vector & value1, const Vector & value2);
	static Vector Divide( const Vector & value1, float value2);
	static Vector Multiply( const Vector & value1, const Vector & value2);
	static Vector Multiply( const Vector & value1, float scaleFactor);
	static Vector Subtract( const Vector & value1, const Vector & value2);
	static Vector Abs(const Vector & value);
	static Vector Negative( const Vector & value);

	static Vector Barycentric( const Vector & value1, const Vector & value2, const Vector & value3, float amount1, float amount2);

	static float Distance( const Vector & value1, const Vector & value2);
	static float DistanceSquared( const Vector & value1, const Vector & value2);

	static float Dot( const Vector & value1, const Vector & value2);
	static Vector Normalize( const Vector & value);

	static Vector Cross( const Vector & vector1, const Vector & vector2);
	static Vector Reflect( const Vector & vector, const Vector & normal);

	static Vector Min( const Vector & value1, const Vector & value2);
	static Vector Max( const Vector & value1, const Vector & value2);
	static Vector Clamp( const Vector & value1, const Vector & min, const Vector & max);

	static Vector Lerp( const Vector & value1, const Vector & value2, float amount);
	static Vector SmoothStep( const Vector & value1, const Vector & value2, float amount);
	static Vector CatmullRom( const Vector & value1, const Vector & value2, const Vector & value3, const Vector & value4, float amount);
	static Vector Hermite( const Vector & value1, const Vector & tangent1, const Vector & value2, const Vector & tangent2, float amount);

	static Vector Transform( const Vector & position, const Matrix & matrix);
	static Vector Transform( const Vector & value, const Quaternion & rotation);
	static Vector TransformNormal( const Vector & normal, const Matrix & matrix);
	static Vector TransformCoord( const Vector & position, const Matrix & matrix);

	static Vector Random(float MinLength, float MaxLength);
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

	union
	{
		struct
		{
			float X;///< X
			float Y;///< Y
			float Z;///< Z
		};

		float V[3];
	};
};