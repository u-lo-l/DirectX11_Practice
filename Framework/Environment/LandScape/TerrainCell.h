#pragma once
#include "LandScape.h"

struct VertexTerrainCell;

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
		Vector TerrainDimension;
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
		Vector2D TexelSize;

		Vector2D TerrainSize;
		Vector2D TextureSize;
		
		Vector2D LODRange;
		float    ScreenDistance;
		float    ScreenDiagonal;
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

	ConstantBuffer * CB_PerTerrain = nullptr;
};

// class SceneryCell
// {
// private:
// 	using VertexType = VertexTextureNormal;
// public:
// 	SceneryCell(
// 		const Vector& InCellExtend,
// 		const Vector2D& InCellStartIndex,
// 		float InGridSize,
// 		const vector<Color>& InHeightMapValues,
// 		const Vector& InTerrainExtend
// 	);
// 	SceneryCell(
// 		const Vector& InCellExtend,
// 		const Vector2D& InCellStartIndex,
// 		float InGridSize,
// 		float Height,
// 		const Vector& InSceneryExtend
// 	);
// 	~SceneryCell();
// 	void Tick();
// 	bool Render(
// 		RenderingShader * InShader,
// 		const Frustum * InFrustum = nullptr
// 	) const;
//
// 	void SetDimension(const Vector& InDimension) const;
// 	void SetCenter(const Vector& InPosition) const;
// 	Vector GetDimension() const;
// 	Vector GetWorldPosition() const;
// 	Vector GetLocalPosition() const;
//
// 	VertexBuffer * GetVertexBuffer() const;
// 	IndexBuffer  * GetIndexBuffer()  const;
// private:
// 	void CreateVertex(
// 		const vector<Color>& InHeightMapValues,
// 		const Vector2D & InScenerySize,
// 		const Vector2D & InCellIndex
// 	);
// 	void CreateVertex(
// 		float InHeight,
// 		const Vector2D & InScenerySize,
// 		const Vector2D & InCellIndex
// 	);
// 	void CreateIndex();
//
// 	vector<VertexType> Vertices;
// 	vector<UINT> Indices;
// 	VertexBuffer * VBuffer = nullptr;
// 	IndexBuffer  * IBuffer = nullptr;
//
// 	Vector LocalPosition;
// 	UINT  GridSize;
// 	Box * BoundingBox = nullptr;
// };
