#include "Pch.h"
#include "GaussianRandomDemo.h"

#include <random>

void sdt::GaussianRandomDemo::Initialize()
{
	constexpr float PixelSize = 4.f;
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	
	std::default_random_engine generator(std::random_device{}());
	std::normal_distribution<float> distribution(0.0, 1.0);
	
	for (int i = 0 ; i < Size ; i++)
	{
		for (int j = 0 ; j < Size ; j++)
		{
			std::complex<float> & Element = GaussianRandomArray[i][j];
			float GaussianRandom = Math::Clamp(distribution(generator), -3, 3);
			GaussianRandom = (GaussianRandom + 3.f) / 6.f;
			float Magnitude = Math::Lerp(0.1f, 1.f, GaussianRandom);
			GaussianRandom = Math::Clamp(distribution(generator), -3, 3);
			GaussianRandom = (GaussianRandom + 3.f) / 6.f;
			float Angle = Math::Lerp(-Math::Pi, Math::Pi, GaussianRandom);
			Element.real(Magnitude * cosf(Angle));
			Element.imag(Magnitude * sinf(Angle));
		}
	}

//Create Output Texture
	ID3D11Texture2D* GaussianRandomTexture = nullptr;
	
	D3D11_TEXTURE2D_DESC GaussianTextureDesc;
	ZeroMemory(&GaussianTextureDesc, sizeof(GaussianTextureDesc));
	GaussianTextureDesc.Width = Size;
	GaussianTextureDesc.Height = Size;
	GaussianTextureDesc.MipLevels = 1;
	GaussianTextureDesc.ArraySize = 1;
	GaussianTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	GaussianTextureDesc.SampleDesc.Count = 1;
	GaussianTextureDesc.Usage = D3D11_USAGE_STAGING;
	GaussianTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	
	vector<BYTE> GaussianTextureData(Size * Size * PixelSize);
	UINT RowPitch = Size * PixelSize;
	
#pragma region SET_INITIAL_DATA 
	for (int i = 0 ; i < Size ; i++)
	{
		for (int j = 0 ; j < Size ; j++)
		{
			const complex<float> & c = GaussianRandomArray[i][j];
			float Mag = std::abs(c);
			float Angle = std::arg(c);

			float normalizedMagnitude = Mag / 2.0f;
			float positiveAngle = Angle + Math::Pi;
			float normalizedAngle = positiveAngle / (2.0f * Math::Pi);

			float r, g, b;
			float h = normalizedAngle;
			float s = 1.0f;
			float v = normalizedMagnitude;

			if (s == 0.0f)
				r = g = b = v;
			else
			{
				int x = (int)(h * 6);
				float f = h * 6 - x;
				float p = v * (1 - s);
				float q = v * (1 - f * s);
				float t = v * (1 - (1 - f) * s);
				switch (x % 6)
				{
					case 0: r = v; g = t; b = p; break;
					case 1: r = q; g = v; b = p; break;
					case 2: r = p; g = v; b = t; break;
					case 3: r = p; g = q; b = v; break;
					case 4: r = t; g = p; b = v; break;
					case 5: r = v; g = p; b = q; break;
				}
			}
			UINT Offset = i * RowPitch + j * PixelSize;
			GaussianTextureData[Offset + 0] = static_cast<BYTE>(r * 255.0f);
			GaussianTextureData[Offset + 1] = static_cast<BYTE>(g * 255.0f);
			GaussianTextureData[Offset + 2] = static_cast<BYTE>(b * 255.0f);
			GaussianTextureData[Offset + 3] = 255;
		}
	}
#pragma endregion SET_INITIAL_DATA

// Create Texture
	D3D11_SUBRESOURCE_DATA initialTextureData;
	initialTextureData.pSysMem = GaussianTextureData.data();
	initialTextureData.SysMemPitch = RowPitch;
	initialTextureData.SysMemSlicePitch = Size * RowPitch;
	
	HRESULT Hr = Device->CreateTexture2D(
		&GaussianTextureDesc,
		&initialTextureData,
		&GaussianRandomTexture
	);
	CHECK(SUCCEEDED(Hr));

// Create Image
	

	DirectX::ScratchImage GaussainComplexImage;
	Hr = DirectX::CaptureTexture(Device, DeviceContext, GaussianRandomTexture, GaussainComplexImage);
	CHECK(SUCCEEDED(Hr));

	Hr = DirectX::SaveToWICFile(
		*(GaussainComplexImage.GetImage(0,0,0)),
		DirectX::WIC_FLAGS_NONE,
		DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG),
		(wstring(W_TEXTURE_PATH) + L"Gaussian Random Complex HSV" + L".png").c_str()
	);
	CHECK(SUCCEEDED(Hr));
}

void sdt::GaussianRandomDemo::Destroy()
{
}

void sdt::GaussianRandomDemo::Tick()
{
}

void sdt::GaussianRandomDemo::Render()
{
}
