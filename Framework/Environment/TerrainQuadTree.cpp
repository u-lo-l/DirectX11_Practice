#include "framework.h"
#include "TerrainQuadTree.h"

TerrainQuadTree::TerrainQuadTreeNode::TerrainQuadTreeNode(const Vector& LeftFrontBottom, const Vector& Extent)
	: Center(LeftFrontBottom + Extent * 0.5f)
	, BoundingBox(Box(Center, Extent))
	, SphereRadius(Extent.Length() * 0.5f)
{
}

bool TerrainQuadTree::TerrainQuadTreeNode::IsFrustumCulled(const Frustum * InFrustum) const
{
	if (InFrustum->Intersects(Center, SphereRadius) == false)
		return false;
	return InFrustum->Intersects(BoundingBox);
}

void TerrainQuadTree::TerrainQuadTreeNode::Tick()
{
}

void TerrainQuadTree::TerrainQuadTreeNode::Render()
{
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
}


TerrainQuadTree::TerrainQuadTree(const LandScape* InTerrain)
{
}
