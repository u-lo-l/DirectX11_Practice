#pragma once

class Vector;
class Matrix;

//////////////////////////////////////////////////////////////////////////
///@brief 사원수
//////////////////////////////////////////////////////////////////////////
class Quaternion
{
public:
	Quaternion();
	Quaternion(float x, float y, float z, float w);
	Quaternion(Vector vectorPart, float scalarPart);
	Quaternion(const Quaternion& Other);
	Quaternion& operator=(const Quaternion& Other);
	
	Quaternion operator -();
	
	operator float* ();
	operator const float* () const;

	bool operator ==(const Quaternion& quaternion2) const;
	bool operator !=(const Quaternion& quaternion2) const;

	Quaternion operator +(const Quaternion& quaternion2) const;
	Quaternion operator -(const Quaternion& quaternion2) const;
	Quaternion operator *(const Quaternion& quaternion2) const;
	Quaternion operator *(const float& scaleFactor) const;
	Quaternion operator /(const Quaternion& quaternion2) const;

	void operator +=(const Quaternion& quaternion2);
	void operator -=(const Quaternion& quaternion2);
	void operator *=(const Quaternion& quaternion2);
	void operator *=(const float& scaleFactor);
	void operator /=(const Quaternion& quaternion2);


	//////////////////////////////////////////////////////////////////////////
	///@brief float형의 *연산 처리
	///@param scalefactor : 값
	///@param value2 : 벡터
	///@return 결과 벡터
	//////////////////////////////////////////////////////////////////////////
	friend Quaternion operator *(const float scaleFactor, const Quaternion& value2)
	{
		return value2 * scaleFactor;
	}
	//////////////////////////////////////////////////////////////////////////


	std::wstring ToWString() const;
	std::string ToString() const;

	float Length();
	float LengthSquared();

	void Normalize();
	static Quaternion Normalize( const Quaternion & quaternion);

	void Conjugate();
	static Quaternion Conjugate( const Quaternion & value);
	static Quaternion Inverse( const Quaternion & quaternion);

	static Quaternion Add( const Quaternion & quaternion1, const Quaternion & quaternion2);
	static Quaternion Divide( const Quaternion & quaternion1, const Quaternion & quaternion2);
	static Quaternion Multiply( const Quaternion & quaternion1, const Quaternion & quaternion2);
	static Quaternion Multiply( const Quaternion & quaternion1, float scaleFactor);
	static Quaternion Subtract( const Quaternion & quaternion1, const Quaternion & quaternion2);

	static Quaternion Negative( const Quaternion & quaternion);

	static Quaternion CreateFromAxisAngle( const Vector & axis, float angle);
	static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll);
	static Quaternion CreateFromRotationMatrix( const Matrix & matrix);

	static float Dot( const Quaternion & quaternion1, const Quaternion & quaternion2);

	static Quaternion Slerp( const Quaternion & quaternion1, const Quaternion & quaternion2, float amount);
	static Quaternion Lerp( const Quaternion & quaternion1, const Quaternion & quaternion2, float amount);

	static Quaternion Concatenate( const Quaternion & quaternion1, const Quaternion & quaternion2);

public:
	const static Quaternion Identity;///< 0.0f, 0.0f, 0.0f, 1.0f

	union
	{
		struct
		{
			float X;///< X
			float Y;///< Y
			float Z;///< Z
			float W;///< W
		};

		float Q[4];
	};
};