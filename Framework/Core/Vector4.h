#pragma once
#include <immintrin.h>
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
	Vector4();
	Vector4(float x, float y, float z, float w);
	Vector4(Vector2D value, float z, float w);
	Vector4(Vector value, float w);
	explicit Vector4(float value);

	operator float*();
	operator const float*() const;

	Vector4 operator -();

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

	static Vector4 Add( const Vector4 & value1, const Vector4 & value2);
	static Vector4 Divide( const Vector4 & value1, const Vector4 & value2);
	static Vector4 Divide( const Vector4 & value1, float divider);
	static Vector4 Multiply( const Vector4 & value1, const Vector4 & value2);
	static Vector4 Multiply( const Vector4 & value1, float scaleFactor);
	static Vector4 Subtract( const Vector4 & value1, const Vector4 & value2);

	static Vector4 Negative( const Vector4 & value );


	static Vector4 Barycentric( const Vector4 & value1, const Vector4 & value2, const Vector4 & value3, float amount1, float amount2 );

	static float Distance( const Vector4 & value1, const Vector4 & value2 );
	static float DistanceSquared( const Vector4 & value1, const Vector4 & value2 );

	static float Dot( const Vector4 & value1, const Vector4 & value2 );
	static Vector4 Normalize( const Vector4 & value );

	static Vector4 Min( const Vector4 & value1, const Vector4 & value2 );
	static Vector4 Max( const Vector4 & value1, const Vector4 & value2 );
	static Vector4 Clamp( const Vector4 & value1, const Vector4 & min, const Vector4 & max );

	static Vector4 Lerp( const Vector4 & value1, const Vector4 & value2, float amount );
	static Vector4 SmoothStep( const Vector4 & value1, const Vector4 & value2, float amount );
	static Vector4 CatmullRom( const Vector4 & value1, const Vector4 & value2, const Vector4 & value3, const Vector4 & value4, float amount );
	static Vector4 Hermite( const Vector4 & value1, const Vector4 & tangent1, const Vector4 & value2, const Vector4 & tangent2, float amount );

	static Vector4 Transform( Vector2D position, const Matrix & matrix );
	static Vector4 Transform( const Vector & position, const Matrix & matrix );
	static Vector4 Transform( const Vector4 & position, const Matrix & matrix );

	static Vector4 Transform( Vector2D value, const Quaternion & rotation );
	static Vector4 Transform( const Vector & value, const Quaternion & rotation );
	static Vector4 Transform( const Vector4 & value, const Quaternion & rotation );

public:
	const static Vector4 Zero;///< 0.0f, 0.0f, 0.0f, 0.0f
	const static Vector4 One;///< 1.0f, 1.0f, 1.0f, 1.0f

	const static Vector4 UnitX;///< 1.0f, 0.0f, 0.0f, 0.0f
	const static Vector4 UnitY;///< 0.0f, 1.0f, 0.0f, 0.0f
	const static Vector4 UnitZ;///< 0.0f, 0.0f, 1.0f, 0.0f
	const static Vector4 UnitW;///< 0.0f, 0.0f, 1.0f, 1.0f

	union
	{
		struct
		{
			float X;///< X
			float Y;///< Y
			float Z;///< Z
			float W;///< W
		};

		float V[4];
	};
};