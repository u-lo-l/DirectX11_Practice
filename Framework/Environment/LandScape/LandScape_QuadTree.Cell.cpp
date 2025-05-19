#include "framework.h"
#include "LandScape_QuadTree.h"



LandScape_QuadTree::LandScapeCell::LandScapeCell(
	Vector CellExtend,
	Vector LocalPosition,
	float GridSize
)
{
	
}

LandScape_QuadTree::LandScapeCell::~LandScapeCell()
{
}

void LandScape_QuadTree::LandScapeCell::Tick()
{
}

void LandScape_QuadTree::LandScapeCell::Render(
	HlslShader<VertexType>* InShader
)
{
	const Frustum * const ViewFrustum = Context::Get()->GetCamera()->GetViewFrustum();
	if (ViewFrustum->Intersects(BoundingBox) == false)
		return;
	InShader->DrawIndexed(Indices.size());
}
