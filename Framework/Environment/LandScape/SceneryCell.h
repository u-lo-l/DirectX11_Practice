#pragma once

class SceneryCell
{
private:
	using VertexType = VertexTextureNormal;
public:
	SceneryCell(
		const Vector& InCellExtend,
		const Vector2D& InCellStartIndex,
		float InGridSize,
		const vector<Color>& InHeightMapValues,
		const Vector& InTerrainExtend
	);
	SceneryCell(
		const Vector& InCellExtend,
		const Vector2D& InCellStartIndex,
		float InGridSize,
		float Height,
		const Vector& InSceneryExtend
	);
	~SceneryCell();
	void Tick();
	bool Render(
		HlslShader<VertexType> * InShader,
		const Frustum * InFrustum = nullptr
	) const;

	void SetDimension(const Vector& InDimension) const;
	void SetCenter(const Vector& InPosition) const;
	Vector GetDimension() const;
	Vector GetWorldPosition() const;
	Vector GetLocalPosition() const;

	VertexBuffer * GetVertexBuffer() const;
	IndexBuffer  * GetIndexBuffer()  const;
private:
	void CreateVertex(
		const vector<Color>& InHeightMapValues,
		const Vector2D & InScenerySize,
		const Vector2D & InCellIndex
	);
	void CreateVertex(
		float InHeight,
		const Vector2D & InScenerySize,
		const Vector2D & InCellIndex
	);
	void CreateIndex();

	vector<VertexType> Vertices;
	vector<UINT> Indices;
	VertexBuffer * VBuffer = nullptr;
	IndexBuffer  * IBuffer = nullptr;

	Vector LocalPosition;
	UINT  GridSize;
	Box * BoundingBox = nullptr;
};
