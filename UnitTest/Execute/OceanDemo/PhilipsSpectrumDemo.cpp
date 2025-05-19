#include "Pch.h"
#include "PhilipsSpectrumDemo.h"

#include <complex>
#include <random>

namespace sdt
{
	void PhilipsSpectrumDemo::Initialize()
	{
		SetupShaders();
		SetupResources();

		GenerateGaussianRandoms();
		GenerateInitialSpectrum();
		// UpdateSpectrum();
		// GetHeightMap();
		// HeighMapTexture2D->SaveOutputAsFile(L"HeightMap");

		const vector<wstring> EntryPoints {
			L"2D/GaussianRandomTexture.hlsl",
			L"2D/WaveSpectrum.hlsl",
			L"2D/WaveSpectrum.hlsl",
			L"2D/IFFTResult.hlsl",
			L"2D/HeightMap.hlsl",
		};
		float SizeSmall = 0.4f * D3D::GetDesc().Height / 2.5f;
		float SizeGap = SizeSmall * 0.2f;
		float SizeBig =  SizeSmall * 2 + SizeGap;
		const vector<float> Sizes{
			SizeSmall, SizeSmall, SizeSmall, SizeSmall, SizeBig
		};
		const vector<Vector2D> Offsets {
			Vector2D(-SizeSmall * 3 - SizeGap * 3, +SizeSmall + SizeGap),
			Vector2D(-SizeSmall - SizeGap, +SizeSmall + SizeGap),
			Vector2D(-SizeSmall * 3 - SizeGap * 3, -SizeSmall - SizeGap),
			Vector2D(-SizeSmall - SizeGap, -SizeSmall - SizeGap),
			Vector2D(SizeBig + SizeGap, 0)
		};
		const Vector2D Center = {D3D::GetDesc().Width * 0.5f, D3D::GetDesc().Height * 0.5f};
		for (int i = 0 ; i < 5 ; i++)
		{
			TextureDebugShader[i] = new Hlsl2DTextureShader(nullptr, EntryPoints[i]);
			TextureDebugShader[i]->GetTransform()->SetScale({Sizes[i], Sizes[i], 1});
			Vector2D Position = Center + Offsets[i];
			TextureDebugShader[i]->GetTransform()->SetWorldPosition({ Position.X, Position.Y, 0});
		}
	}

	void PhilipsSpectrumDemo::Destroy()
	{
		// Resources
		SAFE_DELETE(GaussianRandomTexture2D);
		SAFE_DELETE(InitialSpectrumTexture2D);
		SAFE_DELETE(SpectrumTexture2D);
		SAFE_DELETE(IFFT_Row);
		SAFE_DELETE(IFFT_Row_Transposed);
		SAFE_DELETE(HeighMapTexture2D);
		SAFE_DELETE(CB_PhillipsInit);
		SAFE_DELETE(CB_PhillipsUpdate);
		
		// Shaders
		SAFE_DELETE(CS_SpectrumInitializer);
		SAFE_DELETE(CS_SpectrumUpdater);
		SAFE_DELETE(CS_RowPassIFFT);
		SAFE_DELETE(CS_Transpose);
		SAFE_DELETE(CS_ColPassIFFT);

		SAFE_DELETE(TextureDebugShader[0]);
		SAFE_DELETE(TextureDebugShader[1]);
		SAFE_DELETE(TextureDebugShader[2]);
		SAFE_DELETE(TextureDebugShader[3]);
		SAFE_DELETE(TextureDebugShader[4]);
	}

	void PhilipsSpectrumDemo::Tick()
	{
		PhilipsUpdateData.RunningTime = (SystemTimer::Get()->GetRunningTime() - PhilipsUpdateData.InitTime) / 2.0f;
		CB_PhillipsUpdate->UpdateData(&PhilipsUpdateData, sizeof(PhilipsUpdateDesc));
		
		UpdateSpectrum();
		GetHeightMap();
		TextureDebugShader[0]->Tick();
		TextureDebugShader[1]->Tick();
		TextureDebugShader[2]->Tick();
		TextureDebugShader[3]->Tick();
		TextureDebugShader[4]->Tick();
	}

	void PhilipsSpectrumDemo::Render()
	{
		GaussianRandomTexture2D->BindToGPU(0);
		ID3D11ShaderResourceView * SRV_Gaussian = GaussianRandomTexture2D->GetSRV();
		ID3D11ShaderResourceView * SRV_H_init = InitialSpectrumTexture2D->GetSRV();
		ID3D11ShaderResourceView * SRV_H_t = SpectrumTexture2D->GetSRV();
		ID3D11ShaderResourceView * SRV_H_IFFT = IFFT_Row->GetSRV();
		ID3D11ShaderResourceView * SRV_Height = HeighMapTexture2D->GetSRV();
		
		TextureDebugShader[0]->Render(&SRV_Gaussian);
		TextureDebugShader[1]->Render(&SRV_H_init);
		TextureDebugShader[2]->Render(&SRV_H_t);
		TextureDebugShader[3]->Render(&SRV_H_IFFT);
		TextureDebugShader[4]->Render(&SRV_Height);
	}

	void PhilipsSpectrumDemo::SetupShaders()
	{
		vector<D3D_SHADER_MACRO> ShaderMacros = {
			{"THREAD_X", "16"},
			{"THREAD_Y", "16"},
			{nullptr, }
		};
		// Init Spectrum
		CS_SpectrumInitializer = new HlslComputeShader(
			L"Ocean/Compute/PhilipsSpectrum.Initialize.hlsl",
			ShaderMacros.data()
		);
		CS_SpectrumInitializer->SetDispatchSize(Size / 16, Size / 16 , 1);

		// Update
		CS_SpectrumUpdater = new HlslComputeShader(
			L"Ocean/Compute/PhilipsSpectrum.Update.hlsl",
			ShaderMacros.data()
		);
		CS_SpectrumUpdater->SetDispatchSize(Size / 16, Size / 16 , 1);

		// Transpose
		CS_Transpose = new HlslComputeShader(
			L"Ocean/Compute/TransposeTexture.hlsl",
			ShaderMacros.data()
		);
		CS_Transpose->SetDispatchSize(Size / 16, Size / 16 , 1);

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

	void PhilipsSpectrumDemo::SetupResources()
	{
		// IFFTSizeData.Width = Size;
		// IFFTSizeData.Height = Size;

		PhillipsInitData.Width = static_cast<float>(Size);
		PhillipsInitData.Height = static_cast<float>(Size);
		PhillipsInitData.Wind = {-6, 0};

		PhilipsUpdateData.Width = static_cast<float>(Size);
		PhilipsUpdateData.Height = static_cast<float>(Size);
		PhilipsUpdateData.RunningTime = 0.f;
		PhilipsUpdateData.InitTime = SystemTimer::Get()->GetRunningTime();
		
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
		HeighMapTexture2D = new RWTexture2D(
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

	void PhilipsSpectrumDemo::GenerateGaussianRandoms()
	{
		ID3D11Device * const Device = D3D::Get()->GetDevice();

		std::default_random_engine Generator(std::random_device{}());
		std::normal_distribution<float> Distribution_real(0.0f, 1.0f);
		std::normal_distribution<float> Distribution_imag(0.0f, 1.0f);

		std::vector<complex<float>> GaussianRandomArray(Size * Size);
		for (UINT i = 0 ; i < Size * Size ; i++)
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

	void PhilipsSpectrumDemo::GenerateInitialSpectrum() const
	{
		GaussianRandomTexture2D->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); // SRV
		InitialSpectrumTexture2D->BindToGPUAsUAV(0); // UAV
		CB_PhillipsInit->BindToGPU();
		
		CS_SpectrumInitializer->Dispatch();
		InitialSpectrumTexture2D->UpdateSRV();
	}

	void PhilipsSpectrumDemo::UpdateSpectrum() const
	{
		InitialSpectrumTexture2D->BindToGPUAsSRV(0);
		SpectrumTexture2D->BindToGPUAsUAV(0);
		CB_PhillipsUpdate->BindToGPU();

		CS_SpectrumUpdater->Dispatch();
		SpectrumTexture2D->UpdateSRV();
	}

	void PhilipsSpectrumDemo::GetHeightMap() const
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
		HeighMapTexture2D->BindToGPUAsUAV(0);
		CS_ColPassIFFT->Dispatch();
		HeighMapTexture2D->UpdateSRV();
	}
}
