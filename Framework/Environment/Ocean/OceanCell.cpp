#include "framework.h"
#include "OceanCell.h"

OceanCell::OceanCell(const SceneryCellDesc& InDesc)
	: ARenderable()
{
	this->Tf->SetParent(InDesc.Parent);
	ARenderable::SetName(InDesc.Name);
	ARenderable::SetMaterial(InDesc.OceanMat);
	ARenderable::SetShader();
	this->DisplacementMap = InDesc.DisplacementMap;
	this->NormalMap = InDesc.NormalMap;
	this->FoamGrid = InDesc.FoamGrid;
	CreateVertices(InDesc.Dimension.X, InDesc.Dimension.Z, InDesc.CellSize, InDesc.GridSize);
	ASSERT(this->Vertices.empty() == false, "Vertices should not be empty");
	CreateIndices(static_cast<float>(InDesc.CellSize), InDesc.GridSize);
	ASSERT(this->Indices.empty() == false, "Indices should not be empty");
	CreateInstances(InDesc.Dimension.X, InDesc.Dimension.Z, static_cast<float>(InDesc.CellSize));
	ASSERT(this->Instances.empty() == false, "Instances should not be empty");

	ARenderable::CreateVertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexType));
	ARenderable::CreateIndexBuffer(Indices.data(), Indices.size());
	ARenderable::CreateInstanceBuffer(Instances.data(), Instances.size(), sizeof(InstanceType));

	CB_PerOceanData.GridSize = InDesc.GridSize;
	CB_PerOceanData.HeightScaler = InDesc.Dimension.Y;
	CB_PerOceanData.TerrainSize = {InDesc.Dimension.X, InDesc.Dimension.Y};
	CB_PerOceanData.TextureSize = {(float)DisplacementMap->GetWidth(), (float)DisplacementMap->GetWidth()} ;
	CB_PerOceanData.TexelSize = 1.f / CB_PerOceanData.TextureSize;

	CB_PerOceanData.LODRange = {1, 3};
	CB_PerOceanData.ScreenDistance = D3D::GetDesc().WindowHeight * 0.5f * Context::Get()->GetCamera()->GetProjectionMatrix().M22;;
	CB_PerOceanData.ScreenDiagonal = Vector2D(D3D::GetDesc().WindowHeight, D3D::GetDesc().WindowWidth).Length();
	
	CB_PerOcean = new ConstantBuffer(
		ShaderType::HD,
		2,
		&CB_PerOceanData,
		sizeof(LandScapeTessellationDesc),
		false
	);
	
	RenderManager::Get()->AddRenderable(this);
}

OceanCell::~OceanCell()
{
	SAFE_DELETE(CB_PerOcean);
	SAFE_DELETE(NormalMap);
	Vertices.clear();
	Instances.clear();
}

void OceanCell::SetHeightScaler(float InHeightScaler)
{
	if (CB_PerOceanData.HeightScaler == InHeightScaler)
		return;
	CB_PerOceanData.HeightScaler = InHeightScaler;
	CB_PerOcean->UpdateData(&CB_PerOceanData, sizeof(CB_PerOceanData));
}

void OceanCell::BindResources() const
{
	BindBuffer();

	static bool Capture = false;
	ImGui::Checkbox("Capture", &Capture);
	if (Capture == true)
	{
		DisplacementMap->SaveOutputAsFile(L"Debug/DisplaceTEST");
		Capture = false;
	}
	
	CHECK(!!DisplacementMap);
	DisplacementMap->BindToGPUAsSRV(0, ShaderType::VDP);
	NormalMap->BindToGPUAsSRV(1, ShaderType::PixelShader);
	FoamGrid->BindToGPUAsSRV(2, ShaderType::DP);
	if (!!CB_PerOcean)
		CB_PerOcean->BindToGPU(ShaderType::ALL, 2);
}

void OceanCell::CreateVertices(float DimensionX, float DimensionZ, UINT CellSize, float GridSize)
{
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
				(Vertices[Index].Position.X) / DimensionX,
				(Vertices[Index].Position.Z) / DimensionZ
			);
		}
	}
}

void OceanCell::CreateIndices(float CellSize, float GridSize)
{
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

void OceanCell::CreateInstances(float TerrainDimensionX, float TerrainDimensionZ, float CellSize)
{
	const UINT CellCount_X = static_cast<UINT>(TerrainDimensionX / CellSize);
	const UINT CellCount_Z = static_cast<UINT>(TerrainDimensionZ / CellSize);
	
	Instances.clear();
	Instances.resize(CellCount_X * CellCount_Z);
	const Matrix & World = Tf->GetWorldMatrix(); 
	for (UINT Z = 0 ; Z < CellCount_Z; Z++)
	{
		for (UINT X = 0 ; X < CellCount_X ; X++)
		{
			Vector Position = {X * CellSize, 0.f, Z * CellSize};
			UINT Index = Z * CellCount_X + X;
			Instances[Index].LocalTransform = Matrix::CreateTranslation(Position) * World;
			Instances[Index].TexCoord = {Position.X / TerrainDimensionX, Position.Z / TerrainDimensionZ};
		}
	}
}



