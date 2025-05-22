#include "Framework.h"
#include "Plane.h"
#include "Quaternion.h"

//////////////////////////////////////////////////////////////////////////
///@brief 생성자
//////////////////////////////////////////////////////////////////////////
Plane::Plane()
{
	Normal = Vector::Zero;

	D = 0.0f;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 생성자
///@param a : 노멀축 X
///@param b : 노멀축 Y
///@param c : 노멀축 Z
///@param d : 부호 있는, 원점까지의 거리.
//////////////////////////////////////////////////////////////////////////
Plane::Plane(float a, float b, float c, float d)
{
	Normal.X = a;
	Normal.Y = b;
	Normal.Z = c;

	D = d;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 생성자
///@param normal : 노멀 벡터
///@param d : 부호 있는, 원점까지의 거리.
//////////////////////////////////////////////////////////////////////////
Plane::Plane( const Vector& normal, float d)
{
	Normal = normal;

	D = d;
}

Plane::Plane(const Vector& Normal, const Vector& Point)
{
	this->Normal = Vector::Normalize(Normal);
	this->D = -Vector::Dot(Point, Normal);
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 생성자
///@param value : 4D 벡터(평면의 방정식의 각 계수)
//////////////////////////////////////////////////////////////////////////
Plane::Plane( const Vector4& value)
{
	Normal.X = value.X;
	Normal.Y = value.Y;
	Normal.Z = value.Z;

	D = value.W;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 생성자
///@param point1 : 위치 벡터1
///@param point2 : 위치 벡터2
///@param point3 : 위치 벡터3
//////////////////////////////////////////////////////////////////////////
Plane::Plane( const Vector& point1, const Vector& point2, const Vector& point3)
{
	float x = point2.X - point1.X;
	float y = point2.Y - point1.Y;
	float z = point2.Z - point1.Z;
	float y1 = point3.Y - point1.Y;
	float z1 = point3.Z - point1.Z;

	float value = point3.X - point1.X;
	float value1 = y * z1 - z * y1;
	float value2 = z * value - x * z1;
	float value3 = x * y1 - y * value;
	float value4 = value1 * value1 + value2 * value2 + value3 * value3;
	float value5 = 1.0f / sqrtf(value4);

	Normal.X = value1 * value5;
	Normal.Y = value2 * value5;
	Normal.Z = value3 * value5;

	D = -(Normal.X * point1.X + Normal.Y * point1.Y + Normal.Z * point1.Z);
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief ==
///@param value : 평면
//////////////////////////////////////////////////////////////////////////
bool Plane::operator==(const Plane& value)const
{
	if (Normal.X == value.Normal.X && Normal.Y == value.Normal.Y && Normal.Z == value.Normal.Z)
		return D == value.D;
	else
		return false;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief !=
///@param value : 평면
//////////////////////////////////////////////////////////////////////////
bool Plane::operator!=(const Plane& value)const
{
	if (Normal.X == value.Normal.X && Normal.Y == value.Normal.Y && Normal.Z == value.Normal.Z)
		return D != value.D;
	else
		return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 문자열로 변환
///@param 문자열
//////////////////////////////////////////////////////////////////////////
std::wstring Plane::ToWString()
{
	std::wstring temp = L"";

	temp += L"Normal:" + Normal.ToWString();
	temp += L",D:" + std::to_wstring(D);

	return temp;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 
///@param 
//////////////////////////////////////////////////////////////////////////
// D3DXPLANE Plane::ToD3DXPLANE()
// {
// 	return D3DXPLANE(Normal.X, Normal.Y, Normal.Z, D);
// }
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 정규화
//////////////////////////////////////////////////////////////////////////
void Plane::Normalize()
{
	float x = Normal.X * Normal.X + Normal.Y * Normal.Y + Normal.Z * Normal.Z;

	if (fabsf(x - 1.0f) < 1.1920929E-07f)
		return;

	float single = 1.0f / sqrtf(x);
	Normal.X *= single;
	Normal.Y *= single;
	Normal.Z *= single;

	D *= single;
}

void Plane::Transform(const Matrix& InMatrix)
{
	Vector Origin = Normal * D;
	
	Normal = Normal * InMatrix;
	
}

void Plane::GetEquation(float& A, float& B, float& C, float& D) const
{
	A = this->Normal.X;
	B = this->Normal.Y;
	C = this->Normal.Z;
	D = this->D;
}

Vector Plane::GetNormal() const
{
	return Normal;
}

Vector Plane::GetOrigin() const
{
	return Normal * D;
}

/// @param Point 임의의 점 P(x,y,z). 
/// @return ax + by + cz + d
float Plane::GetDistance(const Vector& Point) const
{
	return Vector::Dot(Normal, Point) + D;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 정규화
///@param value : 평면
///@return 결과 평면
//////////////////////////////////////////////////////////////////////////
Plane Plane::Normalize( const Plane& value)
{
	Plane normal;
	float x = value.Normal.X * value.Normal.X + value.Normal.Y * value.Normal.Y + value.Normal.Z * value.Normal.Z;

	if (fabsf(x - 1.0f) < 1.1920929E-07f)
	{
		normal.Normal = value.Normal;
		normal.D = value.D;

		return normal;
	}

	float value2 = 1.0f / sqrtf(x);
	normal.Normal.X = value.Normal.X * value2;
	normal.Normal.Y = value.Normal.Y * value2;
	normal.Normal.Z = value.Normal.Z * value2;
	normal.D = value.D * value2;

	return normal;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 매트릭스에 의한 평면 변환
///@param plane : 평면
///@param matrix : 매트릭스
///@return 결과 평면
//////////////////////////////////////////////////////////////////////////
Plane Plane::Transform( const Plane& plane, const Matrix& matrix)
{
	Matrix matrix1 = Matrix::Invert(matrix);

	float x = plane.Normal.X;
	float y = plane.Normal.Y;
	float z = plane.Normal.Z;
	float d = plane.D;

	Plane p;
	p.Normal.X = x * matrix1.M11 + y * matrix1.M12 + z * matrix1.M13 + d * matrix1.M14;
	p.Normal.Y = x * matrix1.M21 + y * matrix1.M22 + z * matrix1.M23 + d * matrix1.M24;
	p.Normal.Z = x * matrix1.M31 + y * matrix1.M32 + z * matrix1.M33 + d * matrix1.M34;
	p.D = x * matrix1.M41 + y * matrix1.M42 + z * matrix1.M43 + d * matrix1.M44;

	return p;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 쿼터니온에 의한 평면 변환
///@param plane : 평면
///@param rotation : 쿼터니온
///@return 결과 평면
//////////////////////////////////////////////////////////////////////////
Plane Plane::Transform( const Plane& plane, const Quaternion& rotation)
{
	float x = rotation.X + rotation.X;
	float y = rotation.Y + rotation.Y;
	float z = rotation.Z + rotation.Z;
	float w = rotation.W * x;
	float value = rotation.W * y;

	float w1 = rotation.W * z;
	float x1 = rotation.X * x;
	float value1 = rotation.X * y;

	float x2 = rotation.X * z;
	float y1 = rotation.Y * y;
	float y2 = rotation.Y * z;
	float z1 = rotation.Z * z;

	float value2 = 1.0f - y1 - z1;
	float value3 = value1 - w1;
	float value4 = x2 + value;
	float value5 = value1 + w1;
	float value6 = 1.0f - x1 - z1;

	float value7 = y2 - w;
	float value8 = x2 - value;
	float value9 = y2 + w;
	float value10 = 1.0f - x1 - y1;

	float x3 = plane.Normal.X;
	float y3 = plane.Normal.Y;
	float z2 = plane.Normal.Z;

	Plane p;
	p.Normal.X = x3 * value2 + y3 * value3 + z2 * value4;
	p.Normal.Y = x3 * value5 + y3 * value6 + z2 * value7;
	p.Normal.Z = x3 * value8 + y3 * value9 + z2 * value10;
	p.D = plane.D;

	return p;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 평면과 4D 벡터의 내적
///@param value : 벡터
///@return 결과 값
//////////////////////////////////////////////////////////////////////////
float Plane::Dot( const Vector4& value) const
{
	return Normal.X * value.X + Normal.Y * value.Y + Normal.Z * value.Z + D * value.W;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 평면과 3D 위치 벡터의 내적(W=1.0f)
///@param value : 벡터
///@return 결과 값
//////////////////////////////////////////////////////////////////////////
float Plane::DotCoordinate( const Vector& value) const
{
	return Normal.X * value.X + Normal.Y * value.Y + Normal.Z * value.Z + D;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 평면과 3D 방향 벡터의 내적(W=0.0f)
///@param value : 벡터
///@return 결과 값
//////////////////////////////////////////////////////////////////////////
float Plane::DotNormal( const Vector& value) const
{
	return Normal.X * value.X + Normal.Y * value.Y + Normal.Z * value.Z;
}
//////////////////////////////////////////////////////////////////////////