#include "framework.h"
#include "LandScape.h"

LandScape::LandScape(const LandScapeDesc& InDesc)
	: Info(InDesc)
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

const Vector& LandScape::GetDimension() const
{
	return Info.Dimension;
}

const Texture* LandScape::GetHeightMap() const
{
	return CellInstance->GetHeightMap();
}

const RWTexture2D* LandScape::GetNormalMap() const
{
	return CellInstance->GetNormalMap();
}

const RWTexture2D* LandScape::GetTangentMap() const
{
	return CellInstance->GetTangentMap();
}

UINT LandScape::GetCellSize() const
{
	return Info.CellSize;
}

Transform * LandScape::GetTransform() const
{
	return Tf;
}

float LandScape::GetHeightScaler() const
{
	return Info.Dimension.Y;
}

void LandScape::SetupShaders()
{
}

void LandScape::SetupResources(const LandScapeDesc& InDesc)
{
	Tf = new Transform();
}

void LandScape::SetupCells(const LandScapeDesc& InDesc)
{
	this->HeightMap = new Texture(InDesc.HeightMapName, true);
	TerrainMaterial::MaterialDesc MatDesc;
	
	TerrainCell::SceneryCellDesc Desc;
	Desc.Name = "LandScape";
	Desc.HeightMap = this->HeightMap;
	MatDesc.HeightMap = this->HeightMap;
	Desc.TerrainMat = new TerrainMaterial(MatDesc);
	Desc.CellSize =  InDesc.CellSize;
	Desc.TerrainDimension = InDesc.Dimension;
	Desc.GridSize = InDesc.GridSize;
	Desc.Parent = this->Tf;

	this->CellInstance = new TerrainCell(Desc);

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
