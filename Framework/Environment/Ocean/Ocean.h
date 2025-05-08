#pragma once

class Ocean
{
private:
	using VertexType = VertexTextureNormal;
	struct WVPDesc
	{
		Matrix World;
		Matrix View;
		Matrix Projection;
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
		float HeightScaler = 10.f;
		
		Vector2D LODRange;
		Vector2D TexelSize;
		
		float ScreenDistance;
		float ScreenDiagonal;
		float Padding[2];
	};
	struct LightDirectionDesc
	{
		Vector Direction;
		float Padding;
	};
	UINT Size = 512;
	Vector2D LODRange;
public:

	Ocean(UINT InPatchSize = 4);
	~Ocean();

	void Tick();
	void Render();
	void SaveHeightMap();

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
	LightDirectionDesc LightDirectionData;
	ConstantBuffer * CB_WVP = nullptr;
	ConstantBuffer * CB_Tessellation= nullptr;
	ConstantBuffer * CB_LightDirection= nullptr;

	// Shader
	HlslShader<VertexType> * Shader;
#pragma endregion Render
};


