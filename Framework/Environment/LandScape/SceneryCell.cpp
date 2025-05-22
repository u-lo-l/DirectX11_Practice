#include "framework.h"
#include "SceneryCell.h"

SceneryCell::SceneryCell(
	const Vector & InCellExtend,
	const Vector2D & InCellStartIndex,
	float InGridSize,
	const vector<Color> & InHeightMapValues,
	const Vector& InTerrainExtend
)
{
	const Vector2D ScenerySize = Vector2D(
		InTerrainExtend.X,
		InTerrainExtend.Z
	);
	this->CellExtent = InCellExtend;
	this->GridSize = static_cast<UINT>(InGridSize);
	
	CreateVertex(
		InHeightMapValues,
		ScenerySize,
		InCellStartIndex
	);
	CreateIndex();
}

SceneryCell::~SceneryCell()
{
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(BoundingBox);
}

void SceneryCell::Tick()
{
	// Do Nothing
	
}

bool SceneryCell::Render(HlslShader<VertexType>* InShader, const Frustum* InFrustum)
{
	if (!!InFrustum && InFrustum->Intersects(BoundingBox) == false)
		return false;
	
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	InShader->DrawIndexed(Indices.size());
	return true;
}

void SceneryCell::CreateVertex
(
	const vector<Color> & InHeightMapValues,
	const Vector2D & InTerrainSize,
	const Vector2D & InCellIndex
)
{
		float YMin = FLT_MAX;
	float YMax = -FLT_MAX;
	const float TerrainWidth = InTerrainSize.X;	// 실제 크기 _X
	const float TerrainHeight = InTerrainSize.Y;// 실제 크기 _Z
	const float CellWidth = (CellExtent.X);		// Cell 실제 크기_X
	const float CellHeight = (CellExtent.Z);	// Cell 실제 크기_Z
	
	const Vector PositionOffset = Vector(
		InCellIndex.X * CellWidth,
		0,
		InCellIndex.Y * CellHeight
	);	// Cell의 좌상단 위치

	const UINT GridPerCell_X = static_cast<UINT>(CellWidth) / GridSize;
	const UINT GridPerCell_Y = static_cast<UINT>(CellHeight) / GridSize;
	const UINT VertexPerCell_X = GridPerCell_X + 1;
	const UINT VertexPerCell_Z = GridPerCell_Y + 1;
	
	const UINT CellPerTerrain_X = static_cast<UINT>(TerrainWidth / CellWidth);
	const UINT GridPerTerrain_X = CellPerTerrain_X * GridPerCell_X;
	const UINT VertexPerTerrain_X = GridPerTerrain_X + 1;

	Vertices.clear();
	Vertices.resize(VertexPerCell_X * VertexPerCell_Z);
	
	const float HeightScaler = CellExtent.Y;
	for (UINT Z = 0 ; Z < VertexPerCell_Z ; Z++)
	{
		UINT HeightMap_Z = static_cast<UINT>(InCellIndex.Y) * GridPerCell_Y + Z;
		for (UINT X = 0 ; X < VertexPerCell_X ; X++)
		{
			UINT HeightMap_X = static_cast<UINT>(InCellIndex.X) * GridPerCell_X + X;
			
			UINT CellIndex = Z * VertexPerCell_X + X;
			UINT HeightMapIndex = HeightMap_Z * VertexPerTerrain_X + HeightMap_X;
			
			float Y = InHeightMapValues[HeightMapIndex].B * HeightScaler;
			YMin = min(YMin, Y);
			YMax = max(YMax, Y);

			Vertices[CellIndex].Position = Vector(X * GridSize, 0, Z * GridSize) + PositionOffset;
			Vertices[CellIndex].Normal = Vector(0, 1, 0);
			Vertices[CellIndex].UV = Vector2D(
				(Vertices[CellIndex].Position.X) / TerrainWidth,
				(Vertices[CellIndex].Position.Z) / TerrainHeight
			);
		}
	}

	CellExtent.Y = YMax - YMin;
	LocalPosition = Vector(PositionOffset.X, YMin, PositionOffset.Z);
	const Vector Center = this->LocalPosition + this->CellExtent * 0.5f;
	BoundingBox = new Box(Center, CellExtent * 0.5f);
	VBuffer = new VertexBuffer(
		Vertices.data(),
		Vertices.size(),
		sizeof(VertexType)
	);
}

void SceneryCell::CreateIndex()
{
	const float CellWidth = (CellExtent.X);
	const float CellHeight = (CellExtent.Z);

	const UINT GridPerCell_X = static_cast<UINT>(CellWidth) / GridSize;
	const UINT GridPerCell_Y = static_cast<UINT>(CellHeight) / GridSize;
	const UINT VertexPerCell_X = GridPerCell_X + 1;
	const UINT VertexPerCell_Z = GridPerCell_Y + 1;
	
	const UINT IndexCount = (GridPerCell_X) * (GridPerCell_Y) * 4;

	Indices.clear();
	Indices.resize(IndexCount);
	UINT Index = 0;
	for (UINT Z = 0 ; Z < VertexPerCell_Z - 1 ; Z++)
	{
		for (UINT X = 0 ; X < VertexPerCell_X  - 1; X++)
		{
			Indices[Index++] = VertexPerCell_X * Z + X;
			Indices[Index++] = VertexPerCell_X * (Z + 1) + X;
			Indices[Index++] = VertexPerCell_X * (Z + 1) + (X + 1);
			Indices[Index++] = VertexPerCell_X * Z + (X + 1);
		}
	}
	IBuffer = new IndexBuffer(
		Indices.data(), Indices.size()
	);
}
