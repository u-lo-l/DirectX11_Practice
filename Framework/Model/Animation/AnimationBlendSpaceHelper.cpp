#include "framework.h"
#include "AnimationBlendSpaceHelper.h"
#include <unordered_set>

/**
 * @brief Bowyer–Watson incremental algorithm
 * 
 * @link https://en.wikipedia.org/wiki/Bowyer%E2%80%93Watson_algorithm @endlink 
 */
DelaunayTriangulator2D::DelaunayTriangulator2D(const vector<Vector2D>& SamplePoints)
	: SamplePoints(SamplePoints)
{
	Triangulate();
}

const vector<Triangle2D>& DelaunayTriangulator2D::GetTriangles() const
{
	return Triangles;
}

bool DelaunayTriangulator2D::GetTriangle
(
	const Vector2D& Position,
	array<Vector2D, 3>& OutVertices,
	array<float, 3>& BarycentricWeights
) const
{
	OutVertices.fill({0.f, 0.f});
	BarycentricWeights.fill(0.f);
	
	const int TriangleCount = Triangles.size();
	if (TriangleCount == 0)
		return false;
	if (TriangleCount == 1)
	{
		const array<Vector2D, 3> & V = Triangles[0].GetVertices();
		if (V[0] == V[1])
		{
			BarycentricWeights = {1, 0, 0};
			OutVertices = {V[0], V[0], V[0]};
			return true;
		}
		if (V[1] == V[2])
		{
			Vector2D Edge = V[1] - V[0];
			Edge.Normalize();
			const float Projection = Vector2D::Dot(Edge, Position - V[0]);
			float V0Weight = Projection / Edge.Length();
			V0Weight = Math::Clamp(V0Weight, 0.f, 1.f);
			BarycentricWeights = {V0Weight, 1.f - V0Weight, 0};
			OutVertices = {V[0], V[1], V[1]};
			return true;
		}
	}
	
	for (const Triangle2D & Triangle : Triangles)
	{
		if (Triangle.Contains(Position) == true)
		{
			Triangle.GetBarycentric(Position, BarycentricWeights);
			OutVertices = Triangle.GetVertices();
			return true;
		}
	}
	return false;
}

void DelaunayTriangulator2D::AddSample(const Vector2D& Position)
{
	SamplePoints.push_back(Position);
	Triangulate();
}

void DelaunayTriangulator2D::Triangulate()
{
	Triangles.clear();
	const int SamplesSize = SamplePoints.size();

	if (SamplesSize == 0)
		return;
	if (SamplesSize <= 2)
	{
		Triangles.emplace_back(SamplePoints[0], SamplePoints.back(), SamplePoints.back());
		return ;
	}
	const Triangle2D SuperTriangle = MakeSuperTriangle(this->SamplePoints);
	Triangles.push_back(SuperTriangle);

	for (const Vector2D & Point : SamplePoints)
	{
		vector<int> BadTrianglesIndices;
		unordered_set<LineSegment2D> Edges;
		SearchBadTriangles(Triangles, Point, BadTrianglesIndices);
		SearchPolygonHole(Triangles, BadTrianglesIndices, Edges);
		RemoveBadTriangles(Triangles, BadTrianglesIndices);
		ReTriangulate(Triangles, Edges, Point);
	}
	RemoveSuperTriangle(this->Triangles, SuperTriangle);
}

bool DelaunayTriangulator2D::CheckCircumcircleCriteria
(
	const vector<Triangle2D>& Triangles,
	const Vector2D & Point,
	int & OutTriangleIndex
)
{
	OutTriangleIndex = -1;
	const int TriangleCount = Triangles.size();
	for (int i = 0 ; i < TriangleCount; i++)
	{
		if (GetCircumcirclePosition(Triangles[i], Point) == Inside)
		{
			OutTriangleIndex = i;
			return false;
		}
	}
	return true;
}

DelaunayTriangulator2D::CircumcirclePosition DelaunayTriangulator2D::GetCircumcirclePosition(
	const Triangle2D& Triangle, const Vector2D Point)
{
	const Circle2D & Circumcircle = Triangle2D::GetCircumcircle(Triangle);
	const float DistanceSqr = Vector2D::DistanceSquared(Circumcircle.GetCenter(), Point);
	const float RadiusSqr = powf(Circumcircle.GetRadius(), 2.f);

	if (Math::NearEqual(DistanceSqr, RadiusSqr))
		return On;
	if (DistanceSqr > RadiusSqr)
		return Outside;
	return Inside;
}

Triangle2D DelaunayTriangulator2D::MakeSuperTriangle(const vector<Vector2D> & SamplePoints)
{
	constexpr float Margin = 1.f;
	Vector2D AABBMin { FLT_MAX, FLT_MAX };
	Vector2D AABBMax { -FLT_MAX, -FLT_MAX};
	for (const Vector2D & Point : SamplePoints)
	{
		AABBMin.X = Math::Min(AABBMin.X, Point.X);
		AABBMin.Y = Math::Min(AABBMin.Y, Point.Y);
		AABBMax.X = Math::Max(AABBMax.X, Point.X);
		AABBMax.Y = Math::Max(AABBMax.Y, Point.Y);
	}
	AABBMin += {-Margin, -Margin};
	AABBMax += {+Margin, +Margin};

	const Vector2D Center = (AABBMin + AABBMax) * 0.5f;
	const float SuperTriangleInscribedRadius = Vector2D::Distance(Center, AABBMax);
	return {
		Center + Vector2D(-sqrt(3.f), -1.f) * SuperTriangleInscribedRadius,
		Center + Vector2D(0.f, 2.f) * SuperTriangleInscribedRadius,
		Center + Vector2D(sqrt(3.f), -1.f) * SuperTriangleInscribedRadius,
	};
}

void DelaunayTriangulator2D::SearchBadTriangles
(
	const vector<Triangle2D>& Triangles,
	const Vector2D & Point,
	vector<int>& OutBadTrianglesIndices
)
{
	OutBadTrianglesIndices.clear();
	
	const int TriangleCount = Triangles.size();
	for (int i = 0 ; i < TriangleCount; i++)
	{
		if (GetCircumcirclePosition(Triangles[i], Point) == Inside)
			OutBadTrianglesIndices.push_back(i);
	}
}

void DelaunayTriangulator2D::SearchPolygonHole(
	const vector<Triangle2D>& Triangles,
	const vector<int> & BadTrianglesIndices,
	unordered_set<LineSegment2D> & OutEdges
)
{
	OutEdges.clear();
	hash<LineSegment2D> Hasher;

	for (int i : BadTrianglesIndices)
	{
		CHECK(i >= 0 && i < (int)Triangles.size());
		const array<Vector2D, 3> & V = Triangles[i].GetVertices();
		array<LineSegment2D, 3> Edges = {
			LineSegment2D(V[0], V[1]),
			LineSegment2D(V[1], V[2]),
			LineSegment2D(V[2], V[0])
		};
		for (const LineSegment2D & Edge : Edges)
		{
			std::uint64_t H = Hasher(Edge);
			const auto & It = OutEdges.find(Edge);
			if (It == OutEdges.cend())
				OutEdges.insert(Edge);
			else
			{
				bool IsSame = (*It == Edge);
				OutEdges.erase(It);
			}
		}
	}
}

void DelaunayTriangulator2D::RemoveBadTriangles
(
	vector<Triangle2D>& InOutTriangles,
	vector<int> & BadTrianglesIndices
)
{
	std::sort(BadTrianglesIndices.rbegin(), BadTrianglesIndices.rend());
	for (const int IndexToRemove : BadTrianglesIndices) {
		InOutTriangles.erase(InOutTriangles.begin() + IndexToRemove);
	}
}

void DelaunayTriangulator2D::ReTriangulate
(
	vector<Triangle2D>& OutTriangles,
	const unordered_set<LineSegment2D> & Edges,
	const Vector2D & NewPoint
)
{
	for (const LineSegment2D & Edge : Edges)
		OutTriangles.push_back({Edge.GetEndPoint(0), Edge.GetEndPoint(1), NewPoint});
}

void DelaunayTriangulator2D::RemoveSuperTriangle
(
	vector<Triangle2D>& Triangles,
	const Triangle2D& SuperTriangle
)
{
	const array<Vector2D, 3> & SuperTriangleVertices = SuperTriangle.GetVertices();
	auto RemovalLambda = [&SuperTriangleVertices](const Triangle2D & Triangle)->bool {
		const array<Vector2D, 3> & Vertices = Triangle.GetVertices();
		for (int i = 0 ; i < 3 ; i++)
		{
			if (std::find(Vertices.begin(), Vertices.end(), SuperTriangleVertices[i]) == Vertices.end())
				continue;
			return true;
		}
		return false;
	};

	const auto RemoveIt = std::remove_if(Triangles.begin(), Triangles.end(), RemovalLambda);
	Triangles.erase(RemoveIt, Triangles.end());
}
