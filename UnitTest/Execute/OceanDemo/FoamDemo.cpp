#include "Pch.h"
#include "FoamDemo.h"

void sdt::FoamDemo::Initialize()
{
	Sky = new SkySphere(L"Environments/SkyDawn.dds", 0.5f);

	constexpr float SeaLevel = 4.f;
	const Vector SeaDimension = {512, 512, 512};
	const Ocean::OceanDesc OceanDesc{
		SeaDimension,
		512,
		256,
		16,
		SeaLevel,
		Sky->GetTexture(),
		nullptr,
		{0, 0},
		{0, 0},
		0,
		{-15, -20}
		// {30, 0}
	};
	Sea = new Ocean(OceanDesc);

	const vector<wstring> ShaderNames {
		L"2D/WaveSpectrum4.hlsl",		// Specturm
		
		L"2D/WaveSpectrum.hlsl",		// H
		L"2D/WaveSpectrum.hlsl",		// dH_x
		L"2D/WaveSpectrum.hlsl",		// dH_z
		
		L"2D/DisplacementMap_R.hlsl",	// Height Map
		L"2D/DisplacementMap_G.hlsl",	// dX Map
		L"2D/DisplacementMap_B.hlsl",	// dZ Map

		L"2D/DisplacementMap.hlsl",		// Displacement Map
		L"2D/HeightMap.hlsl",			// Foam
	};
	TextureDebugShaders.resize(ShaderNames.size());
	float SizeSmall = 0.25f * D3D::GetDesc().Height * 0.5f;
	float SizeGap = SizeSmall * 0.1f;
	const vector<float> Sizes{
		SizeSmall,
		SizeSmall, SizeSmall, SizeSmall,
		SizeSmall, SizeSmall, SizeSmall,
		SizeSmall, SizeSmall
	};
	const vector<Vector2D> Offsets {
		Vector2D(SizeSmall * -4.5f - SizeGap * 4.5f, +SizeSmall * 1.5f + SizeGap * 1.f),

		Vector2D(SizeSmall * -1.5f - SizeGap * 1.5f, +SizeSmall * 2.f + SizeGap * 2),
		Vector2D(SizeSmall * -1.5f - SizeGap * 1.5f, 0),
		Vector2D(SizeSmall * -1.5f - SizeGap * 1.5f, -SizeSmall * 2.f - SizeGap * 2),

		Vector2D(SizeSmall * +1.5f + SizeGap * 1.5f, +SizeSmall * 2.f + SizeGap * 2),
		Vector2D(SizeSmall * +1.5f + SizeGap * 1.5f, 0),
		Vector2D(SizeSmall * +1.5f + SizeGap * 1.5f, -SizeSmall * 2.f - SizeGap * 2),

		Vector2D(SizeSmall *  4.5f + SizeGap * 4.5f, -SizeSmall * 1.5f - SizeGap * 1.f),
		Vector2D(SizeSmall *  4.5f + SizeGap * 4.5f, +SizeSmall * 1.5f + SizeGap * 1.f),
	};
	const Vector2D Center = {D3D::GetDesc().Width * 0.5f, D3D::GetDesc().Height * 0.5f};
	
	for (UINT i = 0 ; i < ShaderNames.size() ; i++)
	{
		TextureDebugShaders[i] = new Hlsl2DTextureShader(nullptr, ShaderNames[i]);
		TextureDebugShaders[i]->GetTransform()->SetScale({Sizes[i], Sizes[i], 1});
		Vector2D Position = Center + Offsets[i];
		TextureDebugShaders[i]->GetTransform()->SetWorldPosition({ Position.X, Position.Y, 0});
	}
}

void sdt::FoamDemo::Destroy()
{
	SAFE_DELETE(Sky);
	SAFE_DELETE(Sea);
	for (auto TextureDebugShader : TextureDebugShaders)
		SAFE_DELETE(TextureDebugShader);
}

void sdt::FoamDemo::Tick()
{
	if (!!Sky)
		Sky->Tick();
	if (!!Sea)
		Sea->Tick();
	for (auto TextureDebugShader : TextureDebugShaders)
		TextureDebugShader->Tick();
}

void sdt::FoamDemo::Render()
{
	if (!!Sky)
		Sky->Render();
	if (!!Sea)
		Sea->Render();
	
	UINT TextureIndex = 0;
	
	ID3D11ShaderResourceView * SRV_Phillips = Sea->InitialSpectrumTexture2D->GetSRV();
	TextureDebugShaders[TextureIndex++]->Render(&SRV_Phillips);		// Philips Spectrum * Noise

	vector<ID3D11ShaderResourceView *> SRV_Spectrum;
	SRV_Spectrum.resize((UINT)Ocean::SpectrumTextureType::MAX);
	for (UINT i = 0 ; i < (UINT)Ocean::SpectrumTextureType::MAX ; i++)
	{
		Sea->SpectrumTexture2D->GetSRV(i, &SRV_Spectrum[i]);
		TextureDebugShaders[TextureIndex++]->Render(&SRV_Spectrum[i]);
	}
	for (auto SRV : SRV_Spectrum)
		SAFE_RELEASE(SRV);

	ID3D11ShaderResourceView * SRV_Displacement = Sea->DisplacementMap->GetSRV();
	TextureDebugShaders[TextureIndex++]->Render(&SRV_Displacement);
	TextureDebugShaders[TextureIndex++]->Render(&SRV_Displacement);
	TextureDebugShaders[TextureIndex++]->Render(&SRV_Displacement);
	TextureDebugShaders[TextureIndex++]->Render(&SRV_Displacement);

	ID3D11ShaderResourceView * SRV_FoamGrid = Sea->FoamGrid->GetSRV();
	TextureDebugShaders[TextureIndex++]->Render(&SRV_FoamGrid);
}
