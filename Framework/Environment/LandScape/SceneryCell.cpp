// ReSharper disable CppClangTidyBugproneNarrowingConversions

#include "framework.h"
#include "SceneryCell.h"


TerrainCellTest::TerrainCellTest(const SceneryCellDesc& InDesc)
	: ARenderable()
{
	ARenderable::SetName(InDesc.Name);
	ARenderable::SetMaterial(InDesc.TerrainMat);
	ARenderable::SetShader();
	this->HeightMap = InDesc.HeightMap;
	CreateVertices(InDesc.TerrainDimension.X, InDesc.TerrainDimension.Z, InDesc.CellSize, InDesc.GridSize);
	ASSERT(this->Vertices.empty() == false, "Vertices should not be empty");
	CreateIndices(static_cast<float>(InDesc.CellSize), InDesc.GridSize);
	ASSERT(this->Indices.empty() == false, "Indices should not be empty");
	CreateInstances(InDesc.TerrainDimension.X, InDesc.TerrainDimension.Z, static_cast<float>(InDesc.CellSize));
	ASSERT(this->Instances.empty() == false, "Instances should not be empty");

	ARenderable::CreateVertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexType));
	ARenderable::CreateIndexBuffer(Indices.data(), Indices.size());
	ARenderable::CreateInstanceBuffer(Instances.data(), Instances.size(), sizeof(InstanceType));

	CB_PerTerrainData.GridSize = InDesc.GridSize;
	CB_PerTerrainData.HeightScaler = InDesc.TerrainDimension.Y;
	CB_PerTerrainData.TerrainSize = {InDesc.TerrainDimension.X, InDesc.TerrainDimension.Y};
	CB_PerTerrainData.TextureSize = {(float)HeightMap->GetWidth(), (float)HeightMap->GetWidth()} ;
	CB_PerTerrainData.TexelSize = 1.f / CB_PerTerrainData.TextureSize;
		
	CB_PerTerrainData.LODRange = {1, 3};
	CB_PerTerrainData.ScreenDistance = D3D::GetDesc().WindowHeight * 0.5f * Context::Get()->GetCamera()->GetProjectionMatrix().M22;;
	CB_PerTerrainData.ScreenDiagonal = Vector2D(D3D::GetDesc().WindowHeight, D3D::GetDesc().WindowWidth).Length();
	
	CB_PerTerrain = new ConstantBuffer(
		ShaderType::HD,
		2,
		&CB_PerTerrainData,
		sizeof(LandScapeTessellationDesc),
		false
	);
	
	RenderManager::Get()->AddRenderable(this);
}

TerrainCellTest::~TerrainCellTest()
{
	SAFE_DELETE(CB_PerTerrain);
}

void TerrainCellTest::BindResources() const
{
	BindBuffer();

	CHECK(!!HeightMap);
	HeightMap->BindToGPU(0, ShaderType::VDP);
	if (!!CB_PerTerrain)
		CB_PerTerrain->BindToGPU(ShaderType::ALL, 2);
}

void TerrainCellTest::CreateVertices
(
	float TerrainDimensionX,
	float TerrainDimensionZ,
	UINT  CellSize, 
	float GridSize
)
{
	// Vertices = {
	// 	{Vector(-10.f, -10.f, 0), Vector2D(0, 0)},
	// 	{Vector(-10.f, 10.f, 0), Vector2D(0, 1)},
	// 	{Vector(10.f, 10.f, 0), Vector2D(1, 1)},
	// 	{Vector(10.f, -10.f, 0), Vector2D(1, 0)},
	// };
	// return ;
	//
	const UINT GridPerCell     = static_cast<UINT>(CellSize / GridSize);
	const UINT VertexPerCell_X = GridPerCell + 1;
	const UINT VertexPerCell_Z = GridPerCell + 1;
	
	Vertices.clear();
	Vertices.resize(VertexPerCell_X * VertexPerCell_Z);
	
	for (UINT Z = 0 ; Z < VertexPerCell_Z ; Z++)
	{
		for (UINT X = 0 ; X < VertexPerCell_X ; X++)
		{
			UINT Index = Z * VertexPerCell_X + X;
			Vertices[Index].Position = Vector(static_cast<float>(X), 0, static_cast<float>(Z)) * GridSize;
			Vertices[Index].UV = Vector2D(
				(Vertices[Index].Position.X) / TerrainDimensionX,
				(Vertices[Index].Position.Z) / TerrainDimensionZ
			);
		}
	}
}

void TerrainCellTest::CreateIndices(float CellSize, float GridSize)
{
	// Indices = {
	// 	0, 1, 2, 3
	// };
	// return ;
	const UINT GridPerCell = static_cast<UINT>(CellSize / GridSize);  
	const UINT VertexPerCell = GridPerCell + 1;
	
	const UINT IndexCount = (GridPerCell) * (GridPerCell) * 4;

	Indices.clear();
	Indices.resize(IndexCount);
	UINT Index = 0;
	for (UINT Z = 0 ; Z < VertexPerCell - 1 ; Z++)
	{
		for (UINT X = 0 ; X < VertexPerCell  - 1; X++)
		{
			Indices[Index++] = VertexPerCell * Z + X;
			Indices[Index++] = VertexPerCell * (Z + 1) + X;
			Indices[Index++] = VertexPerCell * (Z + 1) + (X + 1);
			Indices[Index++] = VertexPerCell * Z + (X + 1);
		}
	}
}

void TerrainCellTest::CreateInstances
(
	float TerrainDimensionX,
	float TerrainDimensionZ,
	float CellSize
)
{
	const UINT CellCount_X = static_cast<UINT>(TerrainDimensionX / CellSize);
	const UINT CellCount_Z = static_cast<UINT>(TerrainDimensionZ / CellSize);
	
	Instances.clear();
	Instances.resize(CellCount_X * CellCount_Z);
	for (UINT Z = 0 ; Z < CellCount_Z; Z++)
	{
		for (UINT X = 0 ; X < CellCount_X ; X++)
		{
			Vector Position = {X * CellSize, 0.f, Z * CellSize};
			UINT Index = Z * CellCount_X + X;
			Instances[Index].LocalTransform = Matrix::CreateTranslation(Position);
			Instances[Index].TexCoord = {Position.X / TerrainDimensionX, Position.Z / TerrainDimensionZ};
			// TODO : MaxHeight, MinHeight 추가. InHeightMapValues[HeightMapIndex].R 필요
		}
	}
	// TODO : Move To CreateInstances
	// UINT HeightMap_Z = static_cast<UINT>(InCellIndex.Y) * GridPerCell_Y + Z;
	// UINT HeightMap_X = static_cast<UINT>(InCellIndex.X) * GridPerCell_X + X;
	// UINT HeightMapIndex = HeightMap_Z * VertexPerScenery_X + HeightMap_X;
	// float Y = InHeightMapValues[HeightMapIndex].R * HeightScaler;
	// YMin = min(YMin, Y);
	// YMax = max(YMax, Y);
	// TODO : Move To CreateInstance
	// Vertices[CellIndex].UV = Vector2D(
	// 	(Vertices[CellIndex].Position.X + PositionOffset) / SceneryWith,
	// 	(Vertices[CellIndex].Position.Z + PositionOffset) / SceneryHeight
	// );
}

// SceneryCell::SceneryCell(
// 	const Vector & InCellExtend,
// 	const Vector2D & InCellStartIndex,
// 	float InGridSize,
// 	const vector<Color> & InHeightMapValues,
// 	const Vector& InSceneryExtend
// )
// {
// 	const Vector2D ScenerySize = Vector2D(
// 		InSceneryExtend.X,
// 		InSceneryExtend.Z
// 	);
// 	BoundingBox = new Box({0,0,0}, InCellExtend);
// 	this->GridSize = static_cast<UINT>(InGridSize);
// 	
// 	CreateVertex(
// 		InHeightMapValues,
// 		ScenerySize,
// 		InCellStartIndex
// 	);
// 	CreateIndex();
// }
//
// SceneryCell::SceneryCell(
// 	const Vector & InCellExtend,
// 	const Vector2D & InCellStartIndex,
// 	float InGridSize,
// 	const float Height,
// 	const Vector& InSceneryExtend
// )
// {
// 	const Vector2D ScenerySize = Vector2D(
// 		InSceneryExtend.X,
// 		InSceneryExtend.Z
// 	);
// 	this->GridSize = static_cast<UINT>(InGridSize);
// 	BoundingBox = new Box({0,0,0}, InCellExtend);
// 	
// 	CreateVertex(
// 		Height,
// 		ScenerySize,
// 		InCellStartIndex
// 	);
// 	CreateIndex();
// }
//
// SceneryCell::~SceneryCell()
// {
// 	SAFE_DELETE(VBuffer);
// 	SAFE_DELETE(IBuffer);
// 	SAFE_DELETE(BoundingBox);
// }
//
// void SceneryCell::Tick()
// {
// 	// Do Nothing
// }
//
// bool SceneryCell::Render
// (
// 	RenderingShader * InShader,
// 	const Frustum * InFrustum
// ) const
// {
// 	// if (!!InFrustum && InFrustum->Intersects(BoundingBox->GetCenter(), BoundingBox->GetDiagonal()) == false)
// 	// 	return false;
// 	// if (!!InFrustum && InFrustum->Intersects(BoundingBox) == false)
// 	// 	return false;
// 	//
// 	// VBuffer->BindToGPU();
// 	// IBuffer->BindToGPU();
// 	// InShader->DrawIndexed(Indices.size());
// 	return true;
// }
//
// void SceneryCell::SetDimension(const Vector& InDimension) const
// {
// 	BoundingBox->SetDimension(InDimension);
// }
//
// void SceneryCell::SetCenter(const Vector& InPosition) const
// {
// 	BoundingBox->SetCenter(InPosition);
// }
//
// Vector SceneryCell::GetDimension() const
// {
// 	return BoundingBox->GetDimension();
// }
//
// Vector SceneryCell::GetWorldPosition() const
// {
// 	return BoundingBox->GetCenter();
// }
//
// Vector SceneryCell::GetLocalPosition() const
// {
// 	return LocalPosition;
// }
//
// VertexBuffer* SceneryCell::GetVertexBuffer() const
// {
// 	return VBuffer;
// }
//
// IndexBuffer* SceneryCell::GetIndexBuffer() const
// {
// 	return IBuffer;
// }
//
// void SceneryCell::CreateVertex
// (
// 	const vector<Color> & InHeightMapValues,
// 	const Vector2D & InScenerySize,
// 	const Vector2D & InCellIndex
// )
// {
// 	float YMin = FLT_MAX;
// 	float YMax = -FLT_MAX;
// 	const float SceneryWith = InScenerySize.X;	// 실제 크기 _X
// 	const float SceneryHeight = InScenerySize.Y;// 실제 크기 _Z
// 	const float CellWidth = (BoundingBox->GetDimension().X);		// Cell 실제 크기_X
// 	const float CellHeight = (BoundingBox->GetDimension().Z);	// Cell 실제 크기_Z
// 	
// 	const Vector PositionOffset = Vector(
// 		InCellIndex.X * CellWidth,
// 		0,
// 		InCellIndex.Y * CellHeight
// 	);	// Cell의 좌상단 위치
//
// 	const UINT GridPerCell_X = static_cast<UINT>(CellWidth) / GridSize;
// 	const UINT GridPerCell_Y = static_cast<UINT>(CellHeight) / GridSize;
// 	const UINT VertexPerCell_X = GridPerCell_X + 1;
// 	const UINT VertexPerCell_Z = GridPerCell_Y + 1;
// 	
// 	const UINT CellPerScenery_X = static_cast<UINT>(SceneryWith / CellWidth);
// 	const UINT GridPerScenery_X = CellPerScenery_X * GridPerCell_X;
// 	const UINT VertexPerScenery_X = GridPerScenery_X + 1;
//
// 	Vertices.clear();
// 	Vertices.resize(VertexPerCell_X * VertexPerCell_Z);
// 	
// 	const float HeightScaler = BoundingBox->GetDimension().Y;
// 	for (UINT Z = 0 ; Z < VertexPerCell_Z ; Z++)
// 	{
// 		UINT HeightMap_Z = static_cast<UINT>(InCellIndex.Y) * GridPerCell_Y + Z;
// 		for (UINT X = 0 ; X < VertexPerCell_X ; X++)
// 		{
// 			UINT HeightMap_X = static_cast<UINT>(InCellIndex.X) * GridPerCell_X + X;
// 			
// 			UINT CellIndex = Z * VertexPerCell_X + X;
// 			UINT HeightMapIndex = HeightMap_Z * VertexPerScenery_X + HeightMap_X;
// 			
// 			float Y = InHeightMapValues[HeightMapIndex].R * HeightScaler;
// 			YMin = min(YMin, Y);
// 			YMax = max(YMax, Y);
//
// 			Vertices[CellIndex].Position = Vector(static_cast<float>(X), 0, static_cast<float>(Z)) * static_cast<float>(GridSize);
// 			Vertices[CellIndex].Position += PositionOffset;
// 			Vertices[CellIndex].Normal = Vector(0, 1, 0);
// 			Vertices[CellIndex].UV = Vector2D(
// 				(Vertices[CellIndex].Position.X) / SceneryWith,
// 				(Vertices[CellIndex].Position.Z) / SceneryHeight
// 			);
// 		}
// 	}
//
// 	BoundingBox->SetDimension({BoundingBox->GetDimension().X, YMax - YMin, BoundingBox->GetDimension().Z});
// 	LocalPosition = Vector(PositionOffset.X, YMin, PositionOffset.Z);
// 	const Vector Center = this->LocalPosition + BoundingBox->GetDimension() * 0.5f;
// 	BoundingBox->SetCenter(Center);
// 	// BoundingBox = new Box(Center, BoundingBox->GetExtent() * 0.5f);
// 	VBuffer = new VertexBuffer(
// 		Vertices.data(),
// 		Vertices.size(),
// 		sizeof(VertexType)
// 	);
// }
//
// void SceneryCell::CreateVertex
// (
// 	const float InHeight,
// 	const Vector2D & InScenerySize,
// 	const Vector2D & InCellIndex
// )
// {
// 	const float SceneryWidth = InScenerySize.X;	// 실제 크기 _X
// 	const float SceneryHeight = InScenerySize.Y;// 실제 크기 _Z
// 	const float CellWidth = (BoundingBox->GetDimension().X);		// Cell 실제 크기_X
// 	const float CellHeight = (BoundingBox->GetDimension().Z);	// Cell 실제 크기_Z
// 	
// 	const Vector PositionOffset = Vector(
// 		InCellIndex.X * CellWidth,
// 		0,
// 		InCellIndex.Y * CellHeight
// 	);	// Cell의 좌상단 위치
//
// 	const UINT GridPerCell_X = static_cast<UINT>(CellWidth) / GridSize;
// 	const UINT GridPerCell_Y = static_cast<UINT>(CellHeight) / GridSize;
// 	const UINT VertexPerCell_X = GridPerCell_X + 1;
// 	const UINT VertexPerCell_Z = GridPerCell_Y + 1;
//
// 	Vertices.clear();
// 	Vertices.resize(VertexPerCell_X * VertexPerCell_Z);
// 	
// 	for (UINT Z = 0 ; Z < VertexPerCell_Z ; Z++)
// 	{
// 		for (UINT X = 0 ; X < VertexPerCell_X ; X++)
// 		{
// 			UINT CellIndex = Z * VertexPerCell_X + X;
// 			
// 			Vertices[CellIndex].Position = Vector(static_cast<float>(X), 0, static_cast<float>(Z)) * static_cast<float>(GridSize);
// 			Vertices[CellIndex].Position += PositionOffset;
// 			Vertices[CellIndex].Normal = Vector(0, 1, 0);
// 			Vertices[CellIndex].UV = Vector2D(
// 				(Vertices[CellIndex].Position.X) / SceneryWidth,
// 				(Vertices[CellIndex].Position.Z) / SceneryHeight
// 			);
// 		}
// 	}
//
// 	BoundingBox->SetDimension({BoundingBox->GetDimension().X, InHeight, BoundingBox->GetDimension().Z});
// 	LocalPosition = Vector(PositionOffset.X, -InHeight * 0.5f, PositionOffset.Z);
// 	const Vector Center = this->LocalPosition + BoundingBox->GetDimension() * 0.5f;
// 	BoundingBox->SetCenter(Center);
// 	VBuffer = new VertexBuffer(
// 		Vertices.data(),
// 		Vertices.size(),
// 		sizeof(VertexType)
// 	);
// }
//
// void SceneryCell::CreateIndex()
// {
// 	const float CellWidth = (BoundingBox->GetDimension().X);
// 	const float CellHeight = (BoundingBox->GetDimension().Z);
//
// 	const UINT GridPerCell_X = static_cast<UINT>(CellWidth) / GridSize;
// 	const UINT GridPerCell_Y = static_cast<UINT>(CellHeight) / GridSize;
// 	const UINT VertexPerCell_X = GridPerCell_X + 1;
// 	const UINT VertexPerCell_Z = GridPerCell_Y + 1;
// 	
// 	const UINT IndexCount = (GridPerCell_X) * (GridPerCell_Y) * 4;
//
// 	Indices.clear();
// 	Indices.resize(IndexCount);
// 	UINT Index = 0;
// 	for (UINT Z = 0 ; Z < VertexPerCell_Z - 1 ; Z++)
// 	{
// 		for (UINT X = 0 ; X < VertexPerCell_X  - 1; X++)
// 		{
// 			Indices[Index++] = VertexPerCell_X * Z + X;
// 			Indices[Index++] = VertexPerCell_X * (Z + 1) + X;
// 			Indices[Index++] = VertexPerCell_X * (Z + 1) + (X + 1);
// 			Indices[Index++] = VertexPerCell_X * Z + (X + 1);
// 		}
// 	}
// 	IBuffer = new IndexBuffer(
// 		Indices.data(), Indices.size()
// 	);
// }
