#pragma once

class LandScape;

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
		Color LightColor;
		Vector LightDirection = Vector(0, 0, 0);
		float HeightScaler = 30.f;
		Vector2D TerrainSize;
		Vector2D TexelSize;
	};
	struct DensityDistanceDesc
	{
		Vector CameraPosition;
		float Padding;

		float MinAltitude = 0;
		float MaxAltitude = 0;
		float Near = 100;
		float Far = 400;
	};
	WVPDesc WVPData;
	Matrix ViewInverse_GS;
	TerrainHeightDesc TerrainHeightData;
	DensityDistanceDesc DensityDistanceData;
public:
	explicit Foliage(const LandScape * InTerrain, float MinAltitude = 0, float MaxAltitude = 100);
	~Foliage();

	void Tick();
	void Render() const;

	void Add(const vector<VertexType>& InVertices);
	void Add(const Vector& InPosition, const Vector2D& InScale, UINT InMapIndex);
	void AddTexture(const wstring& InPath);
	void CreateRandomFoliage();
	void SetTerrainHeightScaler(float InTerrainHeightScaler);
private:
	void UpdateVBuffer();

	float FoliageStride = 1.f;

	wstring ShaderName;
	HlslShader<VertexType> * CrossQuadShader = nullptr;

	vector<VertexType> Vertices;
	UINT VertexCount;

	VertexBuffer * VBuffer;
	
	vector<wstring> TextureNames;
	TextureArray * CrossQuadTextures;
	const LandScape * Terrain;
	
	ConstantBuffer * WVPBuffer;
	ConstantBuffer * TerrainHeightCBuffer;
	ConstantBuffer * DensityDistanceCBuffer;
};
