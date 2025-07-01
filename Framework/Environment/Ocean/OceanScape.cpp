#include "framework.h"
#include "OceanScape.h"

OceanScape::OceanScape(const OceanScapeDesc & InDesc)
: Info(InDesc), Tf(new Transform())
{
	Tf->SetWorldPosition({0, Info.SeaLevel, 0});
#pragma region Compute
	SetupCSShaders();
	SetupCSResources();
#pragma endregion Compute

#pragma region Render
	SetupCells();
#pragma endregion Render
}

OceanScape::~OceanScape()
{
#pragma region Compute
	// Resources
	SAFE_DELETE(GaussianRandomTexture2D);
	SAFE_DELETE(InitialSpectrumTexture2D);
	SAFE_DELETE(SpectrumTexture2D);
	SAFE_DELETE(IFFT_Result_Transposed);
	SAFE_DELETE(IFFT_Result);
	SAFE_DELETE(DisplacementMap);
	SAFE_DELETE(FoamGrid);
	SAFE_DELETE(CB_PhillipsInit);
	SAFE_DELETE(CB_PhillipsUpdate);
		
	// SAFE_DELETE(PerlinNoise);
	// Shaders
	SAFE_DELETE(CS_SpectrumInitializer);
	SAFE_DELETE(CS_SpectrumUpdater);
	SAFE_DELETE(CS_RowPassIFFT);
	SAFE_DELETE(CS_Transpose);
	SAFE_DELETE(CS_ColPassIFFT)
#pragma endregion Compute

#pragma region Render
	SAFE_DELETE(Mat);
#pragma endregion Render
}

void OceanScape::Tick()
{
#pragma region Compute
	// PhilipsUpdateData.RunningTime = (sdt::SystemTimer::Get()->GetRunningTime() - PhilipsUpdateData.InitTime);
	PhilipsUpdateData.RunningTime += sdt::SystemTimer::Get()->GetDeltaTime() * 2.f;
	CB_PhillipsUpdate->UpdateData(&PhilipsUpdateData, sizeof(PhilipsUpdateDesc));

	ImGui::SliderFloat("OceanHeightScaler", &Info.Dimension.Y, 0.f, 5.f, "%.3f");
	CellInstance->SetHeightScaler(Info.Dimension.Y);

	FoamData.Width = static_cast<float>(FoamGrid->GetWidth());
	FoamData.Height = static_cast<float>(FoamGrid->GetHeight());
	FoamData.DeltaTime = sdt::SystemTimer::Get()->GetDeltaTime();
	ImGui::SliderFloat("Ocean : Foam Sharpness", &FoamData.FoamSharpness, 0.1f, 5.f, "%.1f");
	ImGui::SliderFloat("Ocean : Foam Multiplier", &FoamData.FoamMultiplier, 0.1f, 5.f, "%.1f");
	ImGui::SliderFloat("Ocean : Foam Threshold", &FoamData.FoamThreshold, 0.95f, 1.01f, "%.4f");
	ImGui::SliderFloat("Ocean : Foam Blur", &FoamData.FoamBlur, 0, 50, "%.0f");
	ImGui::SliderFloat("Ocean : Foam Fade", &FoamData.FoamFade, 0, 1, "%.1f");
	CB_Foam->UpdateData(&FoamData, sizeof(FoamDesc));

	// Update Spectrum
	InitialSpectrumTexture2D->BindToGPUAsSRV(0);
	SpectrumTexture2D->BindToGPUAsUAV(0);
	CB_PhillipsUpdate->BindToGPU(ShaderType::ComputeShader, 0);
	CS_SpectrumUpdater->Dispatch();

	// Get DisplacementMap
	// 	Row IFFT
	CB_PhillipsInit->BindToGPU(ShaderType::ComputeShader, 0);
	SpectrumTexture2D->BindToGPUAsSRV(0);
	IFFT_Result->BindToGPUAsUAV(0);
	CS_RowPassIFFT->Dispatch();
	// 	Transpose
	IFFT_Result->BindToGPUAsSRV(0);
	IFFT_Result_Transposed->BindToGPUAsUAV(0);
	CB_Transpose->BindToGPU();
	CS_Transpose->Dispatch();

	// 	Col IFFT
	CB_PhillipsInit->BindToGPU(ShaderType::ComputeShader, 0);
	IFFT_Result_Transposed->BindToGPUAsSRV(0);
	DisplacementMap->BindToGPUAsUAV(0);
	CS_ColPassIFFT->Dispatch();
	// DisplacementMap 완성
	
	// Simulate Foam
	if (!!CS_SimulateFoam)
	{
		CB_Foam->BindToGPU();
		DisplacementMap->BindToGPUAsSRV(0);
		FoamGrid->BindToGPUAsUAV(0);
		CS_SimulateFoam->Dispatch();
	}

	// GetNormalMap;
	{
		// CB_Transpose->BindToGPU(0);
		// DisplacementMap->BindToGPUAsSRV(0, ShaderType::ComputeShader);
		// NormalMap->BindToGPUAsUAV(0);
		// CS_NormalMapGenerator->Dispatch();
	}
#pragma endregion Compute

#pragma region Render
	// MatrixData.World = Tf->GetMatrix();
	// MatrixData.View = Context::Get()->GetViewMatrix();
	// MatrixData.Projection = Context::Get()->GetProjectionMatrix();
	// MatrixData.ViewInverse = Matrix::Invert(MatrixData.View, true);
	// CB_WVPI->UpdateData(&MatrixData, sizeof(WVPDesc));
	//
	// LightData.LightDirection = Context::Get()->GetLightDirection();
	// LightData.LightColor = Context::Get()->GetLightColor();
	// CB_Light->UpdateData(&LightData, sizeof(DirectionalLightDesc));
	//
	// ImGui::SliderFloat("Ocean : Height Scaler", &TessellationData.HeightScaler, 0.f, 50.f, "%.1f");
	// ImGui::SliderFloat("Ocean : LOD Power", &TessellationData.LODRange.X, 0.1f, 3.f, "%.1f");
	// ImGui::SliderFloat("Ocean : Min Screen Diagonal", &TessellationData.LODRange.Y, 1, 5, "%.0f");
	// ImGui::SliderFloat("Ocean : Noise Scaler", &TessellationData.NoiseScaler, 0.1f, 10, "%.1f");
	// ImGui::SliderFloat("Ocean : Noise Power", &TessellationData.NoisePower, 0.1f, 5, "%.1f");
	// TessellationData.ScreenDistance = D3D::GetDesc().WindowHeight * 0.5f * Context::Get()->GetCamera()->GetProjectionMatrix().M22;
	// TessellationData.ScreenDiagonal = D3D::GetDesc().WindowHeight * D3D::GetDesc().WindowHeight + D3D::GetDesc().WindowWidth * D3D::GetDesc().WindowWidth;
	//
	// TessellationData.CameraPosition = Context::Get()->GetCamera()->GetPosition();
	// CB_Tessellation->UpdateData(&TessellationData, sizeof(TessellationDesc));
#pragma endregion Render
	Mat->Tick();
}

void OceanScape::SetupCSShaders()
{
	constexpr UINT TextureThreadGroupSize = 32;
	const UINT TextureDispatchSize = Info.FFTData.Size / TextureThreadGroupSize;
	const string TextureThreadGroupSizeStr = std::to_string(TextureThreadGroupSize);
	const vector<D3D_SHADER_MACRO> TextureShaderMacros = {
		{"THREAD_X", TextureThreadGroupSizeStr.c_str()},
		{"THREAD_Y", TextureThreadGroupSizeStr.c_str()},
		{nullptr, }
	};
	ComputeShaderDesc TextureDesc {
		"TODO",
		L"TODO",
		L"",
		TextureShaderMacros.data(),
		L"CSMain",
		TextureThreadGroupSize,
		TextureThreadGroupSize,
		1,
		TextureDispatchSize,
		TextureDispatchSize,
		1,
		{},
		true
	};
	
	const string FFTSizeStr = to_string(Info.FFTData.Size);
	const UINT FFTDispatchSize = Info.FFTData.Size;
	const string FFTThreadGroupLengthStr = to_string(Info.FFTData.Size / 2);
	const string FFTLogN = to_string(log2(Info.FFTData.Size));
	const vector<D3D_SHADER_MACRO> FFTShaderMacros = {
		{"FFT_SIZE", FFTSizeStr.c_str()},
		{"THREAD_GROUP_SIZE", FFTThreadGroupLengthStr.c_str()},
		{"LOG_N", FFTLogN.c_str()},
		{nullptr, }
	};
	ComputeShaderDesc FFTDesc {
		"TODO",
		L"TODO",
		L"",
		FFTShaderMacros.data(),
		L"CSMain",
		Info.FFTData.Size / 2,1,1,
		FFTDispatchSize,1,3,
		{},
		true
	};
	
	{
		TextureDesc.ShaderName = "WaveSpectrum";
		TextureDesc.ShaderFileName = L"Ocean/Compute/PhilipsSpectrum.Initialize.hlsl";
		CS_SpectrumInitializer = new ComputeShader(TextureDesc);
	}
	{
		TextureDesc.ShaderName = "Update Spectrum";
		TextureDesc.ShaderFileName = L"Ocean/Compute/PhilipsSpectrum.Update.hlsl";
		CS_SpectrumUpdater = new ComputeShader(TextureDesc);
	}
	{
		TextureDesc.ShaderName = "Wave Foam";
		TextureDesc.ShaderFileName = L"Ocean/Compute/WaveFoamSimulation.hlsl";
		CS_SimulateFoam= new ComputeShader(TextureDesc);
	}
	{
		TextureDesc.ShaderName = "Transpose Texture";
		TextureDesc.ShaderFileName = L"Ocean/Compute/TransposeTextureArray.hlsl";
		TextureDesc.DispatchZ = 3;
		CS_Transpose = new ComputeShader(TextureDesc);
	}
	{
		// TextureDesc.ShaderName = "Normal Generator";
		// TextureDesc.ShaderFileName = L"Ocean/Compute/NormalMapGenerator.hlsl";
		// TextureDesc.DispatchZ = 1;
		// CS_NormalMapGenerator = new ComputeShader(TextureDesc);
	}
	{
		FFTDesc.ShaderName = "2D IFFT RowPass";
		FFTDesc.ShaderFileName = L"Ocean/Compute/WaveIFFT_RowPass.hlsl";
		CS_RowPassIFFT = new ComputeShader(FFTDesc);
	}
	{
		FFTDesc.ShaderName = "2D IFFT ColPass";
		FFTDesc.ShaderFileName = L"Ocean/Compute/WaveIFFT_ColPass.hlsl";
		CS_ColPassIFFT = new ComputeShader(FFTDesc);
	}
}

void OceanScape::SetupCSResources()
{
	const UINT TextureSize = Info.FFTData.Size;
	
	PhillipsInitData.Width = static_cast<float>(TextureSize);
	PhillipsInitData.Height = static_cast<float>(TextureSize);

	PhilipsUpdateData.Width = static_cast<float>(TextureSize);
	PhilipsUpdateData.Height = static_cast<float>(TextureSize);
	PhilipsUpdateData.RunningTime = 0.f;
	PhilipsUpdateData.InitTime = sdt::SystemTimer::Get()->GetRunningTime();

	TransposeData.Width = TextureSize;
	TransposeData.Height = TextureSize;
	TransposeData.ArraySize = (UINT)SpectrumTextureType::MAX;

	FoamData.Width = static_cast<float>(TextureSize);
	FoamData.Height = static_cast<float>(TextureSize);
	FoamData.FoamMultiplier = 0.9f;
	FoamData.FoamThreshold = 0.95f;
	FoamData.FoamBlur = 50;
	FoamData.FoamFade = 0.8f;
	
	InitialSpectrumTexture2D = new RWTexture2D(
		TextureSize, TextureSize,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	SpectrumTexture2D = new RWTexture2DArray(
		(UINT)SpectrumTextureType::MAX, TextureSize, TextureSize,
		DXGI_FORMAT_R32G32_FLOAT
	);
	IFFT_Result = new RWTexture2DArray(
		(UINT)SpectrumTextureType::MAX, TextureSize, TextureSize,
		DXGI_FORMAT_R32G32_FLOAT
	);
	IFFT_Result_Transposed = new RWTexture2DArray(
		(UINT)SpectrumTextureType::MAX, TextureSize, TextureSize,
		DXGI_FORMAT_R32G32_FLOAT
	);
	DisplacementMap = new RWTexture2D(
		TextureSize, TextureSize,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	NormalMap = new RWTexture2D(
		TextureSize, TextureSize,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	FoamGrid = new RWTexture2D(
		TextureSize, TextureSize,
		DXGI_FORMAT_R32G32_FLOAT
	);
	CB_PhillipsInit = new ConstantBuffer(
		ShaderType::ComputeShader,
		0,
		&PhillipsInitData,
		"PhillipsSpectrum Init",
		sizeof(PhillipsInitDesc),
		true
	);
	CB_PhillipsUpdate = new ConstantBuffer(
		ShaderType::ComputeShader,
		0,
		&PhilipsUpdateData,
		"PhillipsSpectrum Update",
		sizeof(PhilipsUpdateDesc),
		false
	);
	CB_Transpose = new ConstantBuffer(
		ShaderType::ComputeShader,
		0,
		&TransposeData,
		"IFFT Transpose",
		sizeof(TransposeData),
		true
	);
	CB_Foam = new ConstantBuffer(
		ShaderType::ComputeShader,
		0,
		nullptr,
		"IFFT Transpose",
		sizeof(FoamDesc),
		false
	);
	
	GaussianRandomTexture2D = Noise::CreateGaussian2DNoise(TextureSize);
	GaussianRandomTexture2D->BindToGPU(0, ShaderType::ComputeShader);
	InitialSpectrumTexture2D->BindToGPUAsUAV(0);
	CB_PhillipsInit->BindToGPU( ShaderType::ComputeShader, 0);
	CS_SpectrumInitializer->Dispatch();
}

void OceanScape::SetupCells()
{
	OceanMaterial::MaterialDesc MatDesc;
	MatDesc.DisplacementMapTiling = Info.Dimension.X / static_cast<float>(Info.CellSize); 
	this->Mat = new OceanMaterial(MatDesc);
	OceanCell::SceneryCellDesc Desc {
		"Ocean Cell",
		this->DisplacementMap,
		this->NormalMap,
		this->FoamGrid,
		Mat,
		Info.CellSize,
		Info.Dimension,
		static_cast<float>(Info.GridSize),
		this->Tf,
	};
	CellInstance = new OceanCell(Desc);
}