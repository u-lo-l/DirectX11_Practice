#pragma once

class Vector;
class Matrix;

//////////////////////////////////////////////////////////////////////////
///@brief 사원수
//////////////////////////////////////////////////////////////////////////
class Quaternion
{
public:
	Quaternion(void);
	Quaternion(float x, float y, float z, float w);
	Quaternion(Vector vectorPart, float scalarPart);

	Quaternion operator -(void);

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


	std::wstring ToString();
	D3DXQUATERNION ToDX();

	float Length();
	float LengthSquared();

	void Normalize();
	static Quaternion Normalize(Quaternion quaternion);

	void Conjugate();
	static Quaternion Conjugate(Quaternion value);
	static Quaternion Inverse(Quaternion quaternion);

	static Quaternion Add(Quaternion quaternion1, Quaternion quaternion2);
	static Quaternion Divide(Quaternion quaternion1, Quaternion quaternion2);
	static Quaternion Multiply(Quaternion quaternion1, Quaternion quaternion2);
	static Quaternion Multiply(Quaternion quaternion1, float scaleFactor);
	static Quaternion Subtract(Quaternion quaternion1, Quaternion quaternion2);

	static Quaternion Negative(Quaternion quaternion);

	static Quaternion CreateFromAxisAngle(Vector axis, float angle);
	static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll);
	static Quaternion CreateFromRotationMatrix(Matrix matrix);

	static float Dot(Quaternion quaternion1, Quaternion quaternion2);

	static Quaternion Slerp(Quaternion quaternion1, Quaternion quaternion2, float amount);
	static Quaternion Lerp(Quaternion quaternion1, Quaternion quaternion2, float amount);

	static Quaternion Concatenate(Quaternion quaternion1, Quaternion quaternion2);

public:
	const static Quaternion Identity;///< 0.0f, 0.0f, 0.0f, 1.0f

	float X;///< X
	float Y;///< Y
	float Z;///< Z
	float W;///< W
};