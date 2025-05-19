#pragma once
#include "Renders/Shader/HlslShader.h"

class ConstantBuffer;
class Transform;
class IndexBuffer;
class Texture;
class VertexBuffer;
struct VertexTextureNormal;

class LandScape
{
public:
	struct LandScapeDesc
	{
		UINT	TerrainDim_X;
		UINT	TerrainDim_Z;
		float   HeightScaler;
		UINT    PatchSize;
		wstring HeightMapName;
		vector<wstring> DiffuseMapNames;
		vector<wstring> NormalMapNames;
	};
private:
	using VertexType = VertexTextureNormal;
	struct WVPDesc
	{
		Matrix World;
		Matrix View;
		Matrix Projection;
	};
	struct TerrainTessDesc
	{
		Vector CameraWorldPosition;
		float HeightScaler = 100.f;
		
		Color LightColor;
		
		Vector LightDirection;
		float BumpScaler = 0.001f;
		
		Vector2D LODRange;
		Vector2D TexelSize;
		
		float  ScreenDistance;
		float  ScreenDiagonal;
		UINT   DiffuseMapCount = 0;
		UINT   NormalMapCount  = 0;

		float TerrainSize;
		float GridSize;
		float TextureSize;
		float Padding;
	};
	struct TextureBlendingDesc
	{
		// Distance Based
		float NearSize = 1.f;
		float FarSize = 0.1f;
		float StartOffset = -1000.f;
		float Range = 5000.f;
		// Perlin Noise
		float NoiseAmount = 1.f;
		float NoisePower  = 1.f;
		// Slope Based
		float SlopBias = 60;
		float SlopSharpness = 3;
		// Altitude Base
		float LowHeight = 5;
		float HighHeight = 15;
		float HeightSharpness = 1;
		// Padding
		UINT Padding = 0;
	};
	Vector2D LODRange;
public:
	explicit LandScape(const LandScapeDesc & Desc);
	explicit LandScape(const wstring & InHeightMapFilename, UINT PatchSize = 16);
	~LandScape();

	void Tick();
	void Render() const;

	UINT GetWidth() const;
	UINT GetHeight() const;
	float GetAltitude(const Vector2D & InPositionXZ) const;
	UINT GetPatchSize() const;
	const Texture * GetHeightMap() const;
	float GetHeightScale() const;
	void SetHeightScale(float InHeightScale);
private:
	void SetupShaders();
	void SetupResources(const LandScapeDesc& Desc);
	void CreateVertex();
	void CreateIndex();
	void CreatePerlinNoiseMap();
	
	UINT PatchSize = 4;
	UINT TerrainDimension[2] = {1024, 1024};
	vector<VertexType> Vertices;
	vector<UINT> Indices;
	WVPDesc WVP;
	TerrainTessDesc TerrainTessData;
	TextureBlendingDesc TextureBlendingData;
	
	HlslShader<VertexType> * Shader;
	
	Texture * HeightMap = nullptr;
	// RWTexture2D * PerlinNoiseMap = nullptr;
	Texture * PerlinNoiseMap = nullptr;
	Texture * VariationMap = nullptr;
	TextureArray * DetailDiffuseMaps = nullptr;
	TextureArray * DetailNormalMaps = nullptr;
	
	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;

	Transform * Tf;
	ConstantBuffer * CB_WVP = nullptr;
	ConstantBuffer * CB_TerrainData = nullptr;
	ConstantBuffer * CB_TextureBlending = nullptr;
};


