#pragma once

class Terrain2;

class Foliage
{
private:
	using VertexType = VertexBillboard;
	struct WVPDesc
	{
		Matrix World;
		Matrix View;
		Matrix Projection;
	};
	struct TerrainHeightDesc
	{
		Vector Direction = Vector(0, 0, 0);
		float HeightScaler = 30.f;
		Vector2D TerrainSize;
		Vector2D TexelSize;
	};
	struct DensityDistanceDesc
	{
		Vector CameraPosition;
		float Near = 100;
		float Far = 400;
		float Padding[3];
	};
	WVPDesc WVPData;
	Matrix ViewInverse_GS;
	TerrainHeightDesc TerrainHeightData;
	DensityDistanceDesc DensityDistanceData;
public:
	explicit Foliage(const Terrain2 * InTerrain);
	~Foliage();

	void Tick();
	void Render() const;

	void Add(const vector<VertexType>& InVertices);
	void Add(const Vector& InPosition, const Vector2D& InScale, UINT InMapIndex);
	void AddTexture(const wstring& InPath);
	void CreateRandomFoliage();

private:
	void UpdateVBuffer();

	float FoliageStride = 0.75f;

	wstring ShaderName;
	HlslShader<VertexType> * CrossQuadShader = nullptr;

	vector<VertexType> Vertices;
	UINT VertexCount;

	VertexBuffer * VBuffer;
	
	vector<wstring> TextureNames;
	TextureArray * CrossQuadTextures;
	const Terrain2 * Terrain;
	
	ConstantBuffer * WVPBuffer;
	ConstantBuffer * TerrainHeightCBuffer;
	ConstantBuffer * DensityDistanceCBuffer;
};
