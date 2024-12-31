#pragma once

class Terrain
{
private:
	using TerrainVertexType = Vertex;
public:
	Terrain(const wstring & InShaderFileName, const wstring& InHeightMapFileName);
	~Terrain();

	void Tick();
	void Render();

	__forceinline void SetPass( const int InPass ) { Pass = InPass; }
private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateBuffer();
private:
	int Pass = 1;
	Shader * Drawer = nullptr;
	Texture* HeightMap;

	UINT Width, Height;
	UINT VertexCount;
	TerrainVertexType* Vertices = nullptr;
	
	UINT IndexCount;
	UINT * Indices = nullptr;

	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;

	Matrix WorldMatrix;
};
