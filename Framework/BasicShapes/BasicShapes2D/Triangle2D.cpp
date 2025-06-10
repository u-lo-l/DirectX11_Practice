#include "framework.h"
#include "Triangle2D.h"

#include "Circle2D.h"

using namespace std;

Triangle2D::Triangle2D(const array<Vector2D, 3>& Vertices)
	: Triangle2D(Vertices[0], Vertices[1], Vertices[2])
{
}

Triangle2D::Triangle2D(const Vector2D& A, const Vector2D& B, const Vector2D& C)
{
	const Vector2D E0 = B - A;
	const Vector2D E1 = C - A;

	if ((E0 ^ E1) < 0.f)
		Vertices = {A, B, C};
	else
		Vertices = {A, C, B};
}

Circle2D Triangle2D::GetCircumcircle(const Triangle2D& Triangle)
{
	auto GetPerpSlope = [](const Vector2D& A, const Vector2D& B)->float	{
		return Math::NearEqual(A.Y, B.Y) ? numeric_limits<float>::infinity() : -(B.X - A.X) / (B.Y - A.Y);
	};
	auto SolveFirstOrderEquation = [](const array<float, 2> & Coefs, float X)->float {
		return Coefs[0] * X + Coefs[1];
	};

	const array<Vector2D, 3> & Vertices = Triangle.GetVertices();
	const Vector2D ABMid = (Vertices[0] + Vertices[1]) * 0.5f;
	const Vector2D ACMid = (Vertices[0] + Vertices[2]) * 0.5f;

	const float ABPerpSlope = GetPerpSlope(Vertices[0], Vertices[1]);
	const float ACPerpSlope = GetPerpSlope(Vertices[0], Vertices[2]);

	const array<float, 2> ABEquationCoefs = {ABPerpSlope, ABMid.Y - ABMid.X * ABPerpSlope};
	const array<float, 2> ACEquationCoefs = {ACPerpSlope, ACMid.Y - ACMid.X * ACPerpSlope};

	Vector2D Circumcenter;
	if (std::isinf(ABPerpSlope))
	{
		Circumcenter.X = ABMid.X;
		Circumcenter.Y = SolveFirstOrderEquation(ACEquationCoefs, Circumcenter.X);
	}
	else if(std::isinf(ACPerpSlope))
	{
		Circumcenter.X = ACMid.X;
		Circumcenter.Y = SolveFirstOrderEquation(ABEquationCoefs, Circumcenter.X);
	}
	else
	{
		const float Numerator = -(ABEquationCoefs[1] - ACEquationCoefs[1]);
		const float Denominator = ABEquationCoefs[0] - ACEquationCoefs[0];
		Circumcenter.X =  Numerator/ Denominator;
		Circumcenter.Y = SolveFirstOrderEquation(ABEquationCoefs, Circumcenter.X);
	}

	float Radius = Vector2D::Distance(Circumcenter, Vertices[0]);
	return { Circumcenter, Radius };
}

const array<Vector2D, 3>& Triangle2D::GetVertices() const
{
	return Vertices;
}

const Vector2D& Triangle2D::GetVertex(int Index) const
{
	return Vertices[Index];
}

void Triangle2D::GetBarycentric(const Vector2D& Point, array<float, 3> & OutBarycentric) const
{
	const Vector2D Edge1 = Vertices[1] - Vertices[0];
	const Vector2D Edge2 = Vertices[2] - Vertices[0];
	const Vector2D Target = Point - Vertices[0];

	const float Denominator = Edge1.X * Edge2.Y - Edge2.X * Edge1.Y;
	if (Math::IsZero(Denominator) == true)
	{
		OutBarycentric = {1, 0, 0};
	}
	else
	{
		const float InvDenominator = 1.0f / Denominator;
		OutBarycentric[0] = (Target.X * Edge2.Y - Edge2.X * Target.Y) * InvDenominator;
		OutBarycentric[1] = (Edge1.X * Target.Y - Target.X * Edge1.Y) * InvDenominator;
		OutBarycentric[2] = 1.0f - OutBarycentric[0] - OutBarycentric[1];
	}
}

/**
 * @link
 * https://www.notion.so/u-lo-l/Cpp-2-f7a7e1d88f144d50a0e5cb558ca8bb97?source=copy_link#a055645299894fdfb57a3ccbdf56583b
 * @endlink
*/
bool Triangle2D::Contains(const Vector2D & Point, bool IncludeEdge) const
{
	const Vector2D EdgeA = Vertices[1] - Vertices[0];
	const Vector2D EdgeB = Vertices[2] - Vertices[0];
	const Vector2D P     = Point - Vertices[0];

	const float Denominator = abs(EdgeA ^ EdgeB);
	ASSERT(Denominator > Math::EPSILON, String::Format("%s | Triangle Not Valid", __FUNCTION__).c_str());
	
	const float DenominatorInv = 1.0f / Denominator;
	const float U = +(EdgeB.Y * P.X - EdgeB.X * P.Y) * DenominatorInv;
	const float V = -(EdgeA.Y * P.X - EdgeA.X * P.Y) * DenominatorInv ;
	bool Result = true;
	if (IncludeEdge == true)
	{
		Result &= U >= -Math::EPSILON;
		Result &= U <= 1 + Math::EPSILON;
		Result &= V >= -Math::EPSILON;
		Result &= V <= 1 + Math::EPSILON;
		Result &= U + V <= 1 + Math::EPSILON;
	}
	else
	{
		Result &= U > -Math::EPSILON;
		Result &= U < 1 + Math::EPSILON;
		Result &= V > -Math::EPSILON;
		Result &= V < 1 + Math::EPSILON;
		Result &= U + V < 1 + Math::EPSILON;
	}
	return Result;
}

bool Triangle2D::IsValid(const array<Vector2D, 3>& Vertices)
{
	if (Vector2D::NearEqual(Vertices[0], Vertices[1]))
		return false;
	if (Vector2D::NearEqual(Vertices[0], Vertices[2]))
		return false;
	if (Vector2D::NearEqual(Vertices[1], Vertices[2]))
		return false;
	return true;
}

bool Triangle2D::IsValid(const Vector2D& A, const Vector2D& B, const Vector2D& C)
{
	if (Vector2D::NearEqual(A, B))
		return true;
	if (Vector2D::NearEqual(A, C))
		return true;
	if (Vector2D::NearEqual(B, C))
		return true;
	return false;
}
