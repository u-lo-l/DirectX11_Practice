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
		Vector2D Wind = {50.f, 30.f};
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
		Vector2D Wind = {50.f, 30.f};
	};
	struct PhilipsUpdateDesc
	{
		float Width;
		float Height;
		float RunningTime;
		float InitTime;
	};
	struct TransposeDesc
	{
		UINT Width = 1024;
		UINT Height = 1024;
		UINT ArraySize = 3;
		UINT Padding;
	};
	struct TessellationDesc
	{
		Vector CameraPosition;
		float HeightScaler = 60.f;
		
		Vector2D LODRange;
		Vector2D TexelSize;
		
		float ScreenDistance;
		float ScreenDiagonal;
		float Padding[2];
		
		Color LightColor;

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
	const RWTexture2D * GetDisplacementMap() const { return DisplacementMap2D; }
	
	void SaveHeightMap();

	void SetWorldPosition(const Vector & Position) const { Tf->SetWorldPosition(Position); }
	void SetWorldRotation(const Vector & RotationInDeg) const { Tf->SetWorldRotation(RotationInDeg * Math::DegToRadian); }
private:

#pragma region Compute
	void SetupShaders();
	void SetupResources();
	void GenerateInitialSpectrum() const;
	void UpdateSpectrum() const;
	void GenerateHeightMap() const;
	// Resources
	PhillipsInitDesc PhillipsInitData;
	PhilipsUpdateDesc PhilipsUpdateData;
	TransposeDesc TransposeData;
	
	Texture * GaussianRandomTexture2D = nullptr;

	RWTexture2D * InitialSpectrumTexture2D = nullptr;	// H_init
	RWTexture2D * SpectrumTexture2D = nullptr;			// H_t

	RWTexture2DArray * IFFT_Row_Result = nullptr;
	RWTexture2DArray * IFFT_Transpose_Result = nullptr;
	RWTexture2D * DisplacementMap2D = nullptr;

	ConstantBuffer * CB_PhillipsInit = nullptr;
	ConstantBuffer * CB_PhillipsUpdate = nullptr;
	ConstantBuffer * CB_Transpose = nullptr;
		
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


