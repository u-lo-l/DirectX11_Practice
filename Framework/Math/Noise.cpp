#include "framework.h"
#include "Noise.h"

#include <complex>
#include <random>

Texture * Noise::CreateGaussian2DNoise(UINT InSize)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();

	std::default_random_engine Generator(std::random_device{}());
	std::normal_distribution<float> Distribution_real(0.0f, 1.0f);
	std::normal_distribution<float> Distribution_imag(0.0f, 1.0f);

	std::vector<complex<float>> GaussianRandomArray(InSize * InSize);
	for (UINT i = 0 ; i < InSize * InSize ; i++)
	{
		complex<float> & Element = GaussianRandomArray[i];
		Element.real(Math::Clamp(Distribution_real(Generator), -3, 3)); 
		Element.imag(Math::Clamp(Distribution_imag(Generator), -3, 3)); 
	}

	ID3D11Texture2D * GaussianRandomTexture = nullptr;
	D3D11_TEXTURE2D_DESC GaussianTextureDesc {0, };
	GaussianTextureDesc.Width = InSize;
	GaussianTextureDesc.Height = InSize;
	GaussianTextureDesc.MipLevels = 1;
	GaussianTextureDesc.ArraySize = 1;
	GaussianTextureDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	GaussianTextureDesc.SampleDesc.Count = 1;
	GaussianTextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	GaussianTextureDesc.CPUAccessFlags = 0;
	GaussianTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA InitialTextureData;
	const UINT RowPitch = InSize * 8;
	InitialTextureData.pSysMem = GaussianRandomArray.data();
	InitialTextureData.SysMemPitch = RowPitch;
	InitialTextureData.SysMemSlicePitch = InSize * RowPitch;

	const HRESULT Hr = Device->CreateTexture2D(
		&GaussianTextureDesc,
		&InitialTextureData,
		&GaussianRandomTexture
	);
	CHECK(SUCCEEDED(Hr));

	Texture * Result = new Texture(GaussianRandomTexture, GaussianTextureDesc);
	SAFE_RELEASE(GaussianRandomTexture);
	return Result;
}

Texture* Noise::CreatePerlin2DNoise(UINT InSize)
{
	RWTexture2D * PerlinNoiseMap = new RWTexture2D(
		InSize,
		InSize,
		DXGI_FORMAT_R32_FLOAT
	);
	const vector<D3D_SHADER_MACRO> ShaderMacros = {
		{"THREAD_X", "32"},
		{"THREAD_Y", "32"},
		{nullptr, }
	};
	HlslComputeShader * CS_PerlinNoiseMap = new HlslComputeShader(
		L"Terrain/Compute/ComputePerlinNoise.hlsl",
		ShaderMacros.data()
	);
	CS_PerlinNoiseMap->SetDispatchSize(
		PerlinNoiseMap->GetWidth() / 32,
		PerlinNoiseMap->GetHeight() / 32,
		1
	);
	PerlinNoiseMap->BindToGPUAsUAV(0);
	CS_PerlinNoiseMap->Dispatch();
	PerlinNoiseMap->UpdateSRV();
	
	// PerlinNoiseMap->SaveOutputAsFile(L"PerlinNoise");
	
	SAFE_DELETE(CS_PerlinNoiseMap);
	D3D11_TEXTURE2D_DESC Desc = {};
	Desc.Width = InSize;
	Desc.Height = InSize;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_R32_FLOAT;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Texture * Result = new Texture(PerlinNoiseMap->GetSRV(), Desc);
	SAFE_DELETE(PerlinNoiseMap);
	return Result;
}
