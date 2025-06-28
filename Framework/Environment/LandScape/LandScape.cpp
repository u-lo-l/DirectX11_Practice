#include "framework.h"
#include "LandScape.h"

LandScape::LandScape(const LandScapeDesc& InDesc)
	: Data(InDesc)
{
	SetupShaders();
	SetupResources(InDesc);
	SetupCells(InDesc);
}

LandScape::~LandScape()
{
	SAFE_DELETE(HeightMap);
	SAFE_DELETE(CellInstance);
	SAFE_DELETE(NormalMapGenerator);
	SAFE_DELETE(Tf)
}

void LandScape::Tick()
{
	if (!!CellInstance) CellInstance->Tick();
}

void LandScape::SetupShaders()
{
}

void LandScape::SetupResources(const LandScapeDesc& InDesc)
{
	// TODO : CreateTerrainNormalMap;
	// NormalMap = new RWTexture2D(HeightMap->GetWidth(), HeightMap->GetHeight());
	Tf = new Transform();
	// TessellationData.HeightScaler = Dimension.Y;
	// TessellationData.DiffuseMapCount = InDesc.DiffuseMaps.size();
	// TessellationData.NormalMapCount = InDesc.NormalMaps.size();
	// TessellationData.LODRange = {1, 3};
	// TessellationData.TexelSize.X = 1.f / static_cast<float>(HeightMap->GetWidth());
	// TessellationData.TexelSize.Y = 1.f / static_cast<float>(HeightMap->GetHeight());
	// TessellationData.TerrainSize = Dimension.X;
	// TessellationData.GridSize = static_cast<float>(GridSize);
	// TessellationData.TextureSize = static_cast<float>(HeightMap->GetWidth());
}

void LandScape::SetupCells(const LandScapeDesc& InDesc)
{
	this->HeightMap = new Texture(InDesc.HeightMapName, true);
	TerrainMaterial::MaterialDesc MatDesc;
	
	TerrainCellTest::SceneryCellDesc Desc;
	Desc.Name = "LandScape";
	Desc.HeightMap = this->HeightMap;
	MatDesc.HeightMap = this->HeightMap;
	Desc.TerrainMat = new TerrainMaterial(MatDesc);
	Desc.CellSize =  InDesc.CellSize;
	Desc.TerrainDimension = InDesc.Dimension;
	Desc.GridSize = InDesc.GridSize;

	this->CellInstance = new TerrainCellTest(Desc);
	this->CellInstance->GetTransform()->SetParent(this->Tf);

#pragma region Bounding Box
	// CellBoxVBuffer = new VertexBuffer(BoxVertices.data(), BoxVertices.size(), sizeof(VertexColor));
	// CellBoxIBuffer = new IndexBuffer(BoxIndices.data(), BoxIndices.size());
	// CellBoxInstBuffer = new InstanceBuffer(
	// 	CellLocalTransform.data(),
	// 	CellLocalTransform.size() + 1, // TODO : 왜 +1 해야 되는지 이유 파악하기.
	// 	sizeof(Matrix)
	// );
#pragma endregion Bounding Box
}
