#include "framework.h"
#include "Ocean.h"

Ocean::Ocean(const OceanDesc& Desc)
: PhillipsInitData()
, PhilipsUpdateData()
, TessellationData()
, Shader(nullptr)
{
	Dimension[0] = Desc.SeaDimension_X;
	Dimension[1] = Desc.SeaDimension_Z;
	TextureSize = Desc.FFTSize;
	PatchSize = Desc.PatchSize;
	Tf = new Transform();
	Tf->SetWorldPosition({Desc.WorldPosition.X, Desc.SeaLevel, Desc.WorldPosition.Y});
	SkyTexture = Desc.SkyTexture;
	TerrainHeightMap = Desc.TerrainHeightMap;
	PhillipsInitData.Wind = Desc.Wind;

#pragma region Compute
	SetupShaders();
	SetupResources();

	GaussianRandomTexture2D = Noise::CreateGaussian2DNoise(TextureSize);
	GenerateInitialSpectrum();
#pragma endregion Compute

#pragma region Render
	const vector<D3D_SHADER_MACRO> Macros = { {"TYPE01", ""}, {nullptr,}};
	Shader = new HlslShader<VertexType>(
		L"Ocean/Ocean.hlsl",
		static_cast<UINT>(ShaderType::VHDP),
		"VSMain",
		"PSMain",
		"",
		Macros.data()
	);
	Shader->SetTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	CHECK(SUCCEEDED(Shader->CreateSamplerState_Linear_Border(static_cast<UINT>(ShaderType::VDP))));
	CHECK(SUCCEEDED(Shader->CreateSamplerState_Anisotropic(static_cast<UINT>(ShaderType::VDP))));
	CHECK(SUCCEEDED(Shader->CreateBlendState_Multiply()));
	CHECK(SUCCEEDED(Shader->CreateDepthStencilState_Default()));
	CHECK(SUCCEEDED(Shader->CreateRasterizerState_Solid()));
	// CHECK(SUCCEEDED(Shader->CreateRasterizerState_WireFrame()));

	PerlinNoise = new Texture(L"PerlinNoise.png", true);
	// PerlinNoise = new Texture(L"Terrain/T_Perlin_Noise.png", true);
	
	TessellationData.TexelSize = {
		1.f / static_cast<float>(TextureSize),
		1.f / static_cast<float>(TextureSize)
	};
	if (!!DisplacementMap)
		TessellationData.HeightMapTiling =  Vector2D(Dimension[0], Dimension[1]) / static_cast<float>(DisplacementMap->GetWidth());

	if (!!PerlinNoise)
			TessellationData.NoiseTiling = TessellationData.HeightMapTiling / 16.f;
			// TessellationData.NoiseTiling =  Vector2D( (float)PatchSize / (float)PerlinNoise->GetWidth(), (float)PatchSize / (float)PerlinNoise->GetHeight()) ;
	if (Macros[0].Name == "TYPE03")
	{
		TessellationData.LODRange = {
			10,
			256
		};
		LODRange = {100, 1000};
	}
	else
	{
		TessellationData.LODRange = {
			1.f,
			2
		};
		LODRange = {1, 10};
	}
	TessellationData.HeightScaler = 4.f;
	TessellationData.NoiseScaler = 1.f;
	TessellationData.NoisePower = 1.f;
	CreateVertex();
	CreateIndex();
	
	VBuffer = new VertexBuffer(
		Vertices.data(),
		Vertices.size(),
		sizeof(VertexType)
	);
	IBuffer = new IndexBuffer(
		Indices.data(),
		Indices.size()
	);
	CB_WVP = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VHDP),
		0,
		nullptr,
		"World, View, Projection",
		sizeof(WVPDesc),
		false
	);
	CB_Tessellation = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VHDP),
		1,
		nullptr,
		"Height Scaler",
		sizeof(TessellationDesc),
		false
	);
	
#pragma endregion Render
}

Ocean::Ocean(const UINT Width, const UINT Height, const UINT InPatchSize)
: Ocean({Width,Height, InPatchSize})
{
}

Ocean::~Ocean()
{
#pragma region Compute
	// Resources
	SAFE_DELETE(GaussianRandomTexture2D);
	SAFE_DELETE(InitialSpectrumTexture2D);
	SAFE_DELETE(SpectrumTexture2D);
	SAFE_DELETE(IFFT_Result_Transposed);
	SAFE_DELETE(IFFT_Result);
	SAFE_DELETE(DisplacementMap);
	SAFE_DELETE(FoamGrid);
	SAFE_DELETE(PerlinNoise);
	SAFE_DELETE(CB_PhillipsInit);
	SAFE_DELETE(CB_PhillipsUpdate);
		
	// Shaders
	SAFE_DELETE(CS_SpectrumInitializer);
	SAFE_DELETE(CS_SpectrumUpdater);
	SAFE_DELETE(CS_RowPassIFFT);
	SAFE_DELETE(CS_Transpose);
	SAFE_DELETE(CS_ColPassIFFT)
#pragma endregion Compute

#pragma region Render
#pragma endregion Render
}

void Ocean::Tick()
{
#pragma region Compute
	PhilipsUpdateData.RunningTime = (sdt::SystemTimer::Get()->GetRunningTime() - PhilipsUpdateData.InitTime);
	CB_PhillipsUpdate->UpdateData(&PhilipsUpdateData, sizeof(PhilipsUpdateDesc));

	FoamData.Width = static_cast<float>(FoamGrid->GetWidth());
	FoamData.Height = static_cast<float>(FoamGrid->GetHeight());
	FoamData.DeltaTime = sdt::SystemTimer::Get()->GetDeltaTime();
	ImGui::SliderFloat("Ocean : Foam Sharpness", &FoamData.FoamSharpness, 0.1f, 5.f, "%.1f");
	ImGui::SliderFloat("Ocean : Foam Multiplier", &FoamData.FoamMultiplier, 0.1f, 5.f, "%.1f");
	ImGui::SliderFloat("Ocean : Foam Threshold", &FoamData.FoamThreshold, 0.95f, 1.01f, "%.4f");
	ImGui::SliderFloat("Ocean : Foam Blur", &FoamData.FoamBlur, 0, 50, "%.0f");
	ImGui::SliderFloat("Ocean : Foam Fade", &FoamData.FoamFade, 0, 1, "%.1f");
	CB_Foam->UpdateData(&FoamData, sizeof(FoamDesc));
	
	UpdateSpectrum();
	GenerateDisplacementMap();
	FoamSimulation();
#pragma endregion Compute

#pragma region Render
	WVP.World = Tf->GetMatrix();
	WVP.View = Context::Get()->GetViewMatrix();
	WVP.Projection = Context::Get()->GetProjectionMatrix();
	WVP.ViewInverse = Matrix::Invert(WVP.View, true);
	CB_WVP->UpdateData(&WVP, sizeof(WVPDesc));

	ImGui::SliderFloat("Ocean : Height Scaler", &TessellationData.HeightScaler, 0.f, 50.f, "%.1f");
	ImGui::SliderFloat("Ocean : LOD Power", &TessellationData.LODRange.X, 0.1f, 3.f, "%.1f");
	ImGui::SliderFloat("Ocean : Min Screen Diagonal", &TessellationData.LODRange.Y, LODRange.X, LODRange.Y, "%.0f");
	ImGui::SliderFloat("Ocean : Noise Scaler", &TessellationData.NoiseScaler, 0.1f, 10, "%.1f");
	ImGui::SliderFloat("Ocean : Noise Power", &TessellationData.NoisePower, 0.1f, 5, "%.1f");
	TessellationData.ScreenDistance = D3D::GetDesc().Height * 0.5f * Context::Get()->GetCamera()->GetProjectionMatrix().M22;
	TessellationData.ScreenDiagonal = D3D::GetDesc().Height * D3D::GetDesc().Height + D3D::GetDesc().Width * D3D::GetDesc().Width;

	TessellationData.CameraPosition = Context::Get()->GetCamera()->GetPosition();
	TessellationData.LightDirection = Context::Get()->GetLightDirection();
	TessellationData.LightColor = Context::Get()->GetLightColor();
	CB_Tessellation->UpdateData(&TessellationData, sizeof(TessellationData));
#pragma endregion Render
}

void Ocean::Render()
{
#pragma region Render
	if (!Shader) return;
	
	if (!!VBuffer) VBuffer->BindToGPU();
	if (!!IBuffer) IBuffer->BindToGPU();
	
	if (!!CB_WVP) CB_WVP->BindToGPU();
	if (!!CB_Tessellation) CB_Tessellation->BindToGPU();

	if (!!DisplacementMap)
		DisplacementMap->BindToGPUAsSRV(0, static_cast<UINT>(ShaderType::VDP));
	if (!!SkyTexture)
		SkyTexture->BindToGPU(1, static_cast<UINT>(ShaderType::PixelShader));
	if (!!FoamGrid)
		FoamGrid->BindToGPUAsSRV(2, static_cast<UINT>(ShaderType::PixelShader));
	if (!!PerlinNoise)
		PerlinNoise->BindToGPU(3, static_cast<UINT>(ShaderType::VDP));
	Shader->DrawIndexed(Indices.size());
#pragma endregion Render
}

void Ocean::SaveHeightMap()
{
	IFFT_Result->SaveOutputAsFile(L"OceanHeightMap");
}


void Ocean::CreateVertex()
{
	const UINT Width = Dimension[0];
	const UINT Height = Dimension[1];

	const UINT PatchWidth = (Dimension[0] / PatchSize) + 1;
	const UINT PatchHeight = (Dimension[1] / PatchSize) + 1;

	Vertices.clear();
	Vertices.resize(PatchWidth * PatchHeight);
	for (UINT Z = 0 ; Z < PatchHeight ; Z++)
	{
		for (UINT X = 0 ; X < PatchWidth ; X++)
		{
			const UINT Index = Z * PatchWidth + X;
			Vertices[Index].Position.X = static_cast<float>(X * PatchSize);
			Vertices[Index].Position.Y = 0;
			Vertices[Index].Position.Z = static_cast<float>(Z * PatchSize);
			Vertices[Index].Normal = {0, 1, 0};
			Vertices[Index].UV.X = (X * PatchSize) / static_cast<float>(Width);
			Vertices[Index].UV.Y = (Z * PatchSize) / static_cast<float>(Height);
		}
	}
}

void Ocean::CreateIndex()
{
	const UINT PatchWidth = (Dimension[0] / PatchSize) + 1;
	const UINT PatchHeight = (Dimension[1] / PatchSize) + 1;
	
	const UINT IndexCount = (PatchWidth - 1) * (PatchHeight - 1) * 4;

	Indices.clear();
	Indices.resize(IndexCount);
	UINT Index = 0;
	for (UINT Z = 0 ; Z < PatchHeight - 1 ; Z++)
	{
		for (UINT X = 0 ; X < PatchWidth  - 1; X++)
		{
			Indices[Index++] = PatchWidth * Z + X;
			Indices[Index++] = PatchWidth * (Z + 1) + X;
			Indices[Index++] = PatchWidth * (Z + 1) + (X + 1);
			Indices[Index++] = PatchWidth * Z + (X + 1);
		}
	}
}