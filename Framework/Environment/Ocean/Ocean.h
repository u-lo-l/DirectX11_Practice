#pragma once

namespace sdt
{
	class FoamDemo;
}

class Ocean
{
	friend class sdt::FoamDemo;
public:
	struct OceanDesc
	{
		Vector	Dimension; // x, z : Horizontal, y : Vertical
		UINT FFTSize;
		UINT CellSize;  // Should Be Power Of 2
		UINT GridSize;
		
		float SeaLevel;
		const Texture * SkyTexture;
		const Texture * TerrainHeightMap;
		Vector2D TerrainPosition;
		Vector2D TerrainDimension;
		float    TerrainMaxHeight;
		Vector2D Wind = {50.f, 30.f};
	};
private:
	using VertexType = VertexTextureNormal;
	using CellVertexType = VertexColor; 
	
	UINT TextureSize;

	struct PhillipsInitDesc
	{
		float Width;
		float Height;
		Vector2D Wind = {50.f, 30.f};
	};
	struct PhilipsUpdateDesc
	{
		float Width;
		float Height;
		float RunningTime = 0.f;
		float InitTime = 0.f;
	};
	struct TransposeDesc
	{
		UINT Width;
		UINT Height;
		UINT ArraySize = static_cast<UINT>(SpectrumTextureType::MAX);
		UINT Padding;
	};
	struct TessellationDesc
	{
		Vector CameraPosition;
		float HeightScaler = 1.f;

		Vector2D LODRange = {1, 3};
		Vector2D TexelSize;

		float ScreenDistance;
		float ScreenDiagonal;
		float NoiseScaler = 1.f;
		float NoisePower = 1.f;

		Vector2D HeightMapTiling;
		Vector2D NoiseTiling;

		Vector2D TerrainPosition;
		Vector2D TerrainDimension;
		
		float TerrainMaxHeight = 100;
		Vector Padding;
	};
	struct FoamDesc
	{
		float Width;
		float Height;
		float DeltaTime;
		float FoamSharpness = 1.f;

		float FoamMultiplier = 1.5f;
		float FoamThreshold = 1.f;
		float FoamBlur = 1.f;
		float FoamFade = 0.1f;
	};

public:
	explicit Ocean(const OceanDesc & Desc);
	~Ocean();

	void Tick();
	void Render(bool bDrawBoundary = false);
	
	void SaveHeightMap();

	void SetWorldPosition(const Vector & Position) const;
	// void SetWorldRotation(const Vector & RotationInDeg) const;
private:

#pragma region Compute
	void SetupComputeShaders();
	void SetupComputeResources();
	void GenerateInitialSpectrum() const;
	void UpdateSpectrum() const;
	void GenerateDisplacementMap() const;
	void FoamSimulation() const;
	// Resources
	PhillipsInitDesc PhillipsInitData;
	PhilipsUpdateDesc PhilipsUpdateData;
	TransposeDesc TransposeData;
	FoamDesc FoamData;
	
	Texture * GaussianRandomTexture2D = nullptr;

	RWTexture2D * InitialSpectrumTexture2D = nullptr;	// H_init

	enum class SpectrumTextureType
	{
		Height = 0,
		Disp_X,
		Disp_Y,
		MAX
	};
	enum class WaveTextureType
	{
		Displacement = 0,
		Normal,
		Foam,
		MAX
	};
	RWTexture2DArray * SpectrumTexture2D = nullptr;

	RWTexture2DArray * IFFT_Result = nullptr;
	RWTexture2DArray * IFFT_Result_Transposed = nullptr;
	RWTexture2D * DisplacementMap = nullptr;
	RWTexture2D * FoamGrid = nullptr;
	
	ConstantBuffer * CB_PhillipsInit = nullptr;
	ConstantBuffer * CB_PhillipsUpdate = nullptr;
	ConstantBuffer * CB_Transpose = nullptr;
	ConstantBuffer * CB_Foam = nullptr;
	// Shaders
	HlslComputeShader * CS_SpectrumInitializer = nullptr;
	HlslComputeShader * CS_SpectrumUpdater = nullptr;
	HlslComputeShader * CS_RowPassIFFT = nullptr;
	HlslComputeShader * CS_Transpose = nullptr;
	HlslComputeShader * CS_ColPassIFFT = nullptr;

	HlslComputeShader * CS_SimulateFoam= nullptr;
#pragma endregion Compute

#pragma region Render
private:
	void SetupRenderShaders();
	void SetupRenderResources();
	void SetupCells();

	Vector Dimension;
	UINT CellSize;
	UINT GridSize;
	vector<SceneryCell *> Cells;
	vector<Matrix> CellLocalTransform;
	
	VertexBuffer * CellBoxVBuffer;
	IndexBuffer * CellBoxIBuffer;
	InstanceBuffer * CellBoxInstBuffer;

	Transform * Tf;
	
	// Resources
	WVPIDesc MatrixData;
	DirectionalLightDesc LightData; // P
	TessellationDesc TessellationData;
	ConstantBuffer * CB_WVPI = nullptr;
	ConstantBuffer * CB_Light = nullptr;
	ConstantBuffer * CB_Tessellation= nullptr;

	const Texture * SkyTexture = nullptr;
	const Texture * TerrainHeightMap = nullptr;
	Texture * PerlinNoise = nullptr;
	// Shader
	HlslShader<VertexType> * OceanRenderer = nullptr;
	HlslShader<CellVertexType> * CellBoundaryRenderer = nullptr;
#pragma endregion Render
};


