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
	~SceneryCell();
	void Tick();
	bool Render(
		HlslShader<VertexType> * InShader,
		const Frustum * InFrustum = nullptr
	);
	Vector GetExtent() const { return CellExtent; }
	Vector GetLocalPosition() const { return LocalPosition; }

	VertexBuffer * GetVertexBuffer() const { return VBuffer; }
	IndexBuffer  * GetIndexBuffer()  const { return IBuffer; }
private:
	void CreateVertex(
		const vector<Color>& InHeightMapValues,
		const Vector2D & InTerrainSize,
		const Vector2D & InCellIndex
	);
	void CreateIndex();

	vector<VertexType> Vertices;
	vector<UINT> Indices;
	VertexBuffer * VBuffer = nullptr;
	IndexBuffer  * IBuffer = nullptr;

	Vector CellExtent;
	Vector LocalPosition; 
	UINT GridSize;
	Box * BoundingBox = nullptr;
	
};
