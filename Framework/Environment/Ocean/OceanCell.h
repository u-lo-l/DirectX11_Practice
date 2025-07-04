#pragma once
#include "OceanScape.h"

class OceanCell final : public ARenderable
{
	using VertexType = VertexTerrainCell;
public:
	struct SceneryCellDesc
	{
		string Name;
		const Material * OceanMat;
		UINT CellSize;
		Vector Dimension;
		float GridSize;
		Transform * Parent;
	};
	struct InstanceType
	{
		InstanceType()
			: LocalTransform(Matrix::Identity), TexCoord({0, 0}) {}
		InstanceType(const Matrix & InTransform, const Vector2D & InTexCoord)
			: LocalTransform(InTransform), TexCoord(InTexCoord) {}
		Matrix LocalTransform;
		Vector2D TexCoord;
	};
	explicit OceanCell(const SceneryCellDesc & InDesc);
	virtual ~OceanCell() override;
	void SetHeightScaler(float InHeightScaler);

private:
	struct LandScapeTessellationDesc
	{
		float HeightScaler = 100.f;
		float GridSize;
		Vector2D LODRange;
	} CB_PerOceanData;
	void CreateVertices(float DimensionX, float DimensionZ, UINT CellSize, float GridSize);
	void CreateIndices(float CellSize, float GridSize);
	void CreateInstances(float TerrainDimensionX, float TerrainDimensionZ, float CellSize);
	virtual void BindResources() const override;
	
	vector<VertexType> Vertices = {};
	vector<UINT> Indices = {};
	vector<InstanceType> Instances = {};
};
