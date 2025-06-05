#include "Framework.h"
#include "Vector.h"

const Vector Vector::Zero = Vector(0.0f, 0.0f, 0.0f);
const Vector Vector::One = Vector(1.0f, 1.0f, 1.0f);

const Vector Vector::UnitX = Vector(1.0f, 0.0f, 0.0f);
const Vector Vector::UnitY = Vector(0.0f, 1.0f, 0.0f);
const Vector Vector::UnitZ = Vector(0.0f, 0.0f, 1.0f);

const Vector Vector::Up = Vector(0.0f, 1.0f, 0.0f);
const Vector Vector::Down = Vector(0.0f, -1.0f, 0.0f);
const Vector Vector::Right = Vector(1.0f, 0.0f, 0.0f);
const Vector Vector::Left = Vector(-1.0f, 0.0f, 0.0f);
const Vector Vector::Forward = Vector(0.0f, 0.0f, 1.0f);
const Vector Vector::Backward = Vector(0.0f, 0.0f, -1.0f);

Vector::Vector()
	: X(0.0f), Y(0.0f), Z(0.0f)
{
}

Vector::Vector(float value)
	: X(value), Y(value), Z(value)
{
}

Vector::Vector(float x, float y, float z)
	: X(x), Y(y), Z(z)
{
}

Vector::Vector( const Vector & v )
	: X(v.X), Y(v.Y), Z(v.Z)
{
}

Vector::Vector(const DirectX::PackedVector::XMHALF4 & Other)
{
	X = DirectX::PackedVector::XMConvertHalfToFloat(Other.x);
	Y = DirectX::PackedVector::XMConvertHalfToFloat(Other.y);
	Z = DirectX::PackedVector::XMConvertHalfToFloat(Other.z);
}

Vector& Vector::operator=( const Vector & v )
{
	if (this == &v)
		return *this;
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	return *this;
}

Vector::Vector( const aiVector3D & v )
	: X(v.x), Y(v.y), Z(v.z)
{
}

Vector Vector::operator-() const
{
	Vector Result;
	Result.X = -X;
	Result.Y = -Y;
	Result.Z = -Z;

	return Result;
}

Vector::operator float*()
{
	return V;
}

Vector::operator const float*() const
{
	return V;
}

Vector::operator DirectX::XMFLOAT3()
{
	return static_cast<DirectX::XMFLOAT3>(V);
}

Vector::operator const DirectX::XMFLOAT3() const
{
	return static_cast<const DirectX::XMFLOAT3>(V);
}

bool Vector::operator==(const Vector& value2) const
{
	if (X == value2.X && Y == value2.Y)
		return Z == value2.Z;
	else
		return false;
}

bool Vector::operator!=(const Vector& value2) const
{
	if (X == value2.X && Y == value2.Y)
		return Z != value2.Z;
	else
		return true;
}

Vector Vector::operator+(const Vector& value2) const
{
	Vector Result;
	Result.X = X + value2.X;
	Result.Y = Y + value2.Y;
	Result.Z = Z + value2.Z;

	return Result;
}

Vector Vector::operator-(const Vector& value2) const
{
	Vector Result;
	Result.X = X - value2.X;
	Result.Y = Y - value2.Y;
	Result.Z = Z - value2.Z;

	return Result;
}

Vector Vector::operator*(const Vector& value2) const
{
	Vector Result;
	Result.X = X * value2.X;
	Result.Y = Y * value2.Y;
	Result.Z = Z * value2.Z;

	return Result;
}

Vector Vector::operator*(const float& scaleFactor) const
{
	Vector Result;
	Result.X = X * scaleFactor;
	Result.Y = Y * scaleFactor;
	Result.Z = Z * scaleFactor;

	return Result;
}

Vector Vector::operator*(const Matrix& Matrix) const
{
	Vector Result;

	Result.X = Matrix.M11 * X + Matrix.M21 * Y + Matrix.M31 * Z + Matrix.M41;
	Result.Y = Matrix.M12 * X + Matrix.M22 * Y + Matrix.M32 * Z + Matrix.M42;
	Result.Z = Matrix.M13 * X + Matrix.M23 * Y + Matrix.M33 * Z + Matrix.M43;

	return Result;
}

Vector Vector::operator/(const Vector& value2) const
{
	Vector Result;
	Result.X = X / value2.X;
	Result.Y = Y / value2.Y;
	Result.Z = Z / value2.Z;

	return Result;
}

Vector Vector::operator/(const float& divider) const
{
	float num = 1.0f / divider;

	Vector Result;
	Result.X = X * num;
	Result.Y = Y * num;
	Result.Z = Z * num;

	return Result;
}

void Vector::operator+=(const Vector& value2)
{
	*this = *this + value2;
}

void Vector::operator-=(const Vector& value2)
{
	*this = *this - value2;
}

void Vector::operator*=(const Vector& value2)
{
	*this = *this * value2;
}

void Vector::operator*=(const float& scaleFactor)
{
	*this = *this * scaleFactor;
}

void Vector::operator/=(const Vector& value2)
{
	*this = *this / value2;
}

void Vector::operator/=(const float& divider)
{
	*this = *this / divider;
}

std::wstring Vector::ToWString() const
{
	std::wstring temp = L"";
	temp += L"X:" + std::to_wstring(X);
	temp += L",Y:" + std::to_wstring(Y);
	temp += L",Z:" + std::to_wstring(Z);

	return temp;
}

std::string Vector::ToString() const
{
	std::string temp = "";
	temp += "X:" + std::to_string(X);
	temp += ",Y:" + std::to_string(Y);
	temp += ",Z:" + std::to_string(Z);

	return temp;
}

float Vector::Length() const
{
	float x = X * X + Y * Y + Z * Z;

	return sqrtf(x);
}

float Vector::LengthSquared() const
{
	return X * X + Y * Y + Z * Z;
}

void Vector::Normalize()
{
	float x = X * X + Y * Y + Z * Z;
	float single;
	if (x < Math::Epsilon)
		single = 0;
	else
		single = 1.0f / sqrtf(x);

	X *= single;
	Y *= single;
	Z *= single;
}

Vector Vector::Add( const Vector & value1, const Vector & value2)
{
	Vector Result;
	Result.X = value1.X + value2.X;
	Result.Y = value1.Y + value2.Y;
	Result.Z = value1.Z + value2.Z;

	return Result;
}

Vector Vector::Divide( const Vector & value1, const Vector & value2)
{
	Vector Result;
	Result.X = value1.X / value2.X;
	Result.Y = value1.Y / value2.Y;
	Result.Z = value1.Z / value2.Z;

	return Result;
}

Vector Vector::Divide( const Vector & value1, float value2)
{
	float single = 1.0f / value2;

	Vector Result;
	Result.X = value1.X * single;
	Result.Y = value1.Y * single;
	Result.Z = value1.Z * single;

	return Result;
}

Vector Vector::Multiply( const Vector & value1, const Vector & value2)
{
	Vector Result;
	Result.X = value1.X * value2.X;
	Result.Y = value1.Y * value2.Y;
	Result.Z = value1.Z * value2.Z;

	return Result;
}

Vector Vector::Multiply( const Vector & value1, float scaleFactor)
{
	Vector Result;
	Result.X = value1.X * scaleFactor;
	Result.Y = value1.Y * scaleFactor;
	Result.Z = value1.Z * scaleFactor;

	return Result;
}

Vector Vector::Subtract( const Vector & value1, const Vector & value2)
{
	Vector Result;
	Result.X = value1.X - value2.X;
	Result.Y = value1.Y - value2.Y;
	Result.Z = value1.Z - value2.Z;

	return Result;
}

Vector Vector::Abs(const Vector& value)
{
	Vector Result;
	Result.X = abs(value.X);
	Result.Y = abs(value.Y);
	Result.Z = abs(value.Z);

	return Result;
}

Vector Vector::Negative( const Vector & value)
{
	Vector Result;
	Result.X = -value.X;
	Result.Y = -value.Y;
	Result.Z = -value.Z;

	return Result;
}

Vector Vector::Barycentric( const Vector & value1, const Vector & value2, const Vector & value3, float amount1, float amount2)
{
	Vector Result;
	Result.X = value1.X + amount1 * (value2.X - value1.X) + amount2 * (value3.X - value1.X);
	Result.Y = value1.Y + amount1 * (value2.Y - value1.Y) + amount2 * (value3.Y - value1.Y);
	Result.Z = value1.Z + amount1 * (value2.Z - value1.Z) + amount2 * (value3.Z - value1.Z);

	return Result;
}

float Vector::Distance( const Vector & value1, const Vector & value2)
{
	float x = value1.X - value2.X;
	float y = value1.Y - value2.Y;
	float z = value1.Z - value2.Z;
	float single = x * x + y * y + z * z;

	return sqrtf(single);
}

float Vector::DistanceSquared( const Vector & value1, const Vector & value2)
{
	float x = value1.X - value2.X;
	float y = value1.Y - value2.Y;
	float z = value1.Z - value2.Z;

	return x * x + y * y + z * z;
}

float Vector::Dot( const Vector & value1, const Vector & value2)
{
	return value1.X * value2.X + value1.Y * value2.Y + value1.Z * value2.Z;
}

Vector Vector::Normalize( const Vector & value)
{
	const float Length = sqrt(value.X * value.X + value.Y * value.Y + value.Z * value.Z);
	if (Length < Math::Epsilon)
		return Zero;

	const float LengthInverse = 1.0f / Length;

	Vector Result;
	Result.X = value.X * LengthInverse;
	Result.Y = value.Y * LengthInverse;
	Result.Z = value.Z * LengthInverse;

	return Result;
}

Vector Vector::Cross( const Vector & vector1, const Vector & vector2)
{
	Vector Result;
	Result.X = vector1.Y * vector2.Z - vector1.Z * vector2.Y;
	Result.Y = vector1.Z * vector2.X - vector1.X * vector2.Z;
	Result.Z = vector1.X * vector2.Y - vector1.Y * vector2.X;

	return Result;
}

Vector Vector::Reflect( const Vector & vector, const Vector & normal)
{
	float x = vector.X * normal.X + vector.Y * normal.Y + vector.Z * normal.Z;

	Vector Result;
	Result.X = vector.X - 2.0f * x * normal.X;
	Result.Y = vector.Y - 2.0f * x * normal.Y;
	Result.Z = vector.Z - 2.0f * x * normal.Z;
	return Result;
}

Vector Vector::Min( const Vector & value1, const Vector & value2)
{
	Vector Result;
	Result.X = value1.X < value2.X ? value1.X : value2.X;
	Result.Y = value1.Y < value2.Y ? value1.Y : value2.Y;
	Result.Z = value1.Z < value2.Z ? value1.Z : value2.Z;

	return Result;
}

Vector Vector::Max( const Vector & value1, const Vector & value2)
{
	Vector Result;
	Result.X = value1.X > value2.X ? value1.X : value2.X;
	Result.Y = value1.Y > value2.Y ? value1.Y : value2.Y;
	Result.Z = value1.Z > value2.Z ? value1.Z : value2.Z;

	return Result;
}

Vector Vector::Clamp( const Vector & value1, const Vector & min, const Vector & max)
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

	Vector Result;
	Result.X = x;
	Result.Y = y;
	Result.Z = z;

	return Result;
}

Vector Vector::Lerp( const Vector & value1, const Vector & value2, float amount)
{
	Vector Result;
	Result.X = value1.X + (value2.X - value1.X) * amount;
	Result.Y = value1.Y + (value2.Y - value1.Y) * amount;
	Result.Z = value1.Z + (value2.Z - value1.Z) * amount;

	return Result;
}

Vector Vector::SmoothStep( const Vector & value1, const Vector & value2, float amount)
{
	float single;
	if (amount > 1.0f)
		single = 1.0f;
	else
		single = (amount < 0.0f ? 0.0f : amount);


	amount = single;
	amount = amount * amount * (3.0f - 2.0f * amount);

	Vector Result;
	Result.X = value1.X + (value2.X - value1.X) * amount;
	Result.Y = value1.Y + (value2.Y - value1.Y) * amount;
	Result.Z = value1.Z + (value2.Z - value1.Z) * amount;

	return Result;
}

Vector Vector::CatmullRom( const Vector & value1, const Vector & value2, const Vector & value3, const Vector & value4, float amount)
{
	float single = amount * amount;
	float single1 = amount * single;

	Vector Result;
	Result.X = 0.5f * (2.0f * value2.X + (-value1.X + value3.X) * amount + (2.0f * value1.X - 5.0f * value2.X + 4.0f * value3.X - value4.X) * single + (-value1.X + 3.0f * value2.X - 3.0f * value3.X + value4.X) * single1);
	Result.Y = 0.5f * (2.0f * value2.Y + (-value1.Y + value3.Y) * amount + (2.0f * value1.Y - 5.0f * value2.Y + 4.0f * value3.Y - value4.Y) * single + (-value1.Y + 3.0f * value2.Y - 3.0f * value3.Y + value4.Y) * single1);
	Result.Z = 0.5f * (2.0f * value2.Z + (-value1.Z + value3.Z) * amount + (2.0f * value1.Z - 5.0f * value2.Z + 4.0f * value3.Z - value4.Z) * single + (-value1.Z + 3.0f * value2.Z - 3.0f * value3.Z + value4.Z) * single1);

	return Result;
}

Vector Vector::Hermite( const Vector & value1, const Vector & tangent1, const Vector & value2, const Vector & tangent2, float amount)
{
	float single = amount * amount;
	float single1 = amount * single;
	float single2 = 2.0f * single1 - 3.0f * single + 1.0f;
	float single3 = -2.0f * single1 + 3.0f * single;
	float single4 = single1 - 2.0f * single + amount;
	float single5 = single1 - single;

	Vector Result;
	Result.X = value1.X * single2 + value2.X * single3 + tangent1.X * single4 + tangent2.X * single5;
	Result.Y = value1.Y * single2 + value2.Y * single3 + tangent1.Y * single4 + tangent2.Y * single5;
	Result.Z = value1.Z * single2 + value2.Z * single3 + tangent1.Z * single4 + tangent2.Z * single5;

	return Result;
}

Vector Vector::Transform( const Vector & position, const Matrix & matrix)
{
	float x = position.X * matrix.M11 + position.Y * matrix.M21 + position.Z * matrix.M31 + matrix.M41;
	float single = position.X * matrix.M12 + position.Y * matrix.M22 + position.Z * matrix.M32 + matrix.M42;
	float x1 = position.X * matrix.M13 + position.Y * matrix.M23 + position.Z * matrix.M33 + matrix.M43;

	Vector Result;
	Result.X = x;
	Result.Y = single;
	Result.Z = x1;

	return Result;
}

Vector Vector::Transform( const Vector & value, const Quaternion & rotation)
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

	Vector Result;
	Result.X = single2;
	Result.Y = x3;
	Result.Z = single3;

	return Result;
}

Vector Vector::TransformNormal( const Vector & normal, const Matrix & matrix)
{
	float x = normal.X * matrix.M11 + normal.Y * matrix.M21 + normal.Z * matrix.M31;
	float single = normal.X * matrix.M12 + normal.Y * matrix.M22 + normal.Z * matrix.M32;
	float x1 = normal.X * matrix.M13 + normal.Y * matrix.M23 + normal.Z * matrix.M33;

	Vector Result;
	Result.X = x;
	Result.Y = single;
	Result.Z = x1;

	return Result;
}

Vector Vector::TransformCoord( const Vector & position, const Matrix & matrix)
{
	float single1 = (matrix.M11 * position.X) + (matrix.M21 * position.Y) + (matrix.M31 * position.Z) + matrix.M41;
	float single2 = (matrix.M12 * position.X) + (matrix.M22 * position.Y) + (matrix.M32 * position.Z) + matrix.M42;
	float single3 = (matrix.M13 * position.X) + (matrix.M23 * position.Y) + (matrix.M33 * position.Z) + matrix.M43;
	float single4 = (matrix.M14 * position.X) + (matrix.M24 * position.Y) + (matrix.M34 * position.Z) + matrix.M44;

	return {(single1 / single4), (single2 / single4), (single3 / single4)};
}

DirectX::PackedVector::XMHALF4 Vector::GetPackedVectorHalf4(const Vector& Value)
{
	DirectX::PackedVector::XMHALF4 PackedVector;
	PackedVector.x = DirectX::PackedVector::XMConvertFloatToHalf(Value.X);
	PackedVector.y = DirectX::PackedVector::XMConvertFloatToHalf(Value.Y);
	PackedVector.z = DirectX::PackedVector::XMConvertFloatToHalf(Value.Z);
	PackedVector.w = DirectX::PackedVector::XMConvertFloatToHalf(0.0f);
	return PackedVector;
}

Vector Vector::Random(float MinLength, float MaxLength)
{
	Vector V;
	V.X = Math::Random(-1.f, 1.f);
	V.Y = Math::Random(-1.f, 1.f);
	V.Z = Math::Random(-1.f, 1.f);
	V.Normalize();
	float scaler = Math::Random(MinLength, MaxLength);
	return V * scaler;
}
