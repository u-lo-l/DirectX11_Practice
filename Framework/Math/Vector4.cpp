﻿#include "Framework.h"
#include "Vector4.h"


const Vector4 Vector4::Zero = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::One = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

const Vector4 Vector4::UnitX = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::UnitY = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
const Vector4 Vector4::UnitZ = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
const Vector4 Vector4::UnitW = Vector4(0.0f, 0.0f, 0.0f, 1.0f);



Vector4::Vector4()
	: X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
{
}


Vector4::Vector4(float x, float y, float z, float w)
	: X(x), Y(y), Z(z), W(w)
{
}


Vector4::Vector4(Vector2D Value, float z, float w)
	: X(Value.X), Y(Value.Y), Z(z), W(w)
{
}


Vector4::Vector4( const Vector & value, float w)
	: X(value.X), Y(value.Y), Z(value.Z), W(w)
{
}

Vector4::Vector4( const Vector4 & other )
	: X(other.X), Y(other.Y), Z(other.Z), W(other.W)
{
}

Vector4 & Vector4::operator=( const Vector4 & other )
{
	if (this == &other)
		return *this;
	X = other.X;
	Y = other.Y;
	Z = other.Z;
	W = other.W;
	return *this;
}



Vector4::Vector4(float value)
{
	X = Y = Z = W = value;
}

Vector4::operator float*()
{
	return V;
}

Vector4::operator const float*() const
{
	return V;
}



Vector4 Vector4::operator-()
{
	Vector4 vector4;
	vector4.X = -X;
	vector4.Y = -Y;
	vector4.Z = -Z;
	vector4.W = -W;

	return vector4;
}


bool Vector4::operator==(const Vector4& value2) const
{
	if (X == value2.X && Y == value2.Y && Z == value2.Z)
		return W == value2.W;
	else
		return false;
}


bool Vector4::operator!=(const Vector4& value2) const
{
	if (X == value2.X && Y == value2.Y && Z == value2.Z)
		return W != value2.W;
	else
		return true;
}


Vector4 Vector4::operator+(const Vector4& value2) const
{
	Vector4 vector4;
	vector4.X = X + value2.X;
	vector4.Y = Y + value2.Y;
	vector4.Z = Z + value2.Z;
	vector4.W = W + value2.W;

	return vector4;
}


Vector4 Vector4::operator-(const Vector4& value2) const
{
	Vector4 vector4;
	vector4.X = X - value2.X;
	vector4.Y = Y - value2.Y;
	vector4.Z = Z - value2.Z;
	vector4.W = W - value2.W;

	return vector4;
}


Vector4 Vector4::operator*(const Vector4& value2) const
{
	Vector4 vector4;
	vector4.X = X * value2.X;
	vector4.Y = Y * value2.Y;
	vector4.Z = Z * value2.Z;
	vector4.W = W * value2.W;

	return vector4;
}


Vector4 Vector4::operator*(const float& scaleFactor) const
{
	Vector4 vector4;
	vector4.X = X * scaleFactor;
	vector4.Y = Y * scaleFactor;
	vector4.Z = Z * scaleFactor;
	vector4.W = W * scaleFactor;

	return vector4;
}


Vector4 Vector4::operator/(const Vector4& value2) const
{
	Vector4 vector4;
	vector4.X = X / value2.X;
	vector4.Y = Y / value2.Y;
	vector4.Z = Z / value2.Z;
	vector4.W = W / value2.W;

	return vector4;
}


Vector4 Vector4::operator/(const float& divider) const
{
	float num = 1.0f / divider;

	Vector4 vector4;
	vector4.X = X * num;
	vector4.Y = Y * num;
	vector4.Z = Z * num;
	vector4.W = W * num;

	return vector4;
}


void Vector4::operator+=(const Vector4& value2)
{
	*this = *this + value2;
}

 
void Vector4::operator-=(const Vector4& value2)
{
	*this = *this - value2;
}

 
void Vector4::operator*=(const Vector4& value2)
{
	*this = *this * value2;
}


void Vector4::operator*=(const float& scaleFactor)
{
	*this = *this * scaleFactor;
}


void Vector4::operator/=(const Vector4& value2)
{
	*this = *this / value2;
}


void Vector4::operator/=(const float& divider)
{
	*this = *this / divider;
}


std::wstring Vector4::ToWString() const
{
	std::wstring temp;

	temp += L"X:" + std::to_wstring(X);
	temp += L",Y:" + std::to_wstring(Y);
	temp += L",Z:" + std::to_wstring(Z);
	temp += L",W:" + std::to_wstring(W);

	return temp;
}

float Vector4::Length()
{
	float x = X * X + Y * Y + Z * Z + W * W;

	return sqrtf(x);
}


float Vector4::LengthSquared()
{
	return X * X + Y * Y + Z * Z + W * W;
}


void Vector4::Normalize()
{
	float x = X * X + Y * Y + Z * Z + W * W;
	float single = 1.0f / sqrtf(x);

	X *= single;
	Y *= single;
	Z *= single;
	W *= single;
}

Vector4 Vector4::Add( const Vector4 & value1, const Vector4 & value2)
{
	Vector4 vector4;
	vector4.X = value1.X + value2.X;
	vector4.Y = value1.Y + value2.Y;
	vector4.Z = value1.Z + value2.Z;
	vector4.W = value1.W + value2.W;

	return vector4;
}
Vector4 Vector4::Divide( const Vector4 & value1, const Vector4 & value2)
{
	Vector4 vector4;
	vector4.X = value1.X / value2.X;
	vector4.Y = value1.Y / value2.Y;
	vector4.Z = value1.Z / value2.Z;
	vector4.W = value1.W / value2.W;

	return vector4;
}
Vector4 Vector4::Divide( const Vector4 & value1, float divider)
{
	float single = 1.0f / divider;

	Vector4 vector4;
	vector4.X = value1.X * single;
	vector4.Y = value1.Y * single;
	vector4.Z = value1.Z * single;
	vector4.W = value1.W * single;

	return vector4;
}
Vector4 Vector4::Multiply( const Vector4 & value1, const Vector4 & value2)
{
	Vector4 vector4;
	vector4.X = value1.X * value2.X;
	vector4.Y = value1.Y * value2.Y;
	vector4.Z = value1.Z * value2.Z;
	vector4.W = value1.W * value2.W;

	return vector4;
}

Vector4 Vector4::Multiply( const Vector4 & value1, float scaleFactor)
{
	Vector4 vector4;
	vector4.X = value1.X * scaleFactor;
	vector4.Y = value1.Y * scaleFactor;
	vector4.Z = value1.Z * scaleFactor;
	vector4.W = value1.W * scaleFactor;
	return vector4;
}

Vector4 Vector4::Subtract( const Vector4 & value1, const Vector4 & value2)
{
	Vector4 vector4;
	vector4.X = value1.X - value2.X;
	vector4.Y = value1.Y - value2.Y;
	vector4.Z = value1.Z - value2.Z;
	vector4.W = value1.W - value2.W;

	return vector4;
}

Vector4 Vector4::Negative( const Vector4 & value)
{
	Vector4 vector4;
	vector4.X = -value.X;
	vector4.Y = -value.Y;
	vector4.Z = -value.Z;
	vector4.W = -value.W;

	return vector4;
}

Vector4 Vector4::Barycentric( const Vector4 & value1, const Vector4 & value2, const Vector4 & value3, float amount1, float amount2)
{
	Vector4 vector4;
	vector4.X = value1.X + amount1 * (value2.X - value1.X) + amount2 * (value3.X - value1.X);
	vector4.Y = value1.Y + amount1 * (value2.Y - value1.Y) + amount2 * (value3.Y - value1.Y);
	vector4.Z = value1.Z + amount1 * (value2.Z - value1.Z) + amount2 * (value3.Z - value1.Z);
	vector4.W = value1.W + amount1 * (value2.W - value1.W) + amount2 * (value3.W - value1.W);

	return vector4;
}


float Vector4::Distance( const Vector4 & value1, const Vector4 & value2)
{
	float x = value1.X - value2.X;
	float y = value1.Y - value2.Y;
	float z = value1.Z - value2.Z;
	float w = value1.W - value2.W;

	float single = x * x + y * y + z * z + w * w;

	return sqrtf(single);
}


float Vector4::DistanceSquared( const Vector4 & value1, const Vector4 & value2)
{
	float x = value1.X - value2.X;
	float y = value1.Y - value2.Y;
	float z = value1.Z - value2.Z;
	float w = value1.W - value2.W;

	return x * x + y * y + z * z + w * w;
}


float Vector4::Dot( const Vector4 & value1, const Vector4 & value2)
{
	return value1.X * value2.X + value1.Y * value2.Y + value1.Z * value2.Z + value1.W * value2.W;
}


Vector4 Vector4::Normalize( const Vector4 & value)
{
	float num = 1.0f / sqrtf(value.X * value.X + value.Y * value.Y + value.Z * value.Z + value.W * value.W);

	Vector4 vector4;
	vector4.X = value.X * num;
	vector4.Y = value.Y * num;
	vector4.Z = value.Z * num;
	vector4.W = value.W * num;

	return vector4;
}


Vector4 Vector4::Min( const Vector4 & value1, const Vector4 & value2)
{
	Vector4 vector4;
	vector4.X = value1.X < value2.X ? value1.X : value2.X;
	vector4.Y = value1.Y < value2.Y ? value1.Y : value2.Y;
	vector4.Z = value1.Z < value2.Z ? value1.Z : value2.Z;
	vector4.W = value1.W < value2.W ? value1.W : value2.W;

	return vector4;
}


Vector4 Vector4::Max( const Vector4 & value1, const Vector4 & value2)
{
	Vector4 vector4;
	vector4.X = value1.X > value2.X ? value1.X : value2.X;
	vector4.Y = value1.Y > value2.Y ? value1.Y : value2.Y;
	vector4.Z = value1.Z > value2.Z ? value1.Z : value2.Z;
	vector4.W = value1.W > value2.W ? value1.W : value2.W;

	return vector4;
}


Vector4 Vector4::Clamp( const Vector4 & value1, const Vector4 & min, const Vector4 & max)
{
	float x = value1.X;
	x = (x > max.X ? max.X : x);
	x = (x < min.X ? min.X : x);

	float y = value1.Y;
	y = (y > max.Y ? max.Y : y);
	y = (y < min.Y ? min.Y : y);

	float z = value1.Z;
	z = (z > max.Z ? max.Z : z);
	z = (z < min.Z ? min.Z : z);

	float w = value1.W;
	w = (w > max.W ? max.W : w);
	w = (w < min.W ? min.W : w);

	Vector4 vector4;
	vector4.X = x;
	vector4.Y = y;
	vector4.Z = z;
	vector4.W = w;
	return vector4;
}


Vector4 Vector4::Lerp( const Vector4 & value1, const Vector4 & value2, float amount)
{
	Vector4 vector4;
	vector4.X = value1.X + (value2.X - value1.X) * amount;
	vector4.Y = value1.Y + (value2.Y - value1.Y) * amount;
	vector4.Z = value1.Z + (value2.Z - value1.Z) * amount;
	vector4.W = value1.W + (value2.W - value1.W) * amount;

	return vector4;
}


Vector4 Vector4::SmoothStep( const Vector4 & value1, const Vector4 & value2, float amount)
{
	float single;
	if (amount > 1.0f)
		single = 1.0f;
	else
		single = (amount < 0.0f ? 0.0f : amount);

	amount = single;
	amount = amount * amount * (3.0f - 2.0f * amount);

	Vector4 vector4;
	vector4.X = value1.X + (value2.X - value1.X) * amount;
	vector4.Y = value1.Y + (value2.Y - value1.Y) * amount;
	vector4.Z = value1.Z + (value2.Z - value1.Z) * amount;
	vector4.W = value1.W + (value2.W - value1.W) * amount;

	return vector4;
}


Vector4 Vector4::CatmullRom( const Vector4 & value1, const Vector4 & value2, const Vector4 & value3, const Vector4 & value4, float amount)
{
	float single = amount * amount;
	float single1 = amount * single;

	Vector4 vector4;
	vector4.X = 0.5f * (2.0f * value2.X + (-value1.X + value3.X) * amount + (2.0f * value1.X - 5.0f * value2.X + 4.0f * value3.X - value4.X) * single + (-value1.X + 3.0f * value2.X - 3.0f * value3.X + value4.X) * single1);
	vector4.Y = 0.5f * (2.0f * value2.Y + (-value1.Y + value3.Y) * amount + (2.0f * value1.Y - 5.0f * value2.Y + 4.0f * value3.Y - value4.Y) * single + (-value1.Y + 3.0f * value2.Y - 3.0f * value3.Y + value4.Y) * single1);
	vector4.Z = 0.5f * (2.0f * value2.Z + (-value1.Z + value3.Z) * amount + (2.0f * value1.Z - 5.0f * value2.Z + 4.0f * value3.Z - value4.Z) * single + (-value1.Z + 3.0f * value2.Z - 3.0f * value3.Z + value4.Z) * single1);
	vector4.W = 0.5f * (2.0f * value2.W + (-value1.W + value3.W) * amount + (2.0f * value1.W - 5.0f * value2.W + 4.0f * value3.W - value4.W) * single + (-value1.W + 3.0f * value2.W - 3.0f * value3.W + value4.W) * single1);

	return vector4;
}


Vector4 Vector4::Hermite( const Vector4 & value1, const Vector4 & tangent1, const Vector4 & value2, const Vector4 & tangent2, float amount)
{
	float single = amount * amount;
	float single1 = amount * single;
	float single2 = 2.0f * single1 - 3.0f * single + 1.0f;
	float single3 = -2.0f * single1 + 3.0f * single;
	float single4 = single1 - 2.0f * single + amount;
	float single5 = single1 - single;

	Vector4 vector4;
	vector4.X = value1.X * single2 + value2.X * single3 + tangent1.X * single4 + tangent2.X * single5;
	vector4.Y = value1.Y * single2 + value2.Y * single3 + tangent1.Y * single4 + tangent2.Y * single5;
	vector4.Z = value1.Z * single2 + value2.Z * single3 + tangent1.Z * single4 + tangent2.Z * single5;
	vector4.W = value1.W * single2 + value2.W * single3 + tangent1.W * single4 + tangent2.W * single5;

	return vector4;
}


Vector4 Vector4::Transform( const Vector2D position, const Matrix & matrix)
{
	float num1 = (position.X * matrix.M11 + position.Y * matrix.M21) + matrix.M41;
	float num2 = (position.X * matrix.M12 + position.Y * matrix.M22) + matrix.M42;
	float num3 = (position.X * matrix.M13 + position.Y * matrix.M23) + matrix.M43;
	float num4 = (position.X * matrix.M14 + position.Y * matrix.M24) + matrix.M44;

	Vector4 vector4;
	vector4.X = num1;
	vector4.Y = num2;
	vector4.Z = num3;
	vector4.W = num4;

	return vector4;
}


Vector4 Vector4::Transform( const Vector & position, const Matrix & matrix)
{
	float num1 = (position.X * matrix.M11 + position.Y * matrix.M21 + position.Z * matrix.M31) + matrix.M41;
	float num2 = (position.X * matrix.M12 + position.Y * matrix.M22 + position.Z * matrix.M32) + matrix.M42;
	float num3 = (position.X * matrix.M13 + position.Y * matrix.M23 + position.Z * matrix.M33) + matrix.M43;
	float num4 = (position.X * matrix.M14 + position.Y * matrix.M24 + position.Z * matrix.M34) + matrix.M44;

	Vector4 vector4;
	vector4.X = num1;
	vector4.Y = num2;
	vector4.Z = num3;
	vector4.W = num4;

	return vector4;
}


Vector4 Vector4::Transform( const Vector4 & vector, const Matrix & matrix)
{
	float num1 = (vector.X * matrix.M11 + vector.Y * matrix.M21 + vector.Z * matrix.M31 + vector.W * matrix.M41);
	float num2 = (vector.X * matrix.M12 + vector.Y * matrix.M22 + vector.Z * matrix.M32 + vector.W * matrix.M42);
	float num3 = (vector.X * matrix.M13 + vector.Y * matrix.M23 + vector.Z * matrix.M33 + vector.W * matrix.M43);
	float num4 = (vector.X * matrix.M14 + vector.Y * matrix.M24 + vector.Z * matrix.M34 + vector.W * matrix.M44);

	Vector4 vector4;
	vector4.X = num1;
	vector4.Y = num2;
	vector4.Z = num3;
	vector4.W = num4;

	return vector4;
}


Vector4 Vector4::Transform( const Vector2D value, const Quaternion & rotation)
{
	float x = rotation.X + rotation.X;
	float y = rotation.Y + rotation.Y;
	float z = rotation.Z + rotation.Z;
	float w = rotation.W * x;
	float single = rotation.W * y;

	float w1 = rotation.W * z;
	float x1 = rotation.X * x;
	float single1 = rotation.X * y;

	float x2 = rotation.X * z;
	float y1 = rotation.Y * y;
	float y2 = rotation.Y * z;
	float z1 = rotation.Z * z;
	float single2 = value.X * (1.0f - y1 - z1) + value.Y * (single1 - w1);

	float x3 = value.X * (single1 + w1) + value.Y * (1.0f - x1 - z1);
	float single3 = value.X * (x2 - single) + value.Y * (y2 + w);

	Vector4 vector4;
	vector4.X = single2;
	vector4.Y = x3;
	vector4.Z = single3;
	vector4.W = 1.0f;

	return vector4;
}


Vector4 Vector4::Transform( const Vector & value, const Quaternion & rotation)
{
	float x = rotation.X + rotation.X;
	float y = rotation.Y + rotation.Y;
	float z = rotation.Z + rotation.Z;
	float w = rotation.W * x;
	float single = rotation.W * y;

	float w1 = rotation.W * z;
	float x1 = rotation.X * x;
	float single1 = rotation.X * y;

	float x2 = rotation.X * z;
	float y1 = rotation.Y * y;
	float y2 = rotation.Y * z;
	float z1 = rotation.Z * z;
	float single2 = value.X * (1.0f - y1 - z1) + value.Y * (single1 - w1) + value.Z * (x2 + single);

	float x3 = value.X * (single1 + w1) + value.Y * (1.0f - x1 - z1) + value.Z * (y2 - w);
	float single3 = value.X * (x2 - single) + value.Y * (y2 + w) + value.Z * (1.0f - x1 - y1);

	Vector4 vector4;
	vector4.X = single2;
	vector4.Y = x3;
	vector4.Z = single3;
	vector4.W = 1.0f;

	return vector4;
}


Vector4 Vector4::Transform( const Vector4 & value, const Quaternion & rotation)
{
	float x = rotation.X + rotation.X;
	float y = rotation.Y + rotation.Y;
	float z = rotation.Z + rotation.Z;
	float w = rotation.W * x;
	float single = rotation.W * y;

	float w1 = rotation.W * z;
	float x1 = rotation.X * x;
	float single1 = rotation.X * y;

	float x2 = rotation.X * z;
	float y1 = rotation.Y * y;
	float y2 = rotation.Y * z;
	float z1 = rotation.Z * z;
	float single2 = value.X * (1.0f - y1 - z1) + value.Y * (single1 - w1) + value.Z * (x2 + single);

	float x3 = value.X * (single1 + w1) + value.Y * (1.0f - x1 - z1) + value.Z * (y2 - w);
	float single3 = value.X * (x2 - single) + value.Y * (y2 + w) + value.Z * (1.0f - x1 - y1);

	Vector4 vector4;
	vector4.X = single2;
	vector4.Y = x3;
	vector4.Z = single3;
	vector4.W = value.W;
	return vector4;
}