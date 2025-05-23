#include "framework.h"

#include <complex>
#include <random>

#include "Ocean.h"

#pragma region Compute

void Ocean::SetupComputeShaders()
{
	constexpr UINT TextureThreadGroupSize = 32;
	const UINT TextureDispatchSize = TextureSize / TextureThreadGroupSize;
	const string TextureThreadGroupSizeStr = std::to_string(TextureThreadGroupSize);

	const string FFTSizeStr = to_string(TextureSize);
	const string FFTThreadGroupLengthStr = to_string(TextureSize / 2);
	const string FFTLogN = to_string(log2(TextureSize));

	const vector<D3D_SHADER_MACRO> TextureShaderMacros = {
		{"THREAD_X", TextureThreadGroupSizeStr.c_str()},
		{"THREAD_Y", TextureThreadGroupSizeStr.c_str()},
		{nullptr, }
	};

	const vector<D3D_SHADER_MACRO> FFTShaderMacros = {
		{"FFT_SIZE", FFTSizeStr.c_str()},
		{"THREAD_GROUP_SIZE", FFTThreadGroupLengthStr.c_str()},
		{"LOG_N", FFTLogN.c_str()},
		{nullptr, }
	};
	// Init Spectrum
	CS_SpectrumInitializer = new HlslComputeShader(
		L"Ocean/Compute/PhilipsSpectrum.Initialize.hlsl",
		TextureShaderMacros.data()
	);
	CS_SpectrumInitializer->SetDispatchSize(TextureDispatchSize, TextureDispatchSize , 1);

	// Update
	CS_SpectrumUpdater = new HlslComputeShader(
		L"Ocean/Compute/PhilipsSpectrum.Update.hlsl",
		TextureShaderMacros.data()
	);
	CS_SpectrumUpdater->SetDispatchSize(TextureDispatchSize, TextureDispatchSize , 1);

	// Transpose
	CS_Transpose = new HlslComputeShader(
		L"Ocean/Compute/TransposeTextureArray.hlsl",
		TextureShaderMacros.data()
	);
	CS_Transpose->SetDispatchSize(TextureDispatchSize, TextureDispatchSize , (UINT)SpectrumTextureType::MAX);

	// RowPass
	CS_RowPassIFFT = new HlslComputeShader(
		L"Ocean/Compute/WaveIFFT_RowPass.hlsl",
		FFTShaderMacros.data()
	);
	CS_RowPassIFFT->SetDispatchSize(TextureSize, 1 , (UINT)SpectrumTextureType::MAX);

	// ColPass
	CS_ColPassIFFT = new HlslComputeShader(
		L"Ocean/Compute/WaveIFFT_ColPass.hlsl",
		FFTShaderMacros.data()
	);
	CS_ColPassIFFT->SetDispatchSize(TextureSize, 1 , (UINT)SpectrumTextureType::MAX);

	CS_SimulateFoam = new HlslComputeShader(
		L"Ocean/Compute/WaveFoamSimulation.hlsl",
		TextureShaderMacros.data()
	);
	CS_SimulateFoam->SetDispatchSize(TextureDispatchSize, TextureDispatchSize , (UINT)SpectrumTextureType::MAX);
}

void Ocean::SetupComputeResources()
{
	PhillipsInitData.Width = static_cast<float>(TextureSize);
	PhillipsInitData.Height = static_cast<float>(TextureSize);

	PhilipsUpdateData.Width = static_cast<float>(TextureSize);
	PhilipsUpdateData.Height = static_cast<float>(TextureSize);
	PhilipsUpdateData.RunningTime = 0.f;
	PhilipsUpdateData.InitTime = sdt::SystemTimer::Get()->GetRunningTime();

	TransposeData.Width = (TextureSize);
	TransposeData.Height = (TextureSize);
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
}

void Ocean::GenerateInitialSpectrum() const
{
	GaussianRandomTexture2D->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); // SRV
	InitialSpectrumTexture2D->BindToGPUAsUAV(0); // UAV
	CB_PhillipsInit->BindToGPU();

	CS_SpectrumInitializer->Dispatch();
	InitialSpectrumTexture2D->UpdateSRV();
}

void Ocean::UpdateSpectrum() const
{
	InitialSpectrumTexture2D->BindToGPUAsSRV(0);
	SpectrumTexture2D->BindToGPUAsUAV(0);
	CB_PhillipsUpdate->BindToGPU();

	CS_SpectrumUpdater->Dispatch();
	SpectrumTexture2D->UpdateSRV();
}

void Ocean::GenerateDisplacementMap() const
{
	// Row IFFT
	CB_PhillipsInit->BindToGPU();
	SpectrumTexture2D->BindToGPUAsSRV(0);
	IFFT_Result->BindToGPUAsUAV(0);
	CS_RowPassIFFT->Dispatch();
	IFFT_Result->UpdateSRV();

	// Transpose
	IFFT_Result->BindToGPUAsSRV(0);
	IFFT_Result_Transposed->BindToGPUAsUAV(0);
	CB_Transpose->BindToGPU();
	CS_Transpose->Dispatch();
	IFFT_Result_Transposed->UpdateSRV();
	//
	// Col IFFT
	CB_PhillipsInit->BindToGPU();
	IFFT_Result_Transposed->BindToGPUAsSRV(0);
	DisplacementMap->BindToGPUAsUAV(0);
	CS_ColPassIFFT->Dispatch();
	DisplacementMap->UpdateSRV();
	
	// // Transpose
	// IFFT_Result->BindToGPUAsSRV(0);
	// IFFT_Result_Transposed->BindToGPUAsUAV(0);
	// CB_Transpose->BindToGPU();
	// CS_Transpose->Dispatch();
	// IFFT_Result_Transposed->UpdateSRV();
}

void Ocean::FoamSimulation() const
{
	if (CS_SimulateFoam == nullptr)
		return ;
	
	CB_Foam->BindToGPU();
	DisplacementMap->BindToGPUAsSRV(0);
	FoamGrid->BindToGPUAsUAV(0);
	CS_SimulateFoam->CreateSamplerState_Linear_Wrap();
	CS_SimulateFoam->Dispatch();
	FoamGrid->UpdateSRV();
}

#pragma endregion Compute
