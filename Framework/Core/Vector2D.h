#pragma once

class Matrix;
class Quaternion;

//////////////////////////////////////////////////////////////////////////
///@brief 2D 벡터
//////////////////////////////////////////////////////////////////////////
class Vector2D
{
public:
	Vector2D(void);
	Vector2D(float value);
	Vector2D(float x, float y);

	operator float* ();
	operator const float* () const;

	Vector2D operator -(void);

	bool operator ==(const Vector2D& value2) const;
	bool operator !=(const Vector2D& value2) const;

	Vector2D operator +(const Vector2D& value2) const;
	Vector2D operator -(const Vector2D& value2) const;
	Vector2D operator *(const Vector2D& value2) const;
	Vector2D operator *(const float& scaleFactor) const;
	Vector2D operator /(const Vector2D& value2) const;
	Vector2D operator /(const float& divider) const;

	void operator +=(const Vector2D& value2);
	void operator -=(const Vector2D& value2);
	void operator *=(const Vector2D& value2);
	void operator *=(const float& scaleFactor);
	void operator /=(const Vector2D& value2);
	void operator /=(const float& divider);


	//////////////////////////////////////////////////////////////////////////
	///@brief float형의 *연산 처리
	///@param scalefactor : 값
	///@param value2 : 벡터
	///@return 결과 벡터
	//////////////////////////////////////////////////////////////////////////
	friend Vector2D operator *(const float scaleFactor, const Vector2D& value2)
	{
		return value2 * scaleFactor;
	}
	//////////////////////////////////////////////////////////////////////////


	std::wstring ToString();

	float Length();
	float LengthSquared();

	void Normalize();

	static Vector2D Add( Vector2D value1, Vector2D value2 );
	static Vector2D Divide( Vector2D value1, Vector2D value2 );
	static Vector2D Divide( Vector2D value1, float divider );
	static Vector2D Multiply( Vector2D value1, Vector2D value2 );
	static Vector2D Multiply( Vector2D value1, float scaleFactor );
	static Vector2D Subtract( Vector2D value1, Vector2D value2 );

	static Vector2D Negative( Vector2D value );

	static Vector2D Barycentric( Vector2D value1, Vector2D value2, Vector2D value3, float amount1, float amount2 );

	static float Distance( Vector2D value1, Vector2D value2 );
	static float DistanceSquared( Vector2D value1, Vector2D value2 );

	static float Dot( Vector2D value1, Vector2D value2 );
	static Vector2D Normalize( Vector2D value );

	static Vector2D Reflect( Vector2D vector, Vector2D normal );

	static Vector2D Min( Vector2D value1, Vector2D value2 );
	static Vector2D Max( Vector2D value1, Vector2D value2 );
	static Vector2D Clamp( Vector2D value1, Vector2D min, Vector2D max );

	static Vector2D Lerp( Vector2D value1, Vector2D value2, float amount );
	static Vector2D SmoothStep( Vector2D value1, Vector2D value2, float amount );
	static Vector2D CatmullRom( Vector2D value1, Vector2D value2, Vector2D value3, Vector2D value4, float amount );
	static Vector2D Hermite( Vector2D value1, Vector2D tangent1, Vector2D value2, Vector2D tangent2, float amount );

	static Vector2D Transform( Vector2D position, Matrix matrix );
	static Vector2D Transform( Vector2D value, Quaternion rotation );
	static Vector2D TransformNormal( Vector2D normal, Matrix matrix );

public:
	const static Vector2D Zero;///< 0.0f, 0.0f
	const static Vector2D One;///< 1.0f, 1.0f

	const static Vector2D UnitX;///< 1.0f, 0.0f
	const static Vector2D UnitY;///< 0.0f, 1.0f

	union
	{
		struct
		{
			float X;///< X
			float Y;///< Y
		};

		float V[2];
	};
};