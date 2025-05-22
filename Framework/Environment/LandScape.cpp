#include "framework.h"
#include "LandScape.h"

LandScape::LandScape(const LandScapeDesc& Desc)
{
	ASSERT(Desc.DiffuseMapNames.size() == Desc.NormalMapNames.size(), "DetailMaps Not Valid");

	TerrainDimension[0] = static_cast<UINT>(Desc.Extent.X);
	TerrainDimension[1] = static_cast<UINT>(Desc.Extent.Z);
	PatchSize = Desc.PatchSize;
	TessellationData.HeightScaler = Desc.Extent.Y;

	CreatePerlinNoiseMap();
	SetupShaders();
	SetupResources(Desc);
}

LandScape::LandScape(const wstring & InHeightMapFilename, const UINT PatchSize)
	: LandScape({{1024, 100, 1024}, PatchSize, InHeightMapFilename, {}, {}})
{
}

LandScape::~LandScape()
{
	SAFE_DELETE(Shader);
	
	SAFE_DELETE(HeightMap);
	SAFE_DELETE(VariationMap);
	SAFE_DELETE(DetailDiffuseMaps);
	SAFE_DELETE(DetailNormalMaps);
	
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	
	SAFE_DELETE(Tf);
	
	SAFE_DELETE(CB_WVP);
	SAFE_DELETE(CB_Light);
	SAFE_DELETE(CB_Blending);
	SAFE_DELETE(CB_Tessellation);
}

void LandScape::Tick()
{
	MatrixData.World = Tf->GetMatrix();
	MatrixData.View = Context::Get()->GetViewMatrix();
	MatrixData.Projection = Context::Get()->GetProjectionMatrix();
	CB_WVP->UpdateData(&MatrixData, sizeof(WVPDesc));

	ImGui::SliderFloat("LandScape : LOD Power", &TessellationData.LODRange.X, 0.1f, 3.f, "%.1f");
	ImGui::SliderFloat("LandScape : Min Screen Diagonal", &TessellationData.LODRange.Y, LODRange.X, LODRange.Y, "%.0f");
	TessellationData.ScreenDistance = D3D::GetDesc().Height * 0.5f * Context::Get()->GetCamera()->GetProjectionMatrix().M22;
	TessellationData.ScreenDiagonal = D3D::GetDesc().Height * D3D::GetDesc().Height + D3D::GetDesc().Width * D3D::GetDesc().Width;
	TessellationData.CameraWorldPosition = Context::Get()->GetCamera()->GetPosition();
	CB_Tessellation->UpdateData(&TessellationData, sizeof(LandScapeTessellationDesc));

	LightData.LightDirection = Context::Get()->GetLightDirection();
	LightData.LightColor = Context::Get()->GetLightColor();
	CB_Light->UpdateData(&LightData, sizeof(DirectionalLightDesc));

	ImGui::SliderFloat("LandScape : NearSize ", &BlendingData.NearSize, 0.5f, 2.f, "%.2f");
	ImGui::SliderFloat("LandScape : Far Size", &BlendingData.FarSize, 0.1f, 0.5f, "%.3f");
	ImGui::SliderFloat("LandScape : StartOffset", &BlendingData.StartOffset, -3000, 0.f, "%.0f");
	ImGui::SliderFloat("LandScape : Range", &BlendingData.Range, 3000, 10000, "%.0f");

	ImGui::SliderFloat("LandScape : NoiseAmount", &BlendingData.NoiseAmount, 1, 3, "%.1f");
	ImGui::SliderFloat("LandScape : NoisePower", &BlendingData.NoisePower, 1, 3, "%.1f");

	ImGui::SliderFloat("LandScape : SlopBias", &BlendingData.SlopBias, 0, 60, "%.0f");
	ImGui::SliderFloat("LandScape : SlopSharpness", &BlendingData.SlopSharpness, 1, 3, "%.1f");
	ImGui::SliderFloat("LandScape : LowHeight", &BlendingData.LowHeight, 0, GetHeightScale() / 10, "%.0f");
	ImGui::SliderFloat("LandScape : HighHeight", &BlendingData.HighHeight, BlendingData.LowHeight, GetHeightScale(), "%.0f");
	ImGui::SliderFloat("LandScape : HeightSharpness", &BlendingData.HeightSharpness, 1, 3, "%.1f");
	CB_Blending->UpdateData(&BlendingData, sizeof(LandScapeBlendingDesc));
}

void LandScape::Render() const
{
	ASSERT(!!Shader, "Shader Doesn't Created")
	
	if (!!VBuffer) VBuffer->BindToGPU();
	if (!!IBuffer) IBuffer->BindToGPU();
	
	CB_WVP->BindToGPU();
	CB_Light->BindToGPU();
	CB_Blending->BindToGPU();
	CB_Tessellation->BindToGPU();

	if (!!HeightMap)
		HeightMap->BindToGPU(0, static_cast<UINT>(ShaderType::VDP));
	if (!!VariationMap)
		VariationMap->BindToGPU(1);
	if (!!DetailDiffuseMaps)
		DetailDiffuseMaps->BindToGPU(2);
	if (!!DetailNormalMaps)
		DetailNormalMaps->BindToGPU(3);
	if (!!PerlinNoiseMap)
		PerlinNoiseMap->BindToGPU(4);
	
	Shader->DrawIndexed(Indices.size());
}

UINT LandScape::GetWidth() const
{
	return TerrainDimension[0];
}

UINT LandScape::GetHeight() const
{
	return TerrainDimension[1];
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
	return TessellationData.HeightScaler;
}

void LandScape::SetHeightScale(float InHeightScale)
{
	TessellationData.HeightScaler = InHeightScale;
}

void LandScape::SetupShaders()
{
	// const string GridSizeStr = std::to_string(min(PatchSize, 64));
	const string GridSizeStr = std::to_string(64);
	const vector<D3D_SHADER_MACRO> Macros = {
		{"TYPE01", "0"},
		{"MAX_TESS_FACTOR", GridSizeStr.c_str()},
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
		TessellationData.LODRange = {10,256};
		LODRange = {100, 1000};
	}
	else
	{
		TessellationData.LODRange = {1.f, 3};
		LODRange = {1, 10};
	}

}

void LandScape::SetupResources(const LandScapeDesc & Desc)
{
	VariationMap = new Texture(L"Terrain/T_MacroVariation.png", true);
	if (!Desc.HeightMapName.empty())
	{
		HeightMap = new Texture(Desc.HeightMapName, true);
		SetHeightScale(Desc.Extent.Y);
	}
	if (!Desc.DiffuseMapNames.empty())
	{
		DetailDiffuseMaps = new TextureArray(Desc.DiffuseMapNames, 1024, 1024, 5);
		TessellationData.DiffuseMapCount = Desc.DiffuseMapNames.size();
	}
	if (!Desc.NormalMapNames.empty())
	{
		DetailNormalMaps = new TextureArray(Desc.NormalMapNames, 1024, 1024, 5);
		TessellationData.NormalMapCount = Desc.NormalMapNames.size();
	}
	
	Tf = new Transform();
	TessellationData.TexelSize = {
		1.f / static_cast<float>(HeightMap->GetWidth()),
		1.f / static_cast<float>(HeightMap->GetHeight())
	};
	TessellationData.TextureSize = 1024;
	TessellationData.HeightScaler = GetHeightScale();
	TessellationData.DiffuseMapCount = Desc.DiffuseMapNames.size();
	TessellationData.NormalMapCount = Desc.NormalMapNames.size();
	TessellationData.LODRange = {1, 3};
	TessellationData.TerrainSize = static_cast<float>(TerrainDimension[0]);
	TessellationData.GridSize = static_cast<float>(PatchSize);
	TessellationData.TextureSize = static_cast<float>(HeightMap->GetWidth());

	CreateVertex();
	CreateIndex();

	CB_WVP = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VHDP),
		static_cast<UINT>(WVP),
		nullptr,
		"World, View, Projection",
		sizeof(WVPDesc),
		false
	);
	CB_Tessellation = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VHDP),
		static_cast<UINT>(Tessellation),
		nullptr,
		"Height Scaler",
		sizeof(LandScapeTessellationDesc),
		false
	);
	CB_Light = new ConstantBuffer(
		static_cast<UINT>(ShaderType::PixelShader),
		static_cast<UINT>(Lighting),
		nullptr,
		"Directional Light",
		sizeof(DirectionalLightDesc),
		false
	);
	CB_Blending = new ConstantBuffer(
		static_cast<UINT>(ShaderType::PixelShader),
		static_cast<UINT>(Blending),
		nullptr,
		"Distance Based Blending",
		sizeof(LandScapeBlendingDesc),
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
	VBuffer = new VertexBuffer(
		Vertices.data(),
		Vertices.size(),
		sizeof(VertexType)
	);
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
	IBuffer = new IndexBuffer(
		Indices.data(),
		Indices.size()
	);
}

void LandScape::CreatePerlinNoiseMap()
{
	PerlinNoiseMap = new Texture(L"Terrain/T_Perlin_Noise.png", true);
	// PerlinNoiseMap = Noise::CreatePerlin2DNoise(1024);
}
