#include "framework.h"
#include "AnimationBlendSpaceHelper.h"
#include <unordered_set>



DelaunayTriangulator2D::DelaunayTriangulator2D()
= default;

void DelaunayTriangulator2D::AddSample(const AnimationClip* Clip, const Vector2D& NormalizedPosition)
{
	ASSERT(!!Clip, "Clip not valid");
	Samples.push_back({Clip, NormalizedPosition});
}

/**
 * @brief Bowyer–Watson incremental algorithm
 * 
 * @link https://en.wikipedia.org/wiki/Bowyer%E2%80%93Watson_algorithm @endlink 
 */
void DelaunayTriangulator2D::Triangulate()
{
	if (VerifySamples() == false)
	{
		ASSERT(false, "DelaunayTriangulator2D : Samples Not Valid");
		return ;
	}
	
	TriangleVertexIndices.clear();
	SegmentVertexIndices.clear();
	const int SamplesSize = Samples.size();
	bCollinear = CheckCollinear();
	if (bCollinear == true)
	{
		GetSuperSegment();
		for (int i = 0 ; i < SamplesSize ; i++)
		{
			const float ProjectedDistance = Samples[i].Position - SuperSegment[0] | SpanDir; 
			SegmentVertexIndices[ProjectedDistance] = i;
		}
	}
	else
	{
		MakeSuperTriangle();
		for (int i = 0 ; i < SamplesSize ; i++)
		{
			vector<int> BadTrianglesIndices;
			unordered_set<pair<int, int>> EdgeEndPointIndices;
			SearchBadTriangles(Samples[i].Position, BadTrianglesIndices);
			SearchPolygonHole(BadTrianglesIndices, EdgeEndPointIndices);
			RemoveBadTriangles(BadTrianglesIndices);
			Retriangulate(EdgeEndPointIndices, i);
		}
		RemoveSuperTriangle();
	}
}

const vector<array<int, 3>>& DelaunayTriangulator2D::GetTriangleVertexIndices() const
{
	return TriangleVertexIndices;
}

bool DelaunayTriangulator2D::GetAnimsAndWeights 
(
	const Vector2D & NormalizedPosition,
	array<const AnimationClip*, 3> & OutClips,
	array<float, 3> & OutWeights
) const
{
	OutClips.fill(nullptr);
	OutWeights.fill(0.f);

	if (bCollinear)
	{
		if (SegmentVertexIndices.empty())
			return false;
		return GetAnimsAndWeights_Collinear(NormalizedPosition, OutClips, OutWeights);
	}
	else
	{
		if (TriangleVertexIndices.empty())
			return false;
		return GetAnimsAndWeights_Triangular(NormalizedPosition, OutClips, OutWeights);
	}
}

bool DelaunayTriangulator2D::GetVerticesAndWeights
(
	const Vector2D& Position,
	array<Vector2D, 3> & OutVertices,
	array<float, 3> & OutWeights
) const
{
	OutVertices.fill({NAN, NAN});
	OutWeights.fill(0.f);

	const int TriangleCount = TriangleVertexIndices.size();
	if (TriangleCount == 0)
		return false;
	return bCollinear ? GetVerticesAndWeights_Collinear(Position, OutVertices, OutWeights) :
						GetVerticesAndWeights_Triangular(Position, OutVertices, OutWeights);
}

bool DelaunayTriangulator2D::VerifySamples() const
{
	if (Samples.empty())
		return false;
	unordered_set<Vector2D> Positions;
	for (const BlendSpace2DAnimationSample & Sample : Samples)
	{
		if (Sample.Animation == nullptr)
			return false;
		if (Sample.Position.LInfNorm() > 1)
			return false;
		if (Sample.Position.X < 0 || Sample.Position.Y < 0)
			return false;
		if (Positions.find(Sample.Position) != Positions.end())
			return false;
		Positions.insert(Sample.Position);
	}
	return true;
}

bool DelaunayTriangulator2D::CheckCollinear() const
{
	if (Samples.size() <= 2)
		return true;
	const Vector2D Span = (Samples[1].Position - Samples[0].Position).Normalized();
	for (size_t i = 2; i < Samples.size(); i++)
	{
		const Vector2D Direction = Samples[i].Position - Samples[i - 1].Position;
		const float Cross = Direction ^ Span;
		if (Math::IsZero(Cross) == false)
			return false;
	}
	return true;
}

void DelaunayTriangulator2D::GetSuperSegment()
{
	const int SamplesSize = Samples.size();
	if (SamplesSize < 2)
		return ;
	const Vector2D & Point1 = Samples[0].Position;
	const Vector2D & Point2 = Samples[1].Position;
	this->SpanDir = (Point2 - Point1).Normalized();

	float Min = FLT_MAX;
	float Max = -FLT_MAX;
	int MinIndex = -1;
	int MaxIndex = -1;
	for (int i = 0 ; i < SamplesSize; i++)
	{
		float Projection = (Samples[i].Position - Point1) | (SpanDir);
		if (Projection < Min)
		{
			Min = Projection;
			MinIndex = i;
		}
		if (Projection > Max)
		{
			Max = Projection;
			MaxIndex = i;
		}
	}
	SuperSegment = { Samples[MinIndex].Position, Samples[MaxIndex].Position };
}

void DelaunayTriangulator2D::MakeSuperTriangle()
{
	const Vector2D AABBMin { -0.1f, -0.1f };
	const Vector2D AABBMax { 1.1f, 1.1f};
	const Vector2D Center = (AABBMin + AABBMax) * 0.5f;
	
	const float SuperTriangleInscribedRadius = Vector2D::Distance(Center, AABBMax);
	Triangle2D SuperTriangle = Triangle2D(
		Center + Vector2D(-sqrt(3.f), -1.f) * SuperTriangleInscribedRadius,
		Center + Vector2D(0.f, 2.f) * SuperTriangleInscribedRadius,
		Center + Vector2D(sqrt(3.f), -1.f) * SuperTriangleInscribedRadius
	);
	
	BlendSpace2DAnimationSample NullSamples[] = {
		{nullptr,SuperTriangle.GetVertex(0) },
		{nullptr,SuperTriangle.GetVertex(1) },
		{nullptr,SuperTriangle.GetVertex(2) }
	};
	int SuperTriangleStartIndex = this->Samples.size();
	array<int, 3> SuperTriangleIndices {
		SuperTriangleStartIndex,
		SuperTriangleStartIndex + 1,
		SuperTriangleStartIndex + 2
	};
	this->Samples.insert(this->Samples.end(), NullSamples, NullSamples + 3);
	this->TriangleVertexIndices.push_back(SuperTriangleIndices);
}

void DelaunayTriangulator2D::RemoveSuperTriangle()
{
	auto TriangleRemovalLambda = [&](const array<int, 3> & Indices) -> bool {
		for (int i : Indices)
		{
			if (this->Samples[i].Animation == nullptr)
				return true;
		}
		return false;
	};
	const auto TriangleRemovalIt = std::remove_if(
		this->TriangleVertexIndices.begin(),
		this->TriangleVertexIndices.end(),
		TriangleRemovalLambda
	);
	TriangleVertexIndices.erase(TriangleRemovalIt, this->TriangleVertexIndices.end());
	
	auto SampleRemovalLambda = [](const BlendSpace2DAnimationSample & Sample) -> bool {
		return Sample.Animation == nullptr;
	};
	const auto SampleRemovalIt = std::remove_if(
		this->Samples.begin(),
		this->Samples.end(),
		SampleRemovalLambda
	);
	Samples.erase(SampleRemovalIt, this->Samples.end());
}

void DelaunayTriangulator2D::SearchBadTriangles
(
	const Vector2D & SamplePosition,
	vector<int> & OutBadTriangleIndices
) const
{
	OutBadTriangleIndices.clear();
	const int TriangleCount = this->TriangleVertexIndices.size();
	for (int i = 0 ; i < TriangleCount; i++)
	{
		const array<int, 3> & TriangleIndices = this->TriangleVertexIndices[i];
		const Triangle2D Triangle = {
			this->Samples[TriangleIndices[0]].Position,
			this->Samples[TriangleIndices[1]].Position,
			this->Samples[TriangleIndices[2]].Position
		};
		if (GetCircumcirclePosition(Triangle, SamplePosition) == Inside)
			OutBadTriangleIndices.push_back(i);
	}
}

void DelaunayTriangulator2D::SearchPolygonHole
(
	const vector<int>& InBadTriangleIndices,
	unordered_set<pair<int, int>>& OutEdgeEndPointIndices
) const
{
	OutEdgeEndPointIndices.clear();

	for (int i : InBadTriangleIndices)
	{
		CHECK(i >= 0 && i < (int)this->TriangleVertexIndices.size());
		array<int, 3> Indices = this->TriangleVertexIndices[i];

		array<pair<int, int>, 3> EdgesIndices;
		EdgesIndices[0] = (Indices[0] < Indices[1]) ? make_pair(Indices[0], Indices[1]) : make_pair(Indices[1], Indices[0]);
		EdgesIndices[1] = (Indices[1] < Indices[2]) ? make_pair(Indices[1], Indices[2]) : make_pair(Indices[2], Indices[1]);
		EdgesIndices[2] = (Indices[2] < Indices[0]) ? make_pair(Indices[2], Indices[0]) : make_pair(Indices[0], Indices[2]);
		for (const pair<int, int> & EdgeIndex : EdgesIndices)
		{
			const auto & It = OutEdgeEndPointIndices.find(EdgeIndex);
			if (It == OutEdgeEndPointIndices.cend())
				OutEdgeEndPointIndices.insert(EdgeIndex);
			else
				OutEdgeEndPointIndices.erase(It);
		}
	}
}

void DelaunayTriangulator2D::RemoveBadTriangles
(
	vector<int> & InBadTriangleIndices
)
{
	std::sort(InBadTriangleIndices.rbegin(), InBadTriangleIndices.rend());
	for (const int IndexToRemove : InBadTriangleIndices) {
		this->TriangleVertexIndices.erase(this->TriangleVertexIndices.begin() + IndexToRemove);
	}
}

void DelaunayTriangulator2D::Retriangulate
(
	const unordered_set<pair<int, int>>& InEdgeEndPointIndices,
	int NewSampleIndex
)
{
	for (const pair<int, int> & Edge : InEdgeEndPointIndices)
	{
		const int EndPoint1Index = Edge.first;
		const int EndPoint2Index = Edge.second;
		Triangle2D NewTriangle = {
			Samples[EndPoint1Index].Position,
			Samples[EndPoint2Index].Position,
			Samples[NewSampleIndex].Position
		};
		if (NewTriangle.GetVertex(1) == Samples[EndPoint2Index].Position)
			this->TriangleVertexIndices.push_back({EndPoint1Index, EndPoint2Index, NewSampleIndex});
		else
			this->TriangleVertexIndices.push_back({EndPoint1Index, NewSampleIndex, EndPoint2Index});
	}
}

DelaunayTriangulator2D::CircumcirclePosition DelaunayTriangulator2D::GetCircumcirclePosition
(
	const Triangle2D & Triangle,
	const Vector2D & Point
)
{
	const Circle2D & Circumcircle = Triangle2D::GetCircumcircle(Triangle);
	const float DistanceSqr = Vector2D::DistanceSquared(Circumcircle.GetCenter(), Point);
	const float RadiusSqr = Circumcircle.GetRadius() * Circumcircle.GetRadius();

	if (Math::NearEqual(DistanceSqr, RadiusSqr))
		return On;
	if (DistanceSqr > RadiusSqr)
		return Outside;
	return Inside;
}

bool DelaunayTriangulator2D::GetAnimsAndWeights_Collinear
(
	const Vector2D & NormalizedPosition,
	array<const AnimationClip*, 3>& OutClips,
	array<float, 3>& OutWeights
) const
{
	const float Distance = NormalizedPosition | SpanDir;
	const auto It2 = SegmentVertexIndices.lower_bound(Distance);
	const auto It1 = (It2 == SegmentVertexIndices.cbegin() || Math::NearEqual(It2->first, Distance)) ? It2 : std::prev(It2);
	const float Value1 = It1->first;
	const float Value2 = It2->first;
	OutClips[0] = Samples[It1->second].Animation;
	OutClips[1] = Samples[It2->second].Animation;
	OutClips[2] = Samples[It2->second].Animation;
	const float Range = Value2 - Value1;
	OutWeights[0] = Math::IsZero(Range) ? 1.f : (Distance - Value1) / Range;
	OutWeights[1] = 1.f - OutWeights[0];
	OutWeights[2] = 0.f;
	return true;
}

bool DelaunayTriangulator2D::GetAnimsAndWeights_Triangular
(
	const Vector2D & NormalizedPosition,
	array<const AnimationClip*, 3>& OutClips,
	array<float, 3> & OutWeights
) const
{
	for (const array<int, 3> & Indices : TriangleVertexIndices)
	{
		const Triangle2D Triangle = {
			this->Samples[Indices[0]].Position,
			this->Samples[Indices[1]].Position,
			this->Samples[Indices[2]].Position
		};
		if (Triangle.Contains(NormalizedPosition) == true)
		{
			Triangle.GetBarycentric(NormalizedPosition, OutWeights);
			OutClips = {
				this->Samples[Indices[0]].Animation,
				this->Samples[Indices[1]].Animation,
				this->Samples[Indices[2]].Animation
			};
			return true;
		}
	}
	return false;
}

bool DelaunayTriangulator2D::GetVerticesAndWeights_Collinear(
	const Vector2D & NormalizedPosition,
	array<Vector2D, 3> & OutVertices,
	array<float, 3> & OutWeights
) const
{
	const float Distance = NormalizedPosition | SpanDir;
	const auto It2 = SegmentVertexIndices.lower_bound(Distance);
	const auto It1 = (It2 == SegmentVertexIndices.cbegin() || Math::NearEqual(It2->first, Distance)) ? It2 : std::prev(It2);
	const float Value1 = It1->first;
	const float Value2 = It2->first;
	OutVertices[0] = Samples[It1->second].Position;
	OutVertices[1] = Samples[It2->second].Position;
	OutVertices[2] = Samples[It2->second].Position;
	const float Range = Value2 - Value1;
	OutWeights[0] = Math::IsZero(Range) ? 1.f : (Distance - Value1) / Range;
	OutWeights[1] = 1.f - OutWeights[0];
	OutWeights[2] = 0.f;
	return true;
}

bool DelaunayTriangulator2D::GetVerticesAndWeights_Triangular(
	const Vector2D & NormalizedPosition,
	array<Vector2D, 3> & OutVertices,
	array<float, 3> & OutWeights
) const
{
	for (const array<int, 3> & Indices : TriangleVertexIndices)
	{
		const Triangle2D Triangle = {
			this->Samples[Indices[0]].Position,
			this->Samples[Indices[1]].Position,
			this->Samples[Indices[2]].Position
		};
		if (Triangle.Contains(NormalizedPosition) == true)
		{
			Triangle.GetBarycentric(NormalizedPosition, OutWeights);
			OutVertices = Triangle.GetVertices();
			return true;
		}
	}
	return false;
}
