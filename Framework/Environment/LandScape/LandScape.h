#pragma once

class LandScape
{
public:
	// for Initialize
	struct LandScapeDesc
	{
		Vector	Dimension; // x, z : Horizontal, y : Vertical
		UINT	CellSize;  // Should Be Power Of 2
		UINT	GridSize;
		wstring HeightMapName;
		vector<wstring> DiffuseMaps;
		vector<wstring> NormalMaps;
	};
private:
	enum CBufferIndex
	{
		WVP = 0,
		Tessellation = 1,
		Lighting = 2,
		Blending = 3
	};
	struct LandScapeBlendingDesc
	{
		float NearSize = 1.0f;
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
		float Padding;
	};

	struct LandScapeTessellationDesc
	{
		Vector CameraWorldPosition;
		float HeightScaler = 100.f;
		
		float TerrainSize;
		float GridSize;
		float TextureSize;
		float Padding;
		
		Vector2D LODRange;
		Vector2D TexelSize;
		
		float  ScreenDistance;
		float  ScreenDiagonal;
		UINT   DiffuseMapCount = 0;
		UINT   NormalMapCount  = 0;
	};
	using VertexType = VertexTextureNormal;
	using CellVertexType = VertexColor; 
	// for Render
public:
	explicit LandScape(const LandScapeDesc & InDesc);
	~LandScape();
	void Tick();
	void Render(bool bDrawBoundary = false);
	const Vector & GetDimension() const { return Dimension; }
	const Texture * GetHeightMap() const { return HeightMap; }
private:
	void SetupShaders();
	void SetupResources(const LandScapeDesc& InDesc);
	void SetupCells();

	// Terrain의 실제 크기.
	Vector Dimension;
	UINT CellSize;
	UINT GridSize;
	
	vector<SceneryCell *> Cells;
	vector<Matrix> CellLocalTransform;
	
	VertexBuffer * CellBoxVBuffer;
	IndexBuffer * CellBoxIBuffer;
	InstanceBuffer * CellBoxInstBuffer;
	HlslShader<CellVertexType> * CellBoundaryRenderer = nullptr;
	
	HlslShader<VertexType> * CellRenderer = nullptr;
	HlslComputeShader * NormalMapGenerator = nullptr;

	Transform * Tf;
	WVPDesc MatrixData;	// VDHP
	DirectionalLightDesc LightData; // P
	LandScapeTessellationDesc TessellationData; // DH
	LandScapeBlendingDesc BlendingData; // DH

	ConstantBuffer * CB_WVP = nullptr; // 0
	ConstantBuffer * CB_Blending = nullptr; // 1
	ConstantBuffer * CB_Light = nullptr; // 2
	ConstantBuffer * CB_Tessellation = nullptr; // 3

	Texture * HeightMap;
	// RWTexture2D * NormalMap;
	Texture * PerlinNoise;
	Texture * VariationMap;
	TextureArray * DiffuseMaps;
	TextureArray * NormalMaps;
};

