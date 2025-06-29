// ReSharper disable CppClangTidyBugproneNarrowingConversions

#include "framework.h"
#include "TerrainCell.h"


TerrainCell::TerrainCell(const SceneryCellDesc& InDesc)
	: ARenderable()
{
	this->Tf->SetParent(InDesc.Parent);
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

	CreateNormalTangentMap();
	
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

TerrainCell::~TerrainCell()
{
	SAFE_DELETE(CB_PerTerrain);
}

void TerrainCell::CreateNormalTangentMap()
{
	const ComputeShader * const NormalMapCreator = ShaderManager::Get()->GetComputeShader("NormalMapCreator");
	ASSERT(!!NormalMapCreator, "NormalMapCreate Not Found");
	const UINT TextureWidth  = HeightMap->GetWidth();
	const UINT TextureHeight = HeightMap->GetHeight();
	NormalMap = new RWTexture2D(TextureWidth, TextureHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
	TangentMap= new RWTexture2D(TextureWidth, TextureHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
	struct CB_Desc
	{
		float HeightScaler = 1.f;
		UINT TextureWidth;
		UINT TextureHeight;
		float Padding;
	} CB_Data;
	CB_Data.HeightScaler = 2048.f;
	CB_Data.TextureWidth = TextureWidth;
	CB_Data.TextureHeight = TextureHeight;
	ConstantBuffer CB = ConstantBuffer(
		ShaderType::ComputeShader,0,
		&CB_Data,sizeof(CB_Data),true
	);
	CB.BindToGPU(ShaderType::ComputeShader, 0);
	HeightMap->BindToGPU(0, ShaderType::ComputeShader);
	NormalMap->BindToGPUAsUAV(0);
	TangentMap->BindToGPUAsUAV(1);
	UINT X, Y, Z;
	NormalMapCreator->GetThreadDim(X, Y, Z);
	NormalMapCreator->Dispatch(TextureWidth / X, TextureHeight / Y, 1);
}

const Texture* TerrainCell::GetHeightMap() const
{
	return HeightMap;
}

const RWTexture2D* TerrainCell::GetNormalMap() const
{
	return NormalMap;
}

const RWTexture2D* TerrainCell::GetTangentMap() const
{
	return TangentMap;
}

void TerrainCell::BindResources() const
{
	BindBuffer();

	CHECK(!!HeightMap);
	HeightMap->BindToGPU(0, ShaderType::VDP);
	NormalMap->BindToGPUAsSRV(1, ShaderType::VDP);
	TangentMap->BindToGPUAsSRV(2, ShaderType::VDP);
	if (!!CB_PerTerrain)
		CB_PerTerrain->BindToGPU(ShaderType::ALL, 2);
}

void TerrainCell::CreateVertices
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

void TerrainCell::CreateIndices(float CellSize, float GridSize)
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

void TerrainCell::CreateInstances
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
		}
	}
}
