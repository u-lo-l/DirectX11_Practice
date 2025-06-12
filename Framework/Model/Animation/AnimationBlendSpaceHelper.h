#pragma once
#include <unordered_set>
#include <unordered_set>

#include "BasicShapes/BasicShapes2D/LineSegment2D.h"

class LineSegment2D;
class Triangle2D;

class DelaunayTriangulator2D
{
private:
	struct BlendSpace2DAnimationSample
	{
		const AnimationClip * Animation;
		Vector2D Position;
	};

	struct BlendSpace2DTriangleNode
	{
		array<int , 3> Indices = { -1, };
		array<const BlendSpace2DTriangleNode *, 3> Neighbors = { nullptr, };
	};
	
public:
	enum CircumcirclePosition
	{
		Outside = -1,
		On = 0,
		Inside = 1
	};
	DelaunayTriangulator2D();
	~DelaunayTriangulator2D() = default;

	void AddSample(const AnimationClip * Clip, const Vector2D & NormalizedPosition);
	void Triangulate();
	const vector<array<int, 3>> & GetTriangleVertexIndices() const;
	bool GetAnimsAndWeights(
		const Vector2D& NormalizedPosition,
		array<const AnimationClip*, 3> & OutClips,
		array<float, 3> & OutWeights
	) const;
	bool GetVerticesAndWeights(
		const Vector2D& Position,
		array<Vector2D, 3> & OutVertices,
		array<float, 3> & OutWeights
	) const;

private:
	bool VerifySamples() const;
	bool CheckCollinear() const;
	// Segmentate
	void GetSuperSegment();
	
	// Triangulate
	void MakeSuperTriangle();
	void RemoveSuperTriangle();
	void SearchBadTriangles(const Vector2D& SamplePosition, vector<int>& OutBadTriangleIndices) const;
	void SearchPolygonHole(const vector<int>& InBadTriangleIndices, unordered_set<pair<int, int>>& OutEdgeEndPointIndices) const;
	void RemoveBadTriangles(vector<int>& InBadTriangleIndices);
	void Retriangulate(const unordered_set<pair<int, int>>& InEdgeEndPointIndices, int NewSampleIndex);
	static CircumcirclePosition GetCircumcirclePosition(const Triangle2D& Triangle, const Vector2D & Point);

	bool GetAnimsAndWeights_Collinear(
		const Vector2D& Position,
		array<const AnimationClip*, 3>& OutClips,
		array<float, 3>& OutWeights
	) const;
	bool GetAnimsAndWeights_Triangular(
		const Vector2D& Position,
		array<const AnimationClip*, 3>& OutClips,
		array<float, 3>& OutWeights
	) const;
	bool GetVerticesAndWeights_Collinear(
		const Vector2D& NormalizedPosition,
		array<Vector2D, 3> & OutVertices,
		array<float, 3>& OutWeights
	) const;
	bool GetVerticesAndWeights_Triangular(
		const Vector2D& NormalizedPosition,
		array<Vector2D, 3> & OutVertices,
		array<float, 3>& OutWeights
	) const;

	
	bool bCollinear;

	// Triangulate
	vector<BlendSpace2DAnimationSample> Samples;
	vector<array<int, 3>> TriangleVertexIndices;

	BlendSpace2DTriangleNode * Triangles;

	// Collinear
	map<float, int> SegmentVertexIndices;
	array<Vector2D, 2> SuperSegment = {Vector2D(0, 0), Vector2D(0, 0)};
	Vector2D SpanDir {NAN, NAN};
};

