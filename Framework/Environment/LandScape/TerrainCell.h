#pragma once

class TerrainCell final : public ARenderable
{
private:
	using VertexType = VertexTerrainCell;
public:
	struct SceneryCellDesc
	{
		string Name;
		const Texture * HeightMap;
		const Material * TerrainMat;
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
	explicit TerrainCell(const SceneryCellDesc & InDesc);
	virtual ~TerrainCell() override;
	void CreateNormalTangentMap();
	[[nodiscard]] const Texture * GetHeightMap() const;
	const RWTexture2D * GetNormalMap() const;
	const RWTexture2D * GetTangentMap() const;

private:
	struct LandScapeTessellationDesc
	{
		Matrix TerrainBaseWorldTF;

		float HeightScaler = 100.f;
		float GridSize;
		Vector2D Padding;

		Vector2D TerrainSize;
		Vector2D LODRange;
	} CB_PerTerrainData;
	void CreateVertices(float TerrainDimensionX, float TerrainDimensionZ, UINT CellSize, float GridSize);
	void CreateIndices(float CellSize, float GridSize);
	void CreateInstances(float TerrainDimensionX, float TerrainDimensionZ, float CellSize);
	virtual void BindResources() const override;
	
	const Texture * HeightMap = nullptr;
	RWTexture2D * NormalMap = nullptr;
	RWTexture2D * TangentMap = nullptr;
	vector<VertexType> Vertices = {};
	vector<UINT> Indices = {};
	vector<InstanceType> Instances = {};
};

