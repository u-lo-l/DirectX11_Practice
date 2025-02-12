#include "Framework.h"
#include "Vector2D.h"


const Vector2D Vector2D::Zero = Vector2D(0.0f, 0.0f);
const Vector2D Vector2D::One = Vector2D(1.0f, 1.0f);
const Vector2D Vector2D::UnitX = Vector2D(1.0f, 0.0f);
const Vector2D Vector2D::UnitY = Vector2D(0.0f, 1.0f);



Vector2D::Vector2D(void)
{
	X = 0.0f;
	Y = 0.0f;
}


Vector2D::Vector2D(float value)
{
	X = Y = value;
}


Vector2D::Vector2D(float x, float y)
{
	X = x;
	Y = y;
}

Vector2D::operator float*()
{
	return V;
}

Vector2D::operator const float*() const
{
	return V;
}



Vector2D Vector2D::operator-()
{
	Vector2D vector2;
	vector2.X = -X;
	vector2.Y = -Y;

	return vector2;
}


bool Vector2D::operator==(const Vector2D& value2) const
{
	if (X == value2.X)
		return Y == value2.Y;
	else
		return false;
}


bool Vector2D::operator!=(const Vector2D& value2) const
{
	if (X == value2.X)
		return Y != value2.Y;
	else
		return true;
}


Vector2D Vector2D::operator+(const Vector2D& value2) const
{
	Vector2D vector2;
	vector2.X = X + value2.X;
	vector2.Y = Y + value2.Y;

	return vector2;
}


Vector2D Vector2D::operator-(const Vector2D& value2) const
{
	Vector2D vector2;
	vector2.X = X - value2.X;
	vector2.Y = Y - value2.Y;

	return vector2;
}


Vector2D Vector2D::operator*(const Vector2D& value2) const
{
	Vector2D vector2;
	vector2.X = X * value2.X;
	vector2.Y = Y * value2.Y;

	return vector2;
}


Vector2D Vector2D::operator*(const float& scaleFactor) const
{
	Vector2D vector2;
	vector2.X = X * scaleFactor;
	vector2.Y = Y * scaleFactor;

	return vector2;
}


Vector2D Vector2D::operator/(const Vector2D& value2) const
{
	Vector2D vector2;
	vector2.X = X / value2.X;
	vector2.Y = Y / value2.Y;

	return vector2;
}


Vector2D Vector2D::operator/(const float& divider) const
{
	float num = 1.0f / divider;

	Vector2D vector2;
	vector2.X = X * num;
	vector2.Y = Y * num;

	return vector2;
}


void Vector2D::operator+=(const Vector2D& value2)
{
	*this = *this + value2;
}

 
void Vector2D::operator-=(const Vector2D& value2)
{
	*this = *this - value2;
}

 
void Vector2D::operator*=(const Vector2D& value2)
{
	*this = *this * value2;
}


void Vector2D::operator*=(const float& scaleFactor)
{
	*this = *this * scaleFactor;
}


void Vector2D::operator/=(const Vector2D& value2)
{
	*this = *this / value2;
}


void Vector2D::operator/=(const float& divider)
{
	*this = *this / divider;
}


std::string Vector2D::ToString() const
{
	std::string Temp;

	Temp += "X:" + std::to_string(X);
	Temp += ",Y:" + std::to_string(Y);

	return Temp;
}

std::wstring Vector2D::ToWString() const
{
	std::wstring Temp;

	Temp += L"X:" + std::to_wstring(X);
	Temp += L",Y:" + std::to_wstring(Y);

	return Temp;
}

float Vector2D::Length() const
{
	float x = X * X + Y * Y;

	return sqrtf(x);
}


float Vector2D::LengthSquared() const
{
	return X * X + Y * Y;
}


void Vector2D::Normalize()
{
	float x = X * X + Y * Y;
	float single = 1.0f / sqrtf(x);

	X *= single;
	Y *= single;
}


Vector2D Vector2D::Add(Vector2D value1, Vector2D value2)
{
	Vector2D vector2;
	vector2.X = value1.X + value2.X;
	vector2.Y = value1.Y + value2.Y;

	return vector2;
}


Vector2D Vector2D::Divide(Vector2D value1, Vector2D value2)
{
	Vector2D vector2;
	vector2.X = value1.X / value2.X;
	vector2.Y = value1.Y / value2.Y;

	return vector2;
}

Vector2D Vector2D::Divide(Vector2D value1, float divider)
{
	float single = 1.0f / divider;

	Vector2D vector2;
	vector2.X = value1.X * single;
	vector2.Y = value1.Y * single;

	return vector2;
}

Vector2D Vector2D::Multiply(Vector2D value1, Vector2D value2)
{
	Vector2D vector2;
	vector2.X = value1.X * value2.X;
	vector2.Y = value1.Y * value2.Y;

	return vector2;
}

Vector2D Vector2D::Multiply(Vector2D value1, float scaleFactor)
{
	Vector2D vector2;
	vector2.X = value1.X * scaleFactor;
	vector2.Y = value1.Y * scaleFactor;

	return vector2;
}

Vector2D Vector2D::Subtract(Vector2D value1, Vector2D value2)
{
	Vector2D vector2;
	vector2.X = value1.X - value2.X;
	vector2.Y = value1.Y - value2.Y;

	return vector2;
}

Vector2D Vector2D::Negative(Vector2D value)
{
	Vector2D vector2;
	vector2.X = -value.X;
	vector2.Y = -value.Y;

	return vector2;
}


Vector2D Vector2D::Barycentric(Vector2D value1, Vector2D value2, Vector2D value3, float amount1, float amount2)
{
	Vector2D vector2;
	vector2.X = value1.X + amount1 * (value2.X - value1.X) + amount2 * (value3.X - value1.X);
	vector2.Y = value1.Y + amount1 * (value2.Y - value1.Y) + amount2 * (value3.Y - value1.Y);

	return vector2;
}


float Vector2D::Distance(Vector2D value1, Vector2D value2)
{
	float x = value1.X - value2.X;
	float y = value1.Y - value2.Y;
	float single = x * x + y * y;

	return sqrtf(single);
}


float Vector2D::DistanceSquared(Vector2D value1, Vector2D value2)
{
	float x = value1.X - value2.X;
	float y = value1.Y - value2.Y;

	return x * x + y * y;
}


float Vector2D::Dot(Vector2D value1, Vector2D value2)
{
	return value1.X * value2.X + value1.Y * value2.Y;
}


Vector2D Vector2D::Normalize(Vector2D value)
{
	float x = value.X * value.X + value.Y * value.Y;
	float single = 1.0f / sqrtf(x);

	Vector2D vector2;
	vector2.X = value.X * single;
	vector2.Y = value.Y * single;

	return vector2;
}


Vector2D Vector2D::Reflect(Vector2D vector, Vector2D normal)
{
	float x = vector.X * normal.X + vector.Y * normal.Y;

	Vector2D vector2;
	vector2.X = vector.X - 2.0f * x * normal.X;
	vector2.Y = vector.Y - 2.0f * x * normal.Y;

	return vector2;
}


Vector2D Vector2D::Min(Vector2D value1, Vector2D value2)
{
	Vector2D vector2;
	vector2.X = (value1.X < value2.X ? value1.X : value2.X);
	vector2.Y = (value1.Y < value2.Y ? value1.Y : value2.Y);

	return vector2;
}


Vector2D Vector2D::Max(Vector2D value1, Vector2D value2)
{
	Vector2D vector2;
	vector2.X = (value1.X > value2.X ? value1.X : value2.X);
	vector2.Y = (value1.Y > value2.Y ? value1.Y : value2.Y);

	return vector2;
}


Vector2D Vector2D::Clamp(Vector2D value1, Vector2D min, Vector2D max)
{
	float x = value1.X;
	x = (x > max.X ? max.X : x);
	x = (x < min.X ? min.X : x);

	float y = value1.Y;
	y = (y > max.Y ? max.Y : y);
	y = (y < min.Y ? min.Y : y);

	Vector2D vector2;
	vector2.X = x;
	vector2.Y = y;

	return vector2;
}


Vector2D Vector2D::Lerp(Vector2D value1, Vector2D value2, float amount)
{
	Vector2D vector2;
	vector2.X = value1.X + (value2.X - value1.X) * amount;
	vector2.Y = value1.Y + (value2.Y - value1.Y) * amount;

	return vector2;
}


Vector2D Vector2D::SmoothStep(Vector2D value1, Vector2D value2, float amount)
{
	float single;
	if (amount > 1.0f)
		single = 1.0f;
	else
		single = (amount < 0.0f ? 0.0f : amount);

	amount = single;
	amount = amount * amount * (3.0f - 2.0f * amount);

	Vector2D vector2;
	vector2.X = value1.X + (value2.X - value1.X) * amount;
	vector2.Y = value1.Y + (value2.Y - value1.Y) * amount;

	return vector2;
}


Vector2D Vector2D::CatmullRom(Vector2D value1, Vector2D value2, Vector2D value3, Vector2D value4, float amount)
{
	float single = amount * amount;
	float single1 = amount * single;

	Vector2D vector2;
	vector2.X = 0.5f * (2.0f * value2.X + (-value1.X + value3.X) * amount + (2.0f * value1.X - 5.0f * value2.X + 4.0f * value3.X - value4.X) * single + (-value1.X + 3.0f * value2.X - 3.0f * value3.X + value4.X) * single1);
	vector2.Y = 0.5f * (2.0f * value2.Y + (-value1.Y + value3.Y) * amount + (2.0f * value1.Y - 5.0f * value2.Y + 4.0f * value3.Y - value4.Y) * single + (-value1.Y + 3.0f * value2.Y - 3.0f * value3.Y + value4.Y) * single1);

	return vector2;
}


Vector2D Vector2D::Hermite(Vector2D value1, Vector2D tangent1, Vector2D value2, Vector2D tangent2, float amount)
{
	float single = amount * amount;
	float single1 = amount * single;
	float single2 = 2.0f * single1 - 3.0f * single + 1.0f;
	float single3 = -2.0f * single1 + 3.0f * single;
	float single4 = single1 - 2.0f * single + amount;
	float single5 = single1 - single;

	Vector2D vector2;
	vector2.X = value1.X * single2 + value2.X * single3 + tangent1.X * single4 + tangent2.X * single5;
	vector2.Y = value1.Y * single2 + value2.Y * single3 + tangent1.Y * single4 + tangent2.Y * single5;

	return vector2;
}


Vector2D Vector2D::Transform(Vector2D position, Matrix matrix)
{
	float x = position.X * matrix.M11 + position.Y * matrix.M21 + matrix.M41;
	float single = position.X * matrix.M12 + position.Y * matrix.M22 + matrix.M42;

	Vector2D vector2;
	vector2.X = x;
	vector2.Y = single;

	return vector2;
}


Vector2D Vector2D::Transform(Vector2D value, Quaternion rotation)
{
	float x = rotation.X + rotation.X;
	float y = rotation.Y + rotation.Y;
	float z = rotation.Z + rotation.Z;
	float w = rotation.W * z;

	float single = rotation.X * x;
	float x1 = rotation.X * y;
	float y1 = rotation.Y * y;
	float z1 = rotation.Z * z;

	float single1 = value.X * (1.0f - y1 - z1) + value.Y * (x1 - w);
	float x2 = value.X * (x1 + w) + value.Y * (1.0f - single - z1);

	Vector2D vector2;
	vector2.X = single1;
	vector2.Y = x2;

	return vector2;
}


Vector2D Vector2D::TransformNormal(Vector2D normal, Matrix matrix)
{
	float x = normal.X * matrix.M11 + normal.Y * matrix.M21;
	float single = normal.X * matrix.M12 + normal.Y * matrix.M22;

	Vector2D vector2;
	vector2.X = x;
	vector2.Y = single;

	return vector2;
}