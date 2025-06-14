#include "framework.h"
#include "AnimationBlendSpaceHelper.h"
#include <unordered_set>



DelaunayTriangulator2D::DelaunayTriangulator2D()
= default;

vector<const AnimationClip*> DelaunayTriangulator2D::GetAllAnimationClips() const
{
	vector<const AnimationClip*> result;
	for (const auto & Sample : Samples)
		result.push_back(Sample.Animation);
	return result;
}

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
		GenerateTriangleGraph();
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
	return bCollinear
		? GetAnimsAndWeights_Collinear(NormalizedPosition, OutClips, OutWeights)
		: GetAnimsAndWeights_Triangular(NormalizedPosition, OutClips, OutWeights);
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
	BlendSpace2DTriangleNode * SuperTriangleNode = new BlendSpace2DTriangleNode();
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

void DelaunayTriangulator2D::GenerateTriangleGraph()
{
	using Edge_t = pair<int, int>;

	// 정점이 N개일 때 Triangle의 최대 개수는 2N-4개이고 Edge의 최대 개수는 3N-6이다.
	std::unordered_map<Edge_t, vector<int>> EdgeTriangleMap;
	
	const int TriangleCount = this->TriangleVertexIndices.size();
	TriangleGraph.resize(TriangleCount);
	// Build Edge-TriangleMap
	for (int i = 0 ; i < TriangleCount; i++)
	{
		const array<int, 3> & TriangleIndices = this->TriangleVertexIndices[i];
		const array<Edge_t, 3> Edges = {
			minmax(TriangleIndices[0], TriangleIndices[1]),
			minmax(TriangleIndices[1], TriangleIndices[2]),
			minmax(TriangleIndices[2], TriangleIndices[0])
		};
		for (int e = 0; e < 3; e++)
		{
			EdgeTriangleMap[Edges[e]].push_back(i); 
		}
	}
	for (const auto & It : EdgeTriangleMap)
	{
		const Edge_t & Edge = It.first;
		const vector<int> & Indices = It.second;
		if (Indices.empty() || Indices.size() > 2)
		{
			ASSERT(false, String::Format("%s | Edge must belongs to One or Two Triangles", __FUNCTION__).c_str());
		}
		else if (Indices.size() == 2)
		{
			for (int i = 0 ; i < 2 ; i++)
			{
				const int MyIndex = Indices[i];
				const int NeighborIndex = Indices[1 - i];
				const array<int, 3> & MyTriangle = this->TriangleVertexIndices[MyIndex];
				for (int e = 0; e < 3; e++)
				{
					Edge_t TargetEdge = minmax(MyTriangle[e], MyTriangle[(e + 1) % 3]);
					if (TargetEdge == Edge)
					{
						TriangleGraph[MyIndex].NeighborsViaEdge[e] = NeighborIndex; 
						break;
					}
				}
			}
		}
		else // 가독성 위해 명시적으로 작성됨.
			continue; // Perimeter Edge
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

bool DelaunayTriangulator2D::GetSamplesAndWeights_Collinear(
	const Vector2D & NormalizedPosition,
	array<const BlendSpace2DAnimationSample *, 3> & OutSamples,
	array<float, 3> & OutWeights
) const
{
	const float ProjectedLength = (NormalizedPosition - SuperSegment[0]) | SpanDir;
	const auto It2 = SegmentVertexIndices.lower_bound(ProjectedLength);
	const auto It1 = (It2 == SegmentVertexIndices.cbegin() || Math::NearEqual(It2->first, ProjectedLength)) ? It2 : std::prev(It2);
	const float Value1 = It1->first;
	const float Value2 = It2->first;

	OutSamples = {&Samples[It1->second], &Samples[It2->second], &Samples[It2->second]};
	
	const float Range = Value2 - Value1;
	const float SegmentLength = Math::IsZero(Range) ? 0 : (ProjectedLength - Value1) / Range; 
	OutWeights[0] = 1 - SegmentLength;
	OutWeights[1] = SegmentLength;
	OutWeights[2] = 0.f;
	
	return true;
}

bool DelaunayTriangulator2D::GetSamplesAndWeights_Triangular
(
	const Vector2D& NormalizedPosition,
	array<const BlendSpace2DAnimationSample *, 3>& OutSamples,
	array<float, 3>& OutWeights
) const
{
	if (TriangleVertexIndices.empty())
		return false;
	struct History_t
	{
		int Index;
		float a;
		float b;
		float c;
	};
	vector<History_t> TempHistory;
	const int MaxIter = TriangleVertexIndices.size();
	vector<bool> Visited(MaxIter, false);
	int iter = 0;
	int TriangleIndex = TriangleVertexIndices.size() / 2;
	while (iter++ < MaxIter)
	{
		// 0. Setup
		if (TriangleIndex == INDEX_NONE || Visited[TriangleIndex])
			return false;
		Visited[TriangleIndex] = true;

		const array<int, 3> & VertexIndices = this->TriangleVertexIndices[TriangleIndex];
		OutSamples = { &Samples[VertexIndices[0]], &Samples[VertexIndices[1]], &Samples[VertexIndices[2]] };
		
		// 1. Calc Barycentric
		const Triangle2D Triangle = {
			OutSamples[0]->Position,
			OutSamples[1]->Position,
			OutSamples[2]->Position
		};
		Triangle.GetBarycentric(NormalizedPosition, OutWeights);
		TempHistory.push_back(
			{
				TriangleIndex,
				OutWeights[0],
				OutWeights[1],
				OutWeights[2]
			}
		);
		// 2. Search Next Triangle
		int HedgingEdge = INDEX_NONE;
		for (int V = 0 ; V < 3; V++)
		{
			if (OutWeights[V] < -Math::EPSILON)
			{
				const int FacingEdgeIndex = (V + 1) % 3; 
				HedgingEdge = FacingEdgeIndex;
				break;
			}
		}
		if (HedgingEdge == INDEX_NONE)
			return true;
		TriangleIndex = TriangleGraph[TriangleIndex].NeighborsViaEdge[HedgingEdge];
		if (static_cast<int>(TriangleGraph.size()) <= TriangleIndex)
		{
			ASSERT(false, String::Format("%s %d | INVALID INDEX", __FUNCTION__, __LINE__).c_str())
			return false;
		}
		// 3. Perimeter
		if (TriangleIndex == INDEX_NONE)
		{
			const Vector2D & EndPoint1 = Samples[VertexIndices[HedgingEdge]].Position;
			const Vector2D & EndPoint2 = Samples[VertexIndices[(HedgingEdge + 1) % 3]].Position;

			const float EdgeLength = Vector2D::Distance(EndPoint1, EndPoint2);
			if (Math::IsZero(EdgeLength))
				return false;
			
			const Vector2D EdgeDir = (EndPoint2 - EndPoint1).Normalized();
			const float ProjectionLength = (NormalizedPosition - EndPoint1) | EdgeDir;
			const float Weight = 1 - Math::Clamp01(ProjectionLength / EdgeLength);
			
			OutSamples[0] = &Samples[VertexIndices[HedgingEdge]];
			OutSamples[1] = &Samples[VertexIndices[(HedgingEdge + 1) % 3]];
			OutSamples[2] = OutSamples[1];
			OutWeights = {Weight, 1.f - Weight, 0.f};
			
			return true;
		}
	}
	return false;
}

bool DelaunayTriangulator2D::GetAnimsAndWeights_Collinear
(
	const Vector2D & NormalizedPosition,
	array<const AnimationClip*, 3>& OutClips,
	array<float, 3>& OutWeights
) const
{
	array<const BlendSpace2DAnimationSample *, 3> Samples;
	bool Result = GetSamplesAndWeights_Collinear(NormalizedPosition, Samples, OutWeights);
	if (Result == true)
		OutClips = {
		Samples[0]->Animation,
		Samples[1]->Animation,
		Samples[2]->Animation
	};
	return Result;
}

bool DelaunayTriangulator2D::GetAnimsAndWeights_Triangular
(
	const Vector2D & NormalizedPosition,
	array<const AnimationClip*, 3>& OutClips,
	array<float, 3> & OutWeights
) const
{
	array<const BlendSpace2DAnimationSample *, 3> Samples;
	bool Result = GetSamplesAndWeights_Triangular(NormalizedPosition, Samples, OutWeights);
	if (Result == true)
		OutClips = {
		Samples[0]->Animation,
		Samples[1]->Animation,
		Samples[2]->Animation
	};
	return Result;
	// 삼각형의 외부에 존재. 가장 근접한 삼각형의 Edge에 투영시켜야 함.
}

bool DelaunayTriangulator2D::GetVerticesAndWeights_Collinear(
	const Vector2D & NormalizedPosition,
	array<Vector2D, 3> & OutVertices,
	array<float, 3> & OutWeights
) const
{
	array<const BlendSpace2DAnimationSample *, 3> Samples;
	bool Result = GetSamplesAndWeights_Collinear(NormalizedPosition, Samples, OutWeights);
		if (Result == true)
		OutVertices = {
			Samples[0]->Position,
			Samples[1]->Position,
			Samples[2]->Position
		};
	return Result;
}

bool DelaunayTriangulator2D::GetVerticesAndWeights_Triangular(
	const Vector2D & NormalizedPosition,
	array<Vector2D, 3> & OutVertices,
	array<float, 3> & OutWeights
) const
{
	array<const BlendSpace2DAnimationSample *, 3> Samples;
	bool Result = GetSamplesAndWeights_Triangular(NormalizedPosition, Samples, OutWeights);
	if (Result == true)
		OutVertices = {
			Samples[0]->Position,
			Samples[1]->Position,
			Samples[2]->Position
		};
	return Result;
}
