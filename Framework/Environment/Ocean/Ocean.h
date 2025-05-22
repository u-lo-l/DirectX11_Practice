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
		UINT SeaDimension_X;
		UINT SeaDimension_Z;
		UINT FFTSize;
		UINT PatchSize;
		Vector2D WorldPosition;
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
	UINT TextureSize = 512;
	UINT Dimension[2];
	Vector2D LODRange;
	struct WVPDesc
	{
		Matrix World;
		Matrix View;
		Matrix Projection;
		Matrix ViewInverse;
	};
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

		Vector2D LODRange;
		Vector2D TexelSize;

		float ScreenDistance;
		float ScreenDiagonal;
		float NoiseScaler = 1.f;
		float NoisePower = 1.f;

		Vector2D HeightMapTiling;
		Vector2D NoiseTiling;

		Color LightColor;

		Vector LightDirection;
		float TerrainMaxHeight = 100;
		
		Vector2D TerrainPosition;
		Vector2D TerrainDimension;
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
	explicit Ocean(UINT Width = 512, UINT Height = 512, UINT InPatchSize= 32);
	~Ocean();

	void Tick();
	void Render();
	
	void SaveHeightMap();

	void SetWorldPosition(const Vector & Position) const { Tf->SetWorldPosition(Position); }
	void SetWorldRotation(const Vector & RotationInDeg) const { Tf->SetWorldRotation(RotationInDeg * Math::DegToRadian); }
private:

#pragma region Compute
	void SetupShaders();
	void SetupResources();
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
	void CreateVertex();
	void CreateIndex();
	
	UINT PatchSize = 1;
	vector<VertexType> Vertices;
	vector<UINT> Indices;
	
	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;

	Transform * Tf;
	
	// Resources
	WVPDesc WVP;
	TessellationDesc TessellationData;
	ConstantBuffer * CB_WVP = nullptr;
	ConstantBuffer * CB_Tessellation= nullptr;

	const Texture * SkyTexture = nullptr;
	const Texture * TerrainHeightMap = nullptr;
	Texture * PerlinNoise = nullptr;
	// Shader
	HlslShader<VertexType> * Shader;
#pragma endregion Render
};


