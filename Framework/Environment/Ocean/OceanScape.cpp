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
	SetupCells(InDesc);
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
	SAFE_DELETE(CB_PhillipsInit);
	SAFE_DELETE(CB_PhillipsUpdate);
		
	// SAFE_DELETE(PerlinNoise);
	// Shaders
	SAFE_DELETE(CS_SpectrumInitializer);
	SAFE_DELETE(CS_SpectrumUpdater);
	SAFE_DELETE(CS_RowPassIFFT);
	SAFE_DELETE(CS_Transpose);
	SAFE_DELETE(CS_ColPassIFFT)
	SAFE_DELETE(CS_NormalMapGenerator)
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

	FoamData.Width = static_cast<float>(Mat->GetDisplacementMap()->GetWidth());
	FoamData.Height = static_cast<float>(Mat->GetDisplacementMap()->GetHeight());
	FoamData.DeltaTime = sdt::SystemTimer::Get()->GetDeltaTime();
	ImGui::SliderFloat("Ocean : Foam Sharpness", &FoamData.FoamSharpness, 0.1f, 5.f, "%.1f");
	ImGui::SliderFloat("Ocean : Foam Multiplier", &FoamData.FoamMultiplier, 0.1f, 5.f, "%.1f");
	ImGui::SliderFloat("Ocean : Foam Threshold", &FoamData.FoamThreshold, 0.95f, 1.01f, "%.4f");
	ImGui::SliderFloat("Ocean : Foam Blur", &FoamData.FoamBlur, 0, 50, "%.0f");
	ImGui::SliderFloat("Ocean : Foam Fade", &FoamData.FoamFade, 0, 1, "%.1f");
	CB_Foam->UpdateData(&FoamData, sizeof(FoamDesc));

	// Update Spectrum
	CS_SpectrumUpdater->BindCB(CB_PhillipsUpdate, 0);
	CS_SpectrumUpdater->BindSRV(InitialSpectrumTexture2D->GetSRV(), 0);
	CS_SpectrumUpdater->BindUAV(SpectrumTexture2D->GetUAV(), 0);
	CS_SpectrumUpdater->Dispatch();

	// Get DisplacementMap
	// 	Row IFFT
	CS_RowPassIFFT->BindCB(CB_PhillipsInit, 0);
	CS_RowPassIFFT->BindSRV(SpectrumTexture2D->GetSRV(), 0);
	CS_RowPassIFFT->BindUAV(IFFT_Result->GetUAV(), 0);
	CS_RowPassIFFT->Dispatch();
	// 	Transpose
	CS_Transpose->BindCB(CB_Transpose, 0);
	CS_Transpose->BindSRV(IFFT_Result->GetSRV(), 0);
	CS_Transpose->BindUAV(IFFT_Result_Transposed->GetUAV(), 0);
	CS_Transpose->Dispatch();

	// 	Col IFFT
	CS_ColPassIFFT->BindCB(CB_PhillipsInit, 0);
	CS_ColPassIFFT->BindSRV(IFFT_Result_Transposed->GetSRV(), 0);
	CS_ColPassIFFT->BindUAV(Mat->GetDisplacementMap()->GetUAV(), 0);
	CS_ColPassIFFT->Dispatch();
	// DisplacementMap 완성

	// Simulate Foam
	if (!!CS_SimulateFoam)
	{
		CS_SimulateFoam->BindCB(CB_Foam, 0);
		CS_SimulateFoam->BindSRV(Mat->GetDisplacementMap()->GetSRV(), 0);
		CS_SimulateFoam->BindUAV(Mat->GetFoamGridMap()->GetUAV(), 0);
		CS_SimulateFoam->Dispatch();
	}
	
	// GenerateNormalMap
	if (!!CS_NormalMapGenerator)
	{
		TransposeData.Padding[0] = this->Info.Dimension.Y;
		TransposeData.Padding[1] = this->Mat->GetDisplacementMapTiling();
		CB_Transpose->UpdateData(&TransposeData, sizeof(TransposeDesc));
		CS_NormalMapGenerator->BindCB(CB_Transpose, 0);
		CS_NormalMapGenerator->BindSRV(Mat->GetDisplacementMap()->GetSRV(), 0);
		CS_NormalMapGenerator->BindSRV(Mat->GetFoamGridMap()->GetSRV(), 1);
		CS_NormalMapGenerator->BindUAV(Mat->GetNormalMap()->GetUAV(), 0);
		CS_NormalMapGenerator->Dispatch();
		// NormalMap->SaveOutputAsFile(L"Debug/Normal");
	}

#pragma endregion Compute

#pragma region Render
	Mat->Tick();
#pragma endregion Render
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
		TextureDesc.SamplerStateNames.clear();
		CS_SimulateFoam= new ComputeShader(TextureDesc);
	}
	{
		TextureDesc.ShaderName = "Normal Generator";
		TextureDesc.ShaderFileName = L"Ocean/Compute/NormalMapGenerator.hlsl";
		CS_NormalMapGenerator= new ComputeShader(TextureDesc);
	}
	{
		TextureDesc.ShaderName = "Transpose Texture";
		TextureDesc.ShaderFileName = L"Ocean/Compute/TransposeTextureArray.hlsl";
		TextureDesc.DispatchZ = 2;
		TextureDesc.SamplerStateNames.clear();
		CS_Transpose = new ComputeShader(TextureDesc);
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

	TransposeData.Width  = static_cast<float>(TextureSize);
	TransposeData.Height = static_cast<float>(TextureSize);
	
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
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	IFFT_Result = new RWTexture2DArray(
		(UINT)SpectrumTextureType::MAX, TextureSize, TextureSize,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	IFFT_Result_Transposed = new RWTexture2DArray(
		(UINT)SpectrumTextureType::MAX, TextureSize, TextureSize,
		DXGI_FORMAT_R32G32B32A32_FLOAT
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
		false
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
	CS_SpectrumInitializer->BindCB(CB_PhillipsInit, 0);
	CS_SpectrumInitializer->BindSRV(GaussianRandomTexture2D->GetSRV(), 0);
	CS_SpectrumInitializer->BindUAV(InitialSpectrumTexture2D->GetUAV(), 0);
	CS_SpectrumInitializer->Dispatch();
}

void OceanScape::SetupCells(const OceanScapeDesc & InDesc)
{
	OceanMaterial::MaterialDesc MatDesc;
	MatDesc.DisplacementMapTiling = InDesc.Dimension.X / static_cast<float>(InDesc.CellSize);
	MatDesc.DisplacementMapSize = InDesc.FFTData.Size;
	MatDesc.NoiseTiling = 1.f; // TODO
	MatDesc.Name = "Ocean Material";
	MatDesc.ShaderName = "Ocean";
	this->Mat = new OceanMaterial(MatDesc);
	OceanCell::SceneryCellDesc CellDesc {
		"Ocean Cell",
		Mat,
		InDesc.CellSize,
		InDesc.Dimension,
		static_cast<float>(InDesc.GridSize),
		this->Tf,
	};
	CellInstance = new OceanCell(CellDesc);
}