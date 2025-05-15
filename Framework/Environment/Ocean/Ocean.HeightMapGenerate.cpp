#include "framework.h"

#include <complex>
#include <random>

#include "Ocean.h"

#pragma region Compute

void Ocean::SetupShaders()
{
	vector<D3D_SHADER_MACRO> ShaderMacros = {
		{"THREAD_X", "32"},
		{"THREAD_Y", "32"},
		{nullptr, }
	};
	// Init Spectrum
	CS_SpectrumInitializer = new HlslComputeShader(
		L"Ocean/Compute/PhilipsSpectrum.Initialize.hlsl",
		ShaderMacros.data()
	);
	CS_SpectrumInitializer->SetDispatchSize(Size / 32, Size / 32 , 1);

	// Update
	CS_SpectrumUpdater = new HlslComputeShader(
		L"Ocean/Compute/PhilipsSpectrum.Update.hlsl",
		ShaderMacros.data()
	);
	CS_SpectrumUpdater->SetDispatchSize(Size / 32, Size / 32 , 1);

	// Transpose
	CS_Transpose = new HlslComputeShader(
		L"Ocean/Compute/TransposeTextureArray.hlsl",
		ShaderMacros.data()
	);
	CS_Transpose->SetDispatchSize(Size / 32, Size / 32 , (UINT)SpectrumTextureType::MAX);

	// RowPass
	ShaderMacros = {
		{"FFT_SIZE", "512"},
		{"THREAD_GROUP_SIZE", "256"},
		{"LOG_N", "9"},
		{nullptr, }
	};
	CS_RowPassIFFT = new HlslComputeShader(
		L"Ocean/Compute/WaveIFFT_RowPass.hlsl",
		ShaderMacros.data()
	);
	CS_RowPassIFFT->SetDispatchSize(Size, 1 , (UINT)SpectrumTextureType::MAX);

	// ColPass
	ShaderMacros = {
		{"FFT_SIZE", "512"},
		{"THREAD_GROUP_SIZE", "256"},
		{"LOG_N", "9"},
		{nullptr, }
	};
	CS_ColPassIFFT = new HlslComputeShader(
		L"Ocean/Compute/WaveIFFT_ColPass.hlsl",
		ShaderMacros.data()
	);
	CS_ColPassIFFT->SetDispatchSize(Size, 1 , (UINT)SpectrumTextureType::MAX);

	ShaderMacros = {
		{"THREAD_X", "32"},
		{"THREAD_Y", "32"},
		{nullptr, }
	};
	CS_SimulateFoam = new HlslComputeShader(
		L"Ocean/Compute/WaveFoamSimulation.hlsl",
		ShaderMacros.data()
	);
	CS_SimulateFoam->SetDispatchSize(Size / 32, Size / 32 , (UINT)SpectrumTextureType::MAX);
}

void Ocean::SetupResources()
{
	PhillipsInitData.Width = static_cast<float>(Size);
	PhillipsInitData.Height = static_cast<float>(Size);

	PhilipsUpdateData.Width = static_cast<float>(Size);
	PhilipsUpdateData.Height = static_cast<float>(Size);
	PhilipsUpdateData.RunningTime = 0.f;
	PhilipsUpdateData.InitTime = sdt::SystemTimer::Get()->GetRunningTime();

	FoamData.Width = static_cast<float>(Size);
	FoamData.Height = static_cast<float>(Size);
	FoamData.FoamMultiplier = 1.5f;
	FoamData.FoamThreshold = 1.5f;
	FoamData.FoamBlur = 1.f;
	FoamData.FoamFade = 0.1f;
	
	InitialSpectrumTexture2D = new RWTexture2D(
		Size, Size,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	SpectrumTexture2D = new RWTexture2DArray(
		(UINT)SpectrumTextureType::MAX, Size, Size,
		DXGI_FORMAT_R32G32_FLOAT
	);
	IFFT_Result = new RWTexture2DArray(
		(UINT)SpectrumTextureType::MAX, Size, Size,
		DXGI_FORMAT_R32G32_FLOAT
	);
	IFFT_Result_Transposed = new RWTexture2DArray(
		(UINT)SpectrumTextureType::MAX, Size, Size,
		DXGI_FORMAT_R32G32_FLOAT
	);
	DisplacementMap = new RWTexture2D(
		Size, Size,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	FoamGrid = new RWTexture2D(
		Size, Size,
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
	CS_SimulateFoam->Dispatch();
	FoamGrid->UpdateSRV();
}

#pragma endregion Compute
