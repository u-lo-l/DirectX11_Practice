#pragma once

class TerrainQuadTree
{
	struct TerrainQuadTreeNode
	{
		TerrainQuadTreeNode(const Vector & LeftFrontBottom, const Vector & Extent);
		bool IsFrustumCulled(const Frustum * InFrustum) const;
		void Render();
		void Tick();
	private:
		using VertexType = VertexTextureNormal;
		Vector Center;
		Box    BoundingBox;
		float  SphereRadius = 0;

		VertexBuffer<VertexType> * VBuffer = nullptr;
		IndexBuffer * IBuffer = nullptr;

		TerrainQuadTreeNode * Children[4] = {nullptr, };
	};
public:
	explicit TerrainQuadTree(const Terrain2 * InTerrain);
	~TerrainQuadTree() = default;
private:
	TerrainQuadTreeNode * Root;
	
};