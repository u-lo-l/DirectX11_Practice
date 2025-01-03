#pragma once

class Terrain
{
public:
	using TerrainVertexType = VertexNormal;
	Terrain(const wstring & InShaderFileName, const wstring& InHeightMapFileName);
	~Terrain();

	void Tick();
	void Render() const;

	__forceinline void SetPass( const int InPass ) { Pass = InPass; }
	__forceinline TerrainVertexType* GetVertice() const { return Vertices; }
	__forceinline UINT GetWidth() const { return Width; }
	__forceinline UINT GetHeight() const { return Height; }
	void GetPositionY(Vector& InPosition) const;
	
private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData() const;
	void CreateBuffer();
private:
	int Pass = 0;
	Shader * Drawer = nullptr;
	Texture* HeightMap;

	UINT Width, Height;
	UINT VertexCount;
	TerrainVertexType * Vertices = nullptr;
	
	UINT IndexCount;
	UINT * Indices = nullptr;

	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;

	Matrix WorldMatrix;
};
