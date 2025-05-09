#pragma once

class Ocean
{
public:
	struct OceanDesc
	{
		UINT Dimension_X;
		UINT Dimension_Z;
		UINT PatchSize;
		Vector2D WorldPosition;
		float SeaLevel;
		const Texture * SkyTexture;
		const Texture * TerrainHeightMap;
		Vector2D TerrainPosition;
		Vector2D TerrainDimension;
		float    TerrainMaxHeight;
	};
private:
	using VertexType = VertexTextureNormal;
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
		Vector2D Wind = {1, 1};
	};
	struct PhilipsUpdateDesc
	{
		float Width;
		float Height;
		float RunningTime;
		float InitTime;
	};
	struct TessellationDesc
	{
		Vector CameraPosition;
		float HeightScaler = 100.f;
		
		Vector2D LODRange;
		Vector2D TexelSize;
		
		float ScreenDistance;
		float ScreenDiagonal;
		float Padding[2];
		
		Vector LightDirection;
		float TerrainMaxHeight = 100;

		Vector2D TerrainPosition;
		Vector2D TerrainDimension;
	};
	UINT Size = 512;
	UINT Dimension[2];
	Vector2D LODRange;
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
	void GenerateGaussianRandoms();
	void GenerateInitialSpectrum() const;
	void UpdateSpectrum() const;
	void GetHeightMap() const;
	// Resources
	PhillipsInitDesc PhillipsInitData;
	PhilipsUpdateDesc PhilipsUpdateData;
	
	Texture * GaussianRandomTexture2D = nullptr;

	RWTexture2D * InitialSpectrumTexture2D = nullptr;	// H_init
	RWTexture2D * SpectrumTexture2D = nullptr;			// H_t
	RWTexture2D * IFFT_Row = nullptr;					// H_t -> IFFT(x)
	RWTexture2D * IFFT_Row_Transposed = nullptr;		// H_t -> IFFT(x) -> Transposed
	RWTexture2D * HeightMapTexture2D = nullptr;

	ConstantBuffer * CB_PhillipsInit = nullptr;
	ConstantBuffer * CB_PhillipsUpdate = nullptr;
		
	// Shaders
	HlslComputeShader * CS_SpectrumInitializer = nullptr;
	HlslComputeShader * CS_SpectrumUpdater = nullptr;
	HlslComputeShader * CS_RowPassIFFT = nullptr;
	HlslComputeShader * CS_Transpose = nullptr;
	HlslComputeShader * CS_ColPassIFFT = nullptr;
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

	// Shader
	HlslShader<VertexType> * Shader;
#pragma endregion Render
};


