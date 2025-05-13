#include "Pch.h"
#include "FoamDemo.h"

void sdt::FoamDemo::Initialize()
{
	constexpr float SeaLevel = 4.f;
	const Vector2D SeaSize = {4096, 4096};
	const Ocean::OceanDesc OceanDesc{
		static_cast<UINT>(SeaSize.X),
		static_cast<UINT>(SeaSize.Y),
		32,
		{0, 0},
		SeaLevel,
		nullptr,
		nullptr,
		{0, 0},
		{0, 0},
		0,
		{100.f, 70.f}
	};
	Sea = new Ocean(OceanDesc);

	const vector<wstring> EntryPoints {
		L"2D/WaveSpectrum.hlsl",	// Specturm
		
		L"2D/IFFTResult.hlsl",		// IFFT x
		L"2D/IFFTResult.hlsl",		// IFFT y
		L"2D/IFFTResult.hlsl",		// IFFT z

		L"2D/IFFTResult.hlsl",		// IFFT Transposed x
		L"2D/IFFTResult.hlsl",		// IFFT Transposed y
		L"2D/IFFTResult.hlsl",		// IFFT Transposed z

		L"2D/DisplacementMap.hlsl",
	};
	TextureDebugShaders.resize(EntryPoints.size());
	float SizeSmall = 0.25f * D3D::GetDesc().Height * 0.5f;
	float SizeGap = SizeSmall * 0.1f;
	float SizeBig =  SizeSmall * 2.f + SizeGap;
	const vector<float> Sizes{
		SizeSmall,
		SizeSmall, SizeSmall, SizeSmall,
		SizeSmall, SizeSmall, SizeSmall,
		SizeBig
	};
	const vector<Vector2D> Offsets {
		Vector2D(-SizeSmall * 5 - SizeGap * 3, 0),

		Vector2D(-SizeSmall * 3 - SizeGap , 2 * SizeSmall + SizeGap),
		Vector2D(-SizeSmall * 3 - SizeGap , 0),
		Vector2D(-SizeSmall * 3 - SizeGap , -2 * SizeSmall - SizeGap),

		Vector2D(-SizeSmall * 0.5f, 2 * SizeSmall + SizeGap),
		Vector2D(-SizeSmall * 0.5f, 0),
		Vector2D(-SizeSmall * 0.5f, -2 * SizeSmall - SizeGap),
		
		Vector2D(SizeSmall + SizeGap + SizeBig, 0)
	};
	const Vector2D Center = {D3D::GetDesc().Width * 0.5f, D3D::GetDesc().Height * 0.5f};
	
	for (int i = 0 ; i < EntryPoints.size() ; i++)
	{
		TextureDebugShaders[i] = new Hlsl2DTextureShader(nullptr, EntryPoints[i]);
		TextureDebugShaders[i]->GetTransform()->SetScale({Sizes[i], Sizes[i], 1});
		Vector2D Position = Center + Offsets[i];
		TextureDebugShaders[i]->GetTransform()->SetWorldPosition({ Position.X, Position.Y, 0});
	}
}

void sdt::FoamDemo::Destroy()
{
	SAFE_DELETE(Sea);
	for (auto TextureDebugShader : TextureDebugShaders)
		SAFE_DELETE(TextureDebugShader);
}

void sdt::FoamDemo::Tick()
{
	// heightMap
	if (!!Sea)
		Sea->Tick();
	for (auto TextureDebugShader : TextureDebugShaders)
		TextureDebugShader->Tick();
}

void sdt::FoamDemo::Render()
{
	ID3D11ShaderResourceView * SRV_Spectrum = Sea->SpectrumTexture2D->GetSRV();

	Sea->IFFT_Row_Result->UpdateSRV();
	ID3D11ShaderResourceView * SRV_RowIFFTArray_X;
	Sea->IFFT_Row_Result->GetSRV(0, &SRV_RowIFFTArray_X);
	ID3D11ShaderResourceView * SRV_RowIFFTArray_Y;
	Sea->IFFT_Row_Result->GetSRV(1, &SRV_RowIFFTArray_Y);
	ID3D11ShaderResourceView * SRV_RowIFFTArray_Z;
	Sea->IFFT_Row_Result->GetSRV(2, &SRV_RowIFFTArray_Z);

	Sea->IFFT_Transpose_Result->UpdateSRV();
	ID3D11ShaderResourceView * SRV_RowIFFTTransposedArray_X;
	Sea->IFFT_Transpose_Result->GetSRV(0, &SRV_RowIFFTTransposedArray_X);
	ID3D11ShaderResourceView * SRV_RowIFFTTransposedArray_Y;
	Sea->IFFT_Transpose_Result->GetSRV(1, &SRV_RowIFFTTransposedArray_Y);
	ID3D11ShaderResourceView * SRV_RowIFFTTransposedArray_Z;
	Sea->IFFT_Transpose_Result->GetSRV(2, &SRV_RowIFFTTransposedArray_Z);
	ID3D11ShaderResourceView * SRV_Displacement = Sea->DisplacementMap2D->GetSRV();
		
	TextureDebugShaders[0]->Render(&SRV_Spectrum);
	TextureDebugShaders[1]->Render(&SRV_RowIFFTArray_X);
	TextureDebugShaders[2]->Render(&SRV_RowIFFTArray_Y);
	TextureDebugShaders[3]->Render(&SRV_RowIFFTArray_Z);
	TextureDebugShaders[4]->Render(&SRV_RowIFFTTransposedArray_X);
	TextureDebugShaders[5]->Render(&SRV_RowIFFTTransposedArray_Y);
	TextureDebugShaders[6]->Render(&SRV_RowIFFTTransposedArray_Z);
	TextureDebugShaders[7]->Render(&SRV_Displacement);
	// if (!!Sea)
		// Sea->Render();
	SAFE_RELEASE(SRV_RowIFFTArray_X);
	SAFE_RELEASE(SRV_RowIFFTArray_Y);
	SAFE_RELEASE(SRV_RowIFFTArray_Z);
	SAFE_RELEASE(SRV_RowIFFTTransposedArray_X);
	SAFE_RELEASE(SRV_RowIFFTTransposedArray_Y);
	SAFE_RELEASE(SRV_RowIFFTTransposedArray_Z);
}
