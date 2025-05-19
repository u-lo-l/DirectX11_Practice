#include "framework.h"
#include "LandScape_QuadTree.h"

LandScape_QuadTree::LandScape_QuadTree(const LandScapeDesc& InDesc)
{
	SetupShaders();
	SetupResources();
	SetupCells();
}

LandScape_QuadTree::~LandScape_QuadTree()
{
	for (LandScapeCell * Cell : Cells)
		SAFE_DELETE(Cell);
	SAFE_DELETE(CellRenderer);
	SAFE_DELETE(BoundaryRenderer);
	SAFE_DELETE(MatrixData);
	SAFE_DELETE(LightData);
	SAFE_DELETE(BlendingData);
	SAFE_DELETE(TessellationData);
	SAFE_DELETE(CB_WVP);
	SAFE_DELETE(CB_Light);
	SAFE_DELETE(CB_Blending);
	SAFE_DELETE(CB_Tessellation);
}

void LandScape_QuadTree::Tick()
{
	CB_WVP->UpdateData(MatrixData, sizeof(WVPDesc));
	CB_Light->UpdateData(LightData, sizeof(DirectionalLightDesc));
	CB_Blending->UpdateData(BlendingData, sizeof(LandScapeBlendingDesc));
	CB_Tessellation->UpdateData(TessellationData, sizeof(LandScapeTessellationDesc));
}

void LandScape_QuadTree::Render(bool bDrawBoundary)
{
	CB_WVP->BindToGPU();
	CB_Light->BindToGPU();
	CB_Blending->BindToGPU();
	CB_Tessellation->BindToGPU();
	// RootCell->Render(CellRenderer, bDrawBoundary ? BoundaryRenderer : nullptr);
}

void LandScape_QuadTree::SetupShaders()
{
}

void LandScape_QuadTree::SetupResources()
{
}

void LandScape_QuadTree::SetupCells()
{
}
