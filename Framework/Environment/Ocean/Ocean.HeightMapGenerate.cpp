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
	CS_SpectrumUpdater->SetDispatchSize(Size / 32, (Size / 2) / 32 , 1);

	// Transpose
	CS_Transpose = new HlslComputeShader(
		L"Ocean/Compute/TransposeTexture.hlsl",
		ShaderMacros.data()
	);
	CS_Transpose->SetDispatchSize(Size / 32, Size / 32 , 1);

	// RowPass
	ShaderMacros = {
		{"FFT_SIZE", "512"},
		{"THREAD_GROUP_SIZE", "256"},
		{"LOG_N", "9"},
		{"ROWPASS", "0"},
		{nullptr, }
	};
	CS_RowPassIFFT = new HlslComputeShader(
		L"Ocean/Compute/IFFT2D.hlsl",
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
		L"Ocean/Compute/IFFT2D.hlsl",
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
		
	IFFT_Row = new RWTexture2D(
		Size, Size,
		DXGI_FORMAT_R32G32_FLOAT
	);
	IFFT_Row_Transposed = new RWTexture2D(
		Size, Size,
		DXGI_FORMAT_R32G32_FLOAT
	);
	HeightMapTexture2D = new RWTexture2D(
		Size, Size,
		DXGI_FORMAT_R32_FLOAT
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
}

// 배열의 중앙을 (0,0)이라 할 때 G(-k) = G*(k). 켤례 대칭을 만족하도록 수정.
void Ocean::GenerateGaussianRandoms()
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();

	std::default_random_engine Generator(std::random_device{}());
	std::normal_distribution<float> Distribution_real(0.0f, 1.0f);
	std::normal_distribution<float> Distribution_imag(0.0f, 1.0f);

	std::vector<complex<float>> GaussianRandomArray(Size * Size);
	for (int i = 0 ; i < Size * Size ; i++)
	{
		complex<float> & Element = GaussianRandomArray[i];
		Element.real(Math::Clamp(Distribution_real(Generator), -3, 3)); 
		Element.imag(Math::Clamp(Distribution_imag(Generator), -3, 3)); 
	}

	ID3D11Texture2D * GaussianRandomTexture = nullptr;
	D3D11_TEXTURE2D_DESC GaussianTextureDesc {0, };
	GaussianTextureDesc.Width = Size;
	GaussianTextureDesc.Height = Size;
	GaussianTextureDesc.MipLevels = 1;
	GaussianTextureDesc.ArraySize = 1;
	GaussianTextureDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	GaussianTextureDesc.SampleDesc.Count = 1;
	GaussianTextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	GaussianTextureDesc.CPUAccessFlags = 0;
	GaussianTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA InitialTextureData;
	const UINT RowPitch = Size * 8;
	InitialTextureData.pSysMem = GaussianRandomArray.data();
	InitialTextureData.SysMemPitch = RowPitch;
	InitialTextureData.SysMemSlicePitch = Size * RowPitch;

	const HRESULT Hr = Device->CreateTexture2D(
		&GaussianTextureDesc,
		&InitialTextureData,
		&GaussianRandomTexture
	);
	CHECK(SUCCEEDED(Hr));

	GaussianRandomTexture2D = new Texture(GaussianRandomTexture, GaussianTextureDesc);
	SAFE_RELEASE(GaussianRandomTexture);
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

void Ocean::GetHeightMap() const
{
	// Row IFFT
	SpectrumTexture2D->BindToGPUAsSRV(0);
	IFFT_Row->BindToGPUAsUAV(0);
	CS_RowPassIFFT->Dispatch();
	IFFT_Row->UpdateSRV();
		
	// Transpose
	IFFT_Row->BindToGPUAsSRV(0);
	IFFT_Row_Transposed->BindToGPUAsUAV(0);
	CS_Transpose->Dispatch();
	IFFT_Row_Transposed->UpdateSRV();

	// Col IFFT
	IFFT_Row_Transposed->BindToGPUAsSRV(0);
	HeightMapTexture2D->BindToGPUAsUAV(0);
	CS_ColPassIFFT->Dispatch();
	HeightMapTexture2D->UpdateSRV();
}


#pragma endregion Compute
