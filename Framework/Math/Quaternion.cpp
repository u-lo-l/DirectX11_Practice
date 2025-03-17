#include "Framework.h"
#include "Quaternion.h"

const Quaternion Quaternion::Identity = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

Quaternion::Quaternion()
	: W(1.0f), X(0.0f), Y(0.0f), Z(0.0f)
{
}

Quaternion::Quaternion(float w, float x, float y, float z)
	: W(w), X(x), Y(y), Z(z)
{
}


Quaternion::Quaternion( const Vector & vectorPart, float scalarPart)
	: W(scalarPart), X(vectorPart.X), Y(vectorPart.Y), Z(vectorPart.Z)
{
}

Quaternion::Quaternion( const Quaternion & Other )
	:W(Other.W), X(Other.X), Y(Other.Y), Z(Other.Z)
{
}

Quaternion & Quaternion::operator=( const Quaternion & Other )
{
	if (this == &Other)
		return *this;
	W = Other.W;
	X = Other.X;
	Y = Other.Y;
	Z = Other.Z;
	return *this;
}

Quaternion::Quaternion( const aiQuaternion & aiQuat )
{
	W = aiQuat.w;
	X = aiQuat.x;
	Y = aiQuat.y;
	Z = aiQuat.z;
}



Quaternion Quaternion::operator-() const
{
	Quaternion quaternion1;
	quaternion1.W = -W;
	quaternion1.X = -X;
	quaternion1.Y = -Y;
	quaternion1.Z = -Z;

	return quaternion1;
}

// Quaternion::operator float* ()
// {
// 	return Q;
// }
//
// Quaternion::operator const float* () const
// {
// 	return Q;
// }

bool Quaternion::operator==(const Quaternion& quaternion2) const
{
	if (X == quaternion2.X && Y == quaternion2.Y && Z == quaternion2.Z)
		return W == quaternion2.W;
	else
		return false;
}

bool Quaternion::operator!=(const Quaternion& quaternion2) const
{
	if (X == quaternion2.X && Y == quaternion2.Y && Z == quaternion2.Z)
		return W != quaternion2.W;
	else
		return true;
}

Quaternion Quaternion::operator+(const Quaternion& Other) const
{
	Quaternion Q;
	Q.X = X + Other.X;
	Q.Y = Y + Other.Y;
	Q.Z = Z + Other.Z;
	Q.W = W + Other.W;

	return Q;
}


Quaternion Quaternion::operator-(const Quaternion& Other) const
{
	Quaternion Q;
	Q.X = X - Other.X;
	Q.Y = Y - Other.Y;
	Q.Z = Z - Other.Z;
	Q.W = W - Other.W;
	return Q;
}


Quaternion Quaternion::operator*(const Quaternion& Other) const
{
	float value1 = Y * Other.Z - Z * Other.Y;
	float value2 = Z * Other.X - X * Other.Z;
	float value3 = X * Other.Y - Y * Other.X;
	float value4 = X * Other.X + Y * Other.Y + Z * Other.Z;

	Quaternion Q;
	Q.X = X * Other.W + Other.X * W + value1;
	Q.Y = Y * Other.W + Other.Y * W + value2;
	Q.Z = Z * Other.W + Other.Z * W + value3;
	Q.W = W * Other.W - value4;

	return Q;
}


Quaternion Quaternion::operator*(const float& InScaleFactor) const
{
	Quaternion quaternion;
	quaternion.X = X * InScaleFactor;
	quaternion.Y = Y * InScaleFactor;
	quaternion.Z = Z * InScaleFactor;
	quaternion.W = W * InScaleFactor;

	return quaternion;
}


Quaternion Quaternion::operator/(const Quaternion& Other) const
{
	float value1 = 1.0f / Other.X * Other.X + Other.Y * Other.Y + Other.Z * Other.Z + Other.W * Other.W;
	float value2 = -Other.X * value1;
	float value3 = -Other.Y * value1;
	float value4 = -Other.Z * value1;
	float value5 = Other.W * value1;

	float multiple1 = Y * value4 - Z * value3;
	float multiple2 = Z * value2 - X * value4;
	float multiple3 = X * value3 - Y * value2;
	float multiple4 = X * value2 + Y * value3 + Z * value4;

	Quaternion quaternion;
	quaternion.X = X * value5 + value2 * W + multiple1;
	quaternion.Y = Y * value5 + value3 * W + multiple2;
	quaternion.Z = Z * value5 + value4 * W + multiple3;
	quaternion.W = W * value5 - multiple4;

	return quaternion;
}


void Quaternion::operator+=(const Quaternion& Other)
{
	*this = *this + Other;
}


void Quaternion::operator-=(const Quaternion& Other)
{
	*this = *this - Other;
}


void Quaternion::operator*=(const Quaternion& Other)
{
	*this = *this * Other;
}


void Quaternion::operator*=(const float& Other)
{
	*this = *this * Other;
}


void Quaternion::operator/=(const Quaternion& Other)
{
	*this = *this / Other;
}


std::wstring Quaternion::ToWString() const
{
	std::wstring Temp = L"";

	Temp += L"W:" + std::to_wstring(W);
	Temp += L",X:" + std::to_wstring(X);
	Temp += L",Y:" + std::to_wstring(Y);
	Temp += L",Z:" + std::to_wstring(Z);

	return Temp;
}

std::string Quaternion::ToString() const
{
	std::string Temp;

	Temp += "W:" + std::to_string(W);
	Temp += ",X:" + std::to_string(X);
	Temp += ",Y:" + std::to_string(Y);
	Temp += ",Z:" + std::to_string(Z);

	return Temp;
}


float Quaternion::Length() const
{
	float x = X * X + Y * Y + Z * Z + W * W;

	return sqrtf(x);
}


float Quaternion::LengthSquared() const
{
	return X * X + Y * Y + Z * Z + W * W;
}

// Radian
Vector Quaternion::ToEulerAnglesInRadian() const
{
	Vector angles;

	// Roll (X-axis rotation)
	float sinr_cosp = 2 * (this->W * this->X + this->Y * this->Z);
	float cosr_cosp = 1 - 2 * (this->X * this->X + this->Y * this->Y);
	angles.X = atan2f(sinr_cosp, cosr_cosp);

	// Pitch (Y-axis rotation)
	float sinp = 2 * (this->W * this->Y - this->Z * this->X);
	if (fabsf(sinp) >= 1)
		angles.Y = std::copysignf(Math::Pi / 2, sinp); // Use 90 degrees if out of range
	else
		angles.Y = asinf(sinp);

	// Yaw (Z-axis rotation)
	float siny_cosp = 2 * (this->W * this->Z + this->X * this->Y);
	float cosy_cosp = 1 - 2 * (this->Y * this->Y + this->Z * this->Z);
	angles.Z = atan2f(siny_cosp, cosy_cosp);

	return angles;
}

Vector Quaternion::ToEulerAnglesInDegrees() const
{
	Vector angles;

	// Roll (X-axis rotation)
	float sinr_cosp = 2 * (this->W * this->X + this->Y * this->Z);
	float cosr_cosp = 1 - 2 * (this->X * this->X + this->Y * this->Y);
	angles.X = atan2f(sinr_cosp, cosr_cosp);

	// Pitch (Y-axis rotation)
	float sinp = 2 * (this->W * this->Y - this->Z * this->X);
	if (fabsf(sinp) >= 1)
		angles.Y = std::copysignf(Math::Pi / 2, sinp); // Use 90 degrees if out of range
	else
		angles.Y = asinf(sinp);

	// Yaw (Z-axis rotation)
	float siny_cosp = 2 * (this->W * this->Z + this->X * this->Y);
	float cosy_cosp = 1 - 2 * (this->Y * this->Y + this->Z * this->Z);
	angles.Z = atan2f(siny_cosp, cosy_cosp);

	angles *= Math::RadianToDeg;
	return angles;
}


void Quaternion::Normalize()
{
	float x = X * X + Y * Y + Z * Z + W * W;
	float value = 1.0f / sqrtf(x);

	X *= value;
	Y *= value;
	Z *= value;
	W *= value;
}


Quaternion Quaternion::Normalize( const Quaternion & quaternion)
{
	float value = quaternion.X * quaternion.X + quaternion.Y * quaternion.Y + quaternion.Z * quaternion.Z + quaternion.W * quaternion.W;
	float value1 = 1.0f / sqrtf(value);


	Quaternion q;
	q.X = quaternion.X * value1;
	q.Y = quaternion.Y * value1;
	q.Z = quaternion.Z * value1;
	q.W = quaternion.W * value1;

	return q;
}


void Quaternion::Conjugate()
{
	X = -X;
	Y = -Y;
	Z = -Z;
}


Quaternion Quaternion::Conjugate( const Quaternion & value)
{
	Quaternion quaternion;
	quaternion.X = -value.X;
	quaternion.Y = -value.Y;
	quaternion.Z = -value.Z;
	quaternion.W = value.W;

	return quaternion;
}


Quaternion Quaternion::Inverse( const Quaternion & quaternion)
{
	float value = quaternion.X * quaternion.X + quaternion.Y * quaternion.Y + quaternion.Z * quaternion.Z + quaternion.W * quaternion.W;
	float value1 = 1.0f / value;

	Quaternion q;
	q.X = -quaternion.X * value1;
	q.Y = -quaternion.Y * value1;
	q.Z = -quaternion.Z * value1;
	q.W = quaternion.W * value1;

	return q;
}
Quaternion Quaternion::Add( const Quaternion & quaternion1, const Quaternion & quaternion2)
{
	Quaternion quaternion;
	quaternion.X = quaternion1.X + quaternion2.X;
	quaternion.Y = quaternion1.Y + quaternion2.Y;
	quaternion.Z = quaternion1.Z + quaternion2.Z;
	quaternion.W = quaternion1.W + quaternion2.W;

	return quaternion;
}
Quaternion Quaternion::Divide( const Quaternion & quaternion1, const Quaternion & quaternion2)
{
	float x = quaternion1.X;
	float y = quaternion1.Y;
	float z = quaternion1.Z;
	float w = quaternion1.W;

	float value = quaternion2.X * quaternion2.X + quaternion2.Y * quaternion2.Y + quaternion2.Z * quaternion2.Z + quaternion2.W * quaternion2.W;
	float value1 = 1.0f / value;

	float x1 = -quaternion2.X * value1;
	float y1 = -quaternion2.Y * value1;
	float z1 = -quaternion2.Z * value1;
	float w1 = quaternion2.W * value1;

	float value2 = y * z1 - z * y1;
	float value3 = z * x1 - x * z1;
	float value4 = x * y1 - y * x1;
	float value5 = x * x1 + y * y1 + z * z1;

	Quaternion quaternion;
	quaternion.X = x * w1 + x1 * w + value2;
	quaternion.Y = y * w1 + y1 * w + value3;
	quaternion.Z = z * w1 + z1 * w + value4;
	quaternion.W = w * w1 - value5;

	return quaternion;
}
Quaternion Quaternion::Multiply( const Quaternion & quaternion1, const Quaternion & quaternion2)
{
	float x = quaternion1.X;
	float y = quaternion1.Y;
	float z = quaternion1.Z;
	float w = quaternion1.W;
	float value = quaternion2.X;
	float y1 = quaternion2.Y;
	float z1 = quaternion2.Z;
	float w1 = quaternion2.W;
	float value1 = y * z1 - z * y1;
	float value2 = z * value - x * z1;
	float value3 = x * y1 - y * value;
	float value4 = x * value + y * y1 + z * z1;

	Quaternion quaternion;
	quaternion.X = x * w1 + value * w + value1;
	quaternion.Y = y * w1 + y1 * w + value2;
	quaternion.Z = z * w1 + z1 * w + value3;
	quaternion.W = w * w1 - value4;

	return quaternion;
}
Quaternion Quaternion::Multiply( const Quaternion & quaternion1, float scaleFactor)
{
	Quaternion quaternion;
	quaternion.X = quaternion1.X * scaleFactor;
	quaternion.Y = quaternion1.Y * scaleFactor;
	quaternion.Z = quaternion1.Z * scaleFactor;
	quaternion.W = quaternion1.W * scaleFactor;

	return quaternion;
}
Quaternion Quaternion::Subtract( const Quaternion & quaternion1, const Quaternion & quaternion2)
{
	Quaternion quaternion;
	quaternion.X = quaternion1.X - quaternion2.X;
	quaternion.Y = quaternion1.Y - quaternion2.Y;
	quaternion.Z = quaternion1.Z - quaternion2.Z;
	quaternion.W = quaternion1.W - quaternion2.W;

	return quaternion;
}
Quaternion Quaternion::Negative( const Quaternion & quaternion)
{
	Quaternion x;
	x.X = -quaternion.X;
	x.Y = -quaternion.Y;
	x.Z = -quaternion.Z;
	x.W = -quaternion.W;

	return x;
}


Quaternion Quaternion::CreateFromAxisAngle( const Vector & axis, float angle)
{
	float value = angle * 0.5f;
	float value1 = sinf(value);
	float value2 = cosf(value);

	Quaternion quaternion;
	quaternion.X = axis.X * value1;
	quaternion.Y = axis.Y * value1;
	quaternion.Z = axis.Z * value1;
	quaternion.W = value2;
	return quaternion;
}

Quaternion Quaternion::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
{
	float value = roll * 0.5f;
	float value1 = sinf(value);
	float value2 = cosf(value);

	float value3 = pitch * 0.5f;
	float value4 = sinf(value3);
	float value5 = cosf(value3);

	float value6 = yaw * 0.5f;
	float value7 = sinf(value6);
	float value8 = cosf(value6);

	Quaternion quaternion;
	quaternion.X = value8 * value4 * value2 + value7 * value5 * value1;
	quaternion.Y = value7 * value5 * value2 - value8 * value4 * value1;
	quaternion.Z = value8 * value5 * value1 - value7 * value4 * value2;
	quaternion.W = value8 * value5 * value2 + value7 * value4 * value1;

	return quaternion;
}


Quaternion Quaternion::CreateFromRotationMatrix( const Matrix & matrix)
{
	float m11 = matrix.M11 + matrix.M22 + matrix.M33;

	Quaternion q;
	if (m11 > 0.0f)
	{
		float value = sqrtf((m11 + 1.0f));
		q.W = value * 0.5f;
		value = 0.5f / value;

		q.X = (matrix.M23 - matrix.M32) * value;
		q.Y = (matrix.M31 - matrix.M13) * value;
		q.Z = (matrix.M12 - matrix.M21) * value;
	}
	else if (matrix.M11 >= matrix.M22 && matrix.M11 >= matrix.M33)
	{
		float value1 = sqrtf((1.0f + matrix.M11 - matrix.M22 - matrix.M33));
		float value2 = 0.5f / value1;

		q.X = 0.5f * value1;
		q.Y = (matrix.M12 + matrix.M21) * value2;
		q.Z = (matrix.M13 + matrix.M31) * value2;
		q.W = (matrix.M23 - matrix.M32) * value2;
	}
	else if (matrix.M22 <= matrix.M33)
	{
		float value3 = sqrtf((1.0f + matrix.M33 - matrix.M11 - matrix.M22));
		float value4 = 0.5f / value3;

		q.X = (matrix.M31 + matrix.M13) * value4;
		q.Y = (matrix.M32 + matrix.M23) * value4;
		q.Z = 0.5f * value3;
		q.W = (matrix.M12 - matrix.M21) * value4;
	}
	else
	{
		float value5 = sqrtf((1.0f + matrix.M22 - matrix.M11 - matrix.M33));
		float value6 = 0.5f / value5;

		q.X = (matrix.M21 + matrix.M12) * value6;
		q.Y = 0.5f * value5;
		q.Z = (matrix.M32 + matrix.M23) * value6;
		q.W = (matrix.M31 - matrix.M13) * value6;
	}

	return q;
}


float Quaternion::Dot( const Quaternion & quaternion1, const Quaternion & quaternion2)
{
	return quaternion1.X * quaternion2.X + quaternion1.Y * quaternion2.Y + quaternion1.Z * quaternion2.Z + quaternion1.W * quaternion2.W;
}


Quaternion Quaternion::Slerp( const Quaternion & quaternion1, const Quaternion & quaternion2, float amount)
{
	float value;
	float value1;
	float value2 = amount;

	bool flag = false;
	float x = quaternion1.X * quaternion2.X + quaternion1.Y * quaternion2.Y + quaternion1.Z * quaternion2.Z + quaternion1.W * quaternion2.W;
	if (x < 0.0f)
	{
		flag = true;
		x = -x;
	}
	if (x <= 0.999999f)
	{
		float value3 = static_cast<float>(acos((double)x));
		float value4 = static_cast<float>(1 / sin((double)value3));
		value1 = sinf(((1.0f - value2) * value3)) * value4;
		value = (flag ? static_cast<float>(-sin((double)(value2 * value3))) * value4 : sinf((value2 * value3)) * value4);
	}
	else
	{
		value1 = 1.0f - value2;
		value = (flag ? -value2 : value2);
	}

	Quaternion quaternion;
	quaternion.X = value1 * quaternion1.X + value * quaternion2.X;
	quaternion.Y = value1 * quaternion1.Y + value * quaternion2.Y;
	quaternion.Z = value1 * quaternion1.Z + value * quaternion2.Z;
	quaternion.W = value1 * quaternion1.W + value * quaternion2.W;

	return quaternion;
}


Quaternion Quaternion::Lerp( const Quaternion & quaternion1, const Quaternion & quaternion2, float amount)
{
	float value = amount;
	float value1 = 1.0f - value;

	Quaternion q;
	if (quaternion1.X * quaternion2.X + quaternion1.Y * quaternion2.Y + quaternion1.Z * quaternion2.Z + quaternion1.W * quaternion2.W < 0.0f)
	{
		q.X = value1 * quaternion1.X - value * quaternion2.X;
		q.Y = value1 * quaternion1.Y - value * quaternion2.Y;
		q.Z = value1 * quaternion1.Z - value * quaternion2.Z;
		q.W = value1 * quaternion1.W - value * quaternion2.W;
	}
	else
	{
		q.X = value1 * quaternion1.X + value * quaternion2.X;
		q.Y = value1 * quaternion1.Y + value * quaternion2.Y;
		q.Z = value1 * quaternion1.Z + value * quaternion2.Z;
		q.W = value1 * quaternion1.W + value * quaternion2.W;
	}


	float q1 = q.X * q.X + q.Y * q.Y + q.Z * q.Z + q.W * q.W;
	float value2 = 1.0f / sqrtf(q1);

	q.X *= value2;
	q.Y *= value2;
	q.Z *= value2;
	q.W *= value2;

	return q;
}


Quaternion Quaternion::Concatenate( const Quaternion & quaternion1, const Quaternion & quaternion2)
{
	float x = quaternion2.X;
	float y = quaternion2.Y;
	float z = quaternion2.Z;
	float w = quaternion2.W;
	float value = quaternion1.X;

	float y1 = quaternion1.Y;
	float z1 = quaternion1.Z;
	float w1 = quaternion1.W;

	float value1 = y * z1 - z * y1;
	float value2 = z * value - x * z1;
	float value3 = x * y1 - y * value;
	float value4 = x * value + y * y1 + z * z1;

	Quaternion quaternion;
	quaternion.X = x * w1 + value * w + value1;
	quaternion.Y = y * w1 + y1 * w + value2;
	quaternion.Z = z * w1 + z1 * w + value3;
	quaternion.W = w * w1 - value4;

	return quaternion;
}