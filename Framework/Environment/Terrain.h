#pragma once

class Terrain
{
public:
	using TerrainVertexType = VertexNormal;
	Terrain(const wstring & InShaderFileName, const wstring& InHeightMapFileName);
	~Terrain();

	void Tick();
	void Render() const;

	void SetPass( const int InPass ) { Pass = InPass; }
	UINT GetWidth() const { return Width; }
	UINT GetHeight() const { return Height; }
	void GetAltitude(Vector& InPosition) const;
	
private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();
	void CreateBuffer();
private:
	int Pass = 0;
	// Shader * SkyShader = nullptr;
	HlslShader<TerrainVertexType> * Shader = nullptr;
	Texture* HeightMap;

	UINT Width, Height;
	UINT VertexCount;
	vector<TerrainVertexType> Vertices;
	
	UINT IndexCount;
	vector<UINT> Indices;

	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;

	Matrix WorldMat;
	ID3D11Buffer * WVPCBuffer = nullptr;
};
