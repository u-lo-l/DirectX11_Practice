#include "framework.h"
#include "LandScape.h"

LandScape::LandScape(const LandScapeDesc& Desc)
{
	ASSERT(Desc.DiffuseMapNames.size() == Desc.NormalMapNames.size(), "DetailMaps Not Valid");

	PatchSize = Desc.PatchSize;
	TerrainTessData.HeightScaler = Desc.HeightScaler;
	CreatePerlinNoiseMap();
	SetupShaders();
	SetupResources(Desc);

	Context::Get()->GetCamera()->SetPosition(-650, 1830, -132);
	Context::Get()->GetCamera()->SetRotation(24, 50, -0);
}

LandScape::LandScape(const wstring & InHeightMapFilename, const UINT PatchSize)
	: LandScape({100, PatchSize, InHeightMapFilename, L"", {}, {}})
{
}

LandScape::~LandScape()
{
	SAFE_DELETE(Shader);
	SAFE_DELETE(HeightMap);
	SAFE_DELETE(BumpMap);
	SAFE_DELETE(VariationMap);
	SAFE_DELETE(DetailDiffuseMaps);
	SAFE_DELETE(DetailNormalMaps);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(Tf)
	SAFE_DELETE(CB_WVP);
	SAFE_DELETE(CB_TerrainData);
}

void LandScape::Tick()
{
	WVP.World = Tf->GetMatrix();
	WVP.View = Context::Get()->GetViewMatrix();
	WVP.Projection = Context::Get()->GetProjectionMatrix();
	CB_WVP->UpdateData(&WVP, sizeof(WVPDesc));

	ImGui::SliderFloat("LandScape : LOD Power", &TerrainTessData.LODRange.X, 0.1f, 3.f, "%.1f");
	ImGui::SliderFloat("LandScape : Min Screen Diagonal", &TerrainTessData.LODRange.Y, LODRange.X, LODRange.Y, "%.0f");
	TerrainTessData.ScreenDistance = D3D::GetDesc().Height * 0.5f * Context::Get()->GetCamera()->GetProjectionMatrix().M22;
	TerrainTessData.ScreenDiagonal = D3D::GetDesc().Height * D3D::GetDesc().Height + D3D::GetDesc().Width * D3D::GetDesc().Width;
	TerrainTessData.CameraWorldPosition = Context::Get()->GetCamera()->GetPosition();
	TerrainTessData.LightDirection = Context::Get()->GetLightDirection();
	TerrainTessData.LightColor = Context::Get()->GetLightColor();
	CB_TerrainData->UpdateData(&TerrainTessData, sizeof(TerrainTessDesc));

	ImGui::SliderFloat("LandScape : NearSize ", &TextureBlendingData.NearSize, 0.5f, 2.f, "%.2f");
	ImGui::SliderFloat("LandScape : Far Size", &TextureBlendingData.FarSize, 0.1f, 0.5f, "%.3f");
	ImGui::SliderFloat("LandScape : StartOffset", &TextureBlendingData.StartOffset, -3000, 0.f, "%.0f");
	ImGui::SliderFloat("LandScape : Range", &TextureBlendingData.Range, 3000, 10000, "%.0f");

	ImGui::SliderFloat("LandScape : NoiseAmount", &TextureBlendingData.NoiseAmount, 1, 3, "%.1f");
	ImGui::SliderFloat("LandScape : NoisePower", &TextureBlendingData.NoisePower, 1, 3, "%.1f");

	ImGui::SliderFloat("LandScape : SlopBias", &TextureBlendingData.SlopBias, 0, 60, "%.0f");
	ImGui::SliderFloat("LandScape : SlopSharpness", &TextureBlendingData.SlopSharpness, 1, 3, "%.1f");
	ImGui::SliderFloat("LandScape : LowHeight", &TextureBlendingData.LowHeight, 10.f, GetHeightScale() / 10, "%.0f");
	ImGui::SliderFloat("LandScape : HighHeight", &TextureBlendingData.HighHeight, TextureBlendingData.LowHeight, GetHeightScale(), "%.0f");
	ImGui::SliderFloat("LandScape : HeightSharpness", &TextureBlendingData.HeightSharpness, 1, 3, "%.1f");

	bool bUseMacroVariation = TextureBlendingData.bUseMacroVariation != 0;
	ImGui::Checkbox("LandScape : MacroVariation", &bUseMacroVariation);
	TextureBlendingData.bUseMacroVariation = bUseMacroVariation == true ? 1 : 0;
	CB_TextureBlending->UpdateData(&TextureBlendingData, sizeof(TextureBlendingDesc));
}

void LandScape::Render() const
{
	if (!Shader) return;
	
	if (!!VBuffer) VBuffer->BindToGPU();
	if (!!IBuffer) IBuffer->BindToGPU();
	
	if (!!CB_WVP) CB_WVP->BindToGPU();
	if (!!CB_TerrainData) CB_TerrainData->BindToGPU();
	if (!!CB_TextureBlending) CB_TextureBlending->BindToGPU();

	if (!!HeightMap)
		HeightMap->BindToGPU(0, static_cast<UINT>(ShaderType::VDP));
	if (!!BumpMap)
		BumpMap->BindToGPU(1);
	if (!!VariationMap)
		VariationMap->BindToGPU(2);
	if (!!DetailDiffuseMaps)
		DetailDiffuseMaps->BindToGPU(3);
	if (!!DetailNormalMaps)
		DetailNormalMaps->BindToGPU(4);
	if (!!PerlinNoiseMap)
		// PerlinNoiseMap->BindToGPUAsSRV(5, static_cast<UINT>(ShaderType::PixelShader));
		PerlinNoiseMap->BindToGPU(5);
	Shader->DrawIndexed(Indices.size());
}

UINT LandScape::GetWidth() const
{
	return (!!HeightMap) ? HeightMap->GetWidth() : 0; 
}

UINT LandScape::GetHeight() const
{
	return (!!HeightMap) ? HeightMap->GetHeight() : 0; 
}

float LandScape::GetAltitude(const Vector2D & InPositionXZ) const
{
	return 0.f;
}

UINT LandScape::GetPatchSize() const
{
	return PatchSize;
}

const Texture * LandScape::GetHeightMap() const
{
	return HeightMap;
}

float LandScape::GetHeightScale() const
{
	return TerrainTessData.HeightScaler;
}

void LandScape::SetHeightScale(float InHeightScale)
{
	TerrainTessData.HeightScaler = InHeightScale;
}

void LandScape::SetupShaders()
{
	string PatchSizeStr = std::to_string(64);
	const vector<D3D_SHADER_MACRO> Macros = {
		{"TYPE01", "0"},
		{"MAX_TESS_FACTOR", PatchSizeStr.c_str()},
		{nullptr,}
	};
	Shader = new HlslShader<VertexType>(
		L"Terrain/TerrainTessellation.hlsl",
		static_cast<UINT>(ShaderType::VHDP),
		"VSMain",
		"PSMain",
		"",
		Macros.data()
	);
	Shader->SetTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	CHECK(SUCCEEDED(Shader->CreateSamplerState_Linear_Clamp(static_cast<UINT>(ShaderType::VDP)))); // Clamp
	CHECK(SUCCEEDED(Shader->CreateSamplerState_Anisotropic_Wrap(static_cast<UINT>(ShaderType::VDP))));
	CHECK(SUCCEEDED(Shader->CreateBlendState_Opaque()));
	CHECK(SUCCEEDED(Shader->CreateDepthStencilState_Default()));
	CHECK(SUCCEEDED(Shader->CreateRasterizerState_Solid()));
	// CHECK(SUCCEEDED(Shader->CreateRasterizerState_WireFrame()));
	if (Macros[0].Name == "TYPE03")
	{
		TerrainTessData.LODRange = {10,256};
		LODRange = {100, 1000};
	}
	else
	{
		TerrainTessData.LODRange = {1.f, 3};
		LODRange = {1, 10};
	}

}

void LandScape::SetupResources(const LandScapeDesc & Desc)
{
	VariationMap = new Texture(L"Terrain/T_MacroVariation.png", true);
	if (!Desc.HeightMapName.empty())
	{
		HeightMap = new Texture(Desc.HeightMapName, true);
		SetHeightScale(Desc.HeightScaler);
	}
	if (!Desc.BumpMapName.empty())
	{
		BumpMap = new Texture(Desc.BumpMapName, true);
	}
	if (!Desc.DiffuseMapNames.empty())
	{
		DetailDiffuseMaps = new TextureArray(Desc.DiffuseMapNames, 1024, 1024, 11);
		TerrainTessData.DiffuseMapCount = Desc.DiffuseMapNames.size();
	}
	if (!Desc.NormalMapNames.empty())
	{
		DetailNormalMaps = new TextureArray(Desc.NormalMapNames, 1024, 1024, 11);
		TerrainTessData.NormalMapCount = Desc.NormalMapNames.size();
	}
	
	Tf = new Transform();
	TerrainTessData.TexelSize = {
		1.f / static_cast<float>(HeightMap->GetWidth()),
		1.f / static_cast<float>(HeightMap->GetHeight())
	};
	TerrainTessData.TerrainSize = HeightMap->GetWidth();
	TerrainTessData.GridSize = PatchSize;
	TerrainTessData.TextureSize = 1024;
	
	CreateVertex();
	VBuffer = new VertexBuffer(
		Vertices.data(),
		Vertices.size(),
		sizeof(VertexType)
	);
	
	CreateIndex();
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
	CB_TerrainData = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VHDP),
		1,
		nullptr,
		"Height Scaler",
		sizeof(TerrainTessDesc),
		false
	);
	CB_TextureBlending = new ConstantBuffer(
		static_cast<UINT>(ShaderType::PixelShader),
		2,
		nullptr,
		"Distance Based Blending",
		sizeof(TextureBlendingData),
		false
	);
}

void LandScape::CreateVertex()
{
	const UINT Width = GetWidth();
	const UINT Height = GetHeight();

	const UINT PatchWidth = Width / PatchSize + 1;
	const UINT PatchHeight = Height / PatchSize + 1;

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

void LandScape::CreateIndex()
{
	const UINT Width = GetWidth();
	const UINT Height = GetHeight();

	const UINT PatchWidth = Width / PatchSize + 1;
	const UINT PatchHeight = Height / PatchSize + 1;
	
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

void LandScape::CreatePerlinNoiseMap()
{
	PerlinNoiseMap = new Texture(L"Terrain/T_Perlin_Noise.png", true);

	// PerlinNoiseMap = new RWTexture2D(
	// 	1024,
	// 	1024,
	// 	DXGI_FORMAT_R32_FLOAT
	// );
	// const vector<D3D_SHADER_MACRO> ShaderMacros = {
	// 	{"THREAD_X", "32"},
	// 	{"THREAD_Y", "32"},
	// 	{nullptr, }
	// };
	// HlslComputeShader * CS_PerlinNoiseMap = new HlslComputeShader(
	// 	L"Terrain/Compute/ComputePerlinNoise.hlsl",
	// 	ShaderMacros.data()
	// );
	// CS_PerlinNoiseMap->SetDispatchSize(
	// 	PerlinNoiseMap->GetWidth() / 32,
	// 	PerlinNoiseMap->GetHeight() / 32,
	// 	1
	// );
	// PerlinNoiseMap->BindToGPUAsUAV(0);
	// CS_PerlinNoiseMap->Dispatch();
	// PerlinNoiseMap->UpdateSRV();
	// PerlinNoiseMap->SaveOutputAsFile(L"PerlinNoise");
	// SAFE_DELETE(CS_PerlinNoiseMap);
}
