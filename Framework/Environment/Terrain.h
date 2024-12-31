#pragma once

class Terrain
{
private:
	using TerrainVertexType = VertexNormal;
public:
	Terrain(const wstring & InShaderFileName, const wstring& InHeightMapFileName);
	~Terrain();

	void Tick();
	void Render() const;

	__forceinline void SetPass( const int InPass ) { Pass = InPass; }
private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();
	void CreateBuffer();
private:
	int Pass = 1;
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
