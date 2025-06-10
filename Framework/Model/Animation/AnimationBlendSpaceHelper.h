#pragma once
#include <unordered_set>
#include <unordered_set>

#include "BasicShapes/BasicShapes2D/LineSegment2D.h"

class LineSegment2D;
class Triangle2D;

struct BlendSpace2DAnimationSample
{
	AnimationClip * Animation;
	Vector2D Position;
};

class DelaunayTriangulator2D
{
public:
	enum CircumcirclePosition
	{
		Outside = -1,
		On = 0,
		Inside = 1
	};
	explicit DelaunayTriangulator2D(const vector<Vector2D> & SamplePoints);
	~DelaunayTriangulator2D() = default;
	const vector<Triangle2D> & GetTriangles() const;
	bool GetTriangle(const Vector2D& Position, array<Vector2D, 3>& OutVertices,
	                 array<float, 3>& BarycentricWeights) const;
	void AddSample(const Vector2D & Position);
private:
	void Triangulate();
	static bool CheckCircumcircleCriteria(const vector<Triangle2D>& Triangles, const Vector2D& Point, int& OutTriangleIndex);
	static CircumcirclePosition GetCircumcirclePosition(const Triangle2D& Triangle, Vector2D Point);
	static void RemoveSuperTriangle(vector<Triangle2D>& Triangles, const Triangle2D& SuperTriangle);
	static Triangle2D MakeSuperTriangle(const vector<Vector2D>& SamplePoints);
	static void SearchBadTriangles(const vector<Triangle2D>& Triangles, const Vector2D& Point, vector<int>& OutBadTrianglesIndices);
	static void SearchPolygonHole(const vector<Triangle2D>& Triangles, const vector<int>& BadTrianglesIndices, unordered_set<LineSegment2D> & OutEdges);
	static void RemoveBadTriangles(vector<Triangle2D>& InOutTriangles, vector<int>& BadTrianglesIndices);
	static void ReTriangulate(vector<Triangle2D>& OutTriangles, const unordered_set<LineSegment2D>& Edges, const Vector2D& NewPoint);

	vector<Vector2D> SamplePoints;
	vector<Triangle2D> Triangles;
};

