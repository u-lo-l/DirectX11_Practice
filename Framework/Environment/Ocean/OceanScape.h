#pragma once

/**
	* _cascadeSettings1:<br/>
		LengthScale: 600<br/>
		LowCutoff: 0<br/>
		HighCutoff: 1<br/>
		
	* _cascadeSettings2:<br/>
		LengthScale: 256<br/>
		LowCutoff: 1<br/>
		HighCutoff: 2<br/>
		
	* _cascadeSettings3:<br/>
		LengthScale: 50<br/>
		LowCutoff: 2<br/>
		HighCutoff: 9999<br/>
 */
class OceanCell;
class OceanScape
{
	struct FFTDesc
	{
		UINT Size = 256;
		Vector2D Wind = {50.f, 50.f};
	};
	struct CascadeDesc
	{
		float LengthScale = 1.f;
		Vector2D CutoffRange = {0.f, 9999.f};
		float Padding;
	};
public:
	struct OceanScapeDesc
	{
		Vector	Dimension; // x, z : Horizontal, y : Vertical
		UINT	CellSize;  // Should Be Power Of 2
		
		UINT	GridSize;
		float	SeaLevel;
		const	Texture * SkyTexture;
		const	Texture * TerrainHeightMap;
		
		Vector2D TerrainPosition;
		Vector2D TerrainDimension;

		float   TerrainMaxHeight;
		FFTDesc FFTData;
	};
	explicit OceanScape(OceanScapeDesc InDesc);
	~OceanScape();
	float GetHeightScaler() const { return Info.Dimension.Y; }
	void Tick();

private:
	void SetupCSShaders();
	void CreateInitialSpectrum();
	void SetupCSResources();
	void SetupCells(const OceanScapeDesc& InDesc);
	void SetInitialSpectrumResources();
	void SetSpectrumUpdateResources();
	void SetTransposeResources();
	void SetFoamResources();
	void UpdateSpectrum();
	void IFFT();
	void UpdateNormalMap();
	void UpdateFoam();

	void ReGeneratePopup();
	void ShaderRecompilePopup();
	OceanScapeDesc Info;
	Transform * Tf;
	OceanCell * CellInstance = nullptr;
	OceanMaterial * Mat;
	float TimeScaler = 1.f;
#pragma region Compute
	enum class SpectrumTextureType
	{
		Height = 0,
		DispX,
		DispZ,
		MAX
	};
	struct PhillipsInitDesc
	{
		float Width;
		float Height;
		Vector2D Wind = {50.f, 30.f};
		CascadeDesc CascadeData[3];
	} PhillipsInitData;
	struct PhilipsUpdateDesc
	{
		float Width;
		float Height;
		float RunningTime = 0.f;
		float InitTime = 0.f;
	} PhilipsUpdateData;
	struct TransposeDesc
	{
		float Width;
		float Height;
		float Padding1;
		float Padding2;
	} TransposeData;
	struct FoamDesc
	{
		float Width;
		float Height;
		float DeltaTime;
		float DisplacementMapTiling;
		
		float FoamThreshold = 1.f;
		float FoamMultiplier = 1.f;
		float HeightScaler;
		float FoamSharpness = 0.1f;
	} FoamData;
	
	Texture * GaussianRandomTexture2D = nullptr;
	RWTexture2DArray * InitialSpectrumTexture2D = nullptr;	// H_init
	RWTexture2DArray * SpectrumTexture2D = nullptr;		// H_t, DispX_t, DispZ_t 생성
	RWTexture2DArray * IFFT_Result = nullptr;
	RWTexture2DArray * IFFT_Result_Transposed = nullptr;
	// RWTexture2D * DisplacementMap = nullptr;
	// RWTexture2D * NormalMap = nullptr;
	// RWTexture2D * FoamGrid = nullptr;

	ConstantBuffer * CB_PhillipsInit = nullptr;
	ConstantBuffer * CB_PhillipsUpdate = nullptr;
	ConstantBuffer * CB_Transpose = nullptr;
	ConstantBuffer * CB_Foam = nullptr;
	
	/**
	 * Gaussian Noise를 바탕으로 PhilipsSpectrum 생성. -> H_Init Texture 생성
	 */
	ComputeShader * CS_SpectrumInitializer = nullptr;
	/**
	 * H_Init Texture를 바탕으로 DisplacementMap생성을 위한 각 축(X, Y, Z)에 대한 스펙트럼 생성<br/>
	 * DispY_t(H_t), DispX_t, DispZ_t 생성
	 */
	ComputeShader * CS_SpectrumUpdater = nullptr;
	/**
	 * ColPassFFT에 넘겨주기 위해 RowPassIFFT 거친 결과를 전치 
	 */
	ComputeShader * CS_TransposeTexArray = nullptr;
	/**
	 * DispY_t(H_t), DispX_t, DispZ_t를 Row방향으로 IFFT 수행
	 */
	ComputeShader * CS_RowPassIFFT = nullptr;
	/**
	 * 전치된 3개의 Texture를 IFFT하여 하나의 DisplacementMap 생성
	 */
	ComputeShader * CS_ColPassIFFT  = nullptr;
	/**
	 * 생성된 DisplacementMap을 미분하여 FoamTexture를 생성
	 */
	ComputeShader * CS_SimulateFoam = nullptr;
	ComputeShader * CS_NormalMapGenerator = nullptr;
	ComputeShader * CS_TransposeTexSelf = nullptr;

	unordered_map<string, ComputeShader ** > ComputeShaders = {
	{"SpectrumInitializer", &CS_SpectrumInitializer},
	{"SpectrumUpdater", &CS_SpectrumUpdater},
	{"TransposeTexArray", &CS_TransposeTexArray},
	{"RowPassIFFT", &CS_RowPassIFFT},
	{"ColPassIFFT", &CS_ColPassIFFT}, 
	{"SimulateFoam", &CS_SimulateFoam},
	{"NormalMapGenerator", &CS_NormalMapGenerator},
	{"TransposeTexSelf", &CS_TransposeTexSelf},
	};
#pragma endregion
};


