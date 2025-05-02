#include "Pch.h"
#include "GaussianRandomDemo.h"

#include <complex>
#include <random>

void sdt::GaussianRandomDemo::Initialize()
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	
	std::default_random_engine generator(std::random_device{}());
	std::normal_distribution<float> distribution(0.0, 1.0);

	std::vector<std::complex<float>> GaussianRandomArray(Size * Size);
	for (int i = 0 ; i < Size * Size ; i++)
	{
		complex<float> & Element = GaussianRandomArray[i];
		float GaussianRandom = Math::Clamp(distribution(generator), -3, 3);
		GaussianRandom = (GaussianRandom + 3.f) / 6.f;
		float Magnitude = Math::Lerp(0.1f, 1.f, GaussianRandom);
		GaussianRandom = Math::Clamp(distribution(generator), -3, 3);
		GaussianRandom = (GaussianRandom + 3.f) / 6.f;
		float Angle = Math::Lerp(-Math::Pi, Math::Pi, GaussianRandom);
		Element.real(Magnitude * cosf(Angle));
		Element.imag(Magnitude * sinf(Angle));
	}

// Create Texture
	ID3D11Texture2D* GaussianRandomTexture = nullptr;

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

	D3D11_SUBRESOURCE_DATA initialTextureData {};
	UINT RowPitch = Size * 8;
	initialTextureData.pSysMem = GaussianRandomArray.data();
	initialTextureData.SysMemPitch = RowPitch;
	initialTextureData.SysMemSlicePitch = Size * RowPitch;

	HRESULT Hr = Device->CreateTexture2D(
		&GaussianTextureDesc,
		&initialTextureData,
		&GaussianRandomTexture
	);
	CHECK(SUCCEEDED(Hr));


	GaussianRandom = new Texture(GaussianRandomTexture, GaussianTextureDesc);
	
	Shader = new Hlsl2DTextureShader(GaussianRandom->GetSRV(), L"2D/GaussianRandomTexture.hlsl");
	float TextureHeight = 0.4f * D3D::GetDesc().Height;
	Shader->GetTransform()->SetScale({TextureHeight, TextureHeight, 1});
	Shader->GetTransform()->SetWorldPosition({ D3D::GetDesc().Width * 0.5f,D3D::GetDesc().Height * 0.5f, 0});

	Camera * const MainCamera = Context::Get()->GetCamera();
	MainCamera->SetPosition( 0, 10, -50 );
}

void sdt::GaussianRandomDemo::Destroy()
{
}

void sdt::GaussianRandomDemo::Tick()
{
	Shader->Tick();
}

void sdt::GaussianRandomDemo::Render()
{
	GaussianRandom->BindToGPU(0, (UINT)ShaderType::PixelShader);
	Shader->Render();
}
