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
	CS_Transpose->SetDispatchSize(Size / 32, Size / 32 , 3);

	// RowPass
	ShaderMacros = {
		{"FFT_SIZE", "512"},
		{"THREAD_GROUP_SIZE", "256"},
		{"LOG_N", "9"},
		{"ROWPASS", "0"},
		{nullptr, }
	};
	CS_RowPassIFFT = new HlslComputeShader(
		// L"Ocean/Compute/IFFT2D.hlsl",
		L"Ocean/Compute/WaveDisplacement.hlsl",
		ShaderMacros.data()
	);
	CS_RowPassIFFT->SetDispatchSize(Size, 1 , 1);

	// ColPass
	ShaderMacros = {
		{"FFT_SIZE", "512"},
		{"THREAD_GROUP_SIZE", "256"},
		{"LOG_N", "9"},
		{"COLPASS", "0"},
		{nullptr, }
	};
	CS_ColPassIFFT = new HlslComputeShader(
		// L"Ocean/Compute/IFFT2D.hlsl",
		L"Ocean/Compute/WaveDisplacement.hlsl",
		ShaderMacros.data()
	);
	CS_ColPassIFFT->SetDispatchSize(Size, 1 , 1);
}

void Ocean::SetupResources()
{
	PhillipsInitData.Width = static_cast<float>(Size);
	PhillipsInitData.Height = static_cast<float>(Size);
	PhillipsInitData.Wind = {-50, -30};

	PhilipsUpdateData.Width = static_cast<float>(Size);
	PhilipsUpdateData.Height = static_cast<float>(Size);
	PhilipsUpdateData.RunningTime = 0.f;
	PhilipsUpdateData.InitTime = sdt::SystemTimer::Get()->GetRunningTime();
		
	InitialSpectrumTexture2D = new RWTexture2D(
		Size, Size,
		DXGI_FORMAT_R32G32_FLOAT
	);
	SpectrumTexture2D = new RWTexture2D(
		Size, Size,
		DXGI_FORMAT_R32G32_FLOAT
	);
		
	IFFT_Row_Result = new RWTexture2DArray(
		3, Size, Size,
		DXGI_FORMAT_R32G32_FLOAT
	);
	IFFT_Transpose_Result = new RWTexture2DArray(
		3, Size, Size,
		DXGI_FORMAT_R32G32_FLOAT
	);
	DisplacementMap2D = new RWTexture2D(
		Size, Size,
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
		true
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

void Ocean::GenerateHeightMap() const
{
	// Row IFFT
	CB_PhillipsInit->BindToGPU();
	SpectrumTexture2D->BindToGPUAsSRV(0);
	IFFT_Row_Result->BindToGPUAsUAV(0);
	CS_RowPassIFFT->Dispatch();
	IFFT_Row_Result->UpdateSRV();
		
	// Transpose
	IFFT_Row_Result->BindToGPUAsSRV(0);
	IFFT_Transpose_Result->BindToGPUAsUAV(0);
	CB_Transpose->BindToGPU();
	CS_Transpose->Dispatch();
	IFFT_Transpose_Result->UpdateSRV();

	// Col IFFT
	CB_PhillipsInit->BindToGPU();
	IFFT_Transpose_Result->BindToGPUAsSRV(0);
	DisplacementMap2D->BindToGPUAsUAV(0);
	CS_ColPassIFFT->Dispatch();
	DisplacementMap2D->UpdateSRV();
}


#pragma endregion Compute
