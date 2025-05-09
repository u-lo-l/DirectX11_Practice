#include "framework.h"
#include "LandScape.h"

LandScape::LandScape(const LandScapeDesc& Desc, const UINT PatchSize)
	: PatchSize(PatchSize)
{
	const vector<D3D_SHADER_MACRO> Macros = { {"TYPE01", "0"}, {nullptr,}};
	Shader = new HlslShader<VertexType>(
		L"Terrain/TerrainTessellation.hlsl",
		static_cast<UINT>(ShaderType::VHDP),
		"VSMain",
		"PSMain",
		"",
		Macros.data()
	);
	Shader->SetTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	CHECK(SUCCEEDED(Shader->CreateSamplerState_Linear(static_cast<UINT>(ShaderType::VDP))));
	CHECK(SUCCEEDED(Shader->CreateSamplerState_Anisotropic_Clamp(static_cast<UINT>(ShaderType::VDP))));
	CHECK(SUCCEEDED(Shader->CreateBlendState_Opaque()));
	CHECK(SUCCEEDED(Shader->CreateDepthStencilState_Default()));
	CHECK(SUCCEEDED(Shader->CreateRasterizerState_Solid()));
	// CHECK(SUCCEEDED(Shader->CreateRasterizerState_WireFrame()));

	if (!Desc.HeightMapName.empty())
		HeightMap = new Texture(Desc.HeightMapName, true);
	if (!Desc.DiffuseMapName.empty())
		DiffuseMap = new Texture(Desc.DiffuseMapName, true);
	if (!Desc.BumpMapName.empty())
		BumpMap = new Texture(Desc.BumpMapName, true);
	if (!Desc.NormalMapName.empty())
		NormalMap = new Texture(Desc.NormalMapName, true);
	
	Tf = new Transform();
	TerrainTessData.TexelSize = {
		1.f / static_cast<float>(HeightMap->GetWidth()),
		1.f / static_cast<float>(HeightMap->GetHeight())
	};
	if (Macros[0].Name == "TYPE03")
	{
		TerrainTessData.LODRange = {
			10,
			256
		};
		LODRange = {100, 1000};
	}
	else
	{
		TerrainTessData.LODRange = {
			2.f,
			5
		};
		LODRange = {1, 10};
	}
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
	WVPCBuffer = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VHDP),
		0,
		nullptr,
		"World, View, Projection",
		sizeof(WVPDesc),
		false
	);
	HeightScalerCBuffer = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VHDP),
		1,
		nullptr,
		"Height Scaler",
		sizeof(TerrainTessDesc),
		false
	);
	LightDirectionCBuffer = new ConstantBuffer(
		ShaderType::PixelShader,
		2,
		nullptr,
		"LightDirection",
		sizeof(LightDirectionDesc),
		false
	);
}

LandScape::LandScape(const wstring & InHeightMapFilename, const UINT PatchSize)
	: LandScape({InHeightMapFilename, L"", L"", L""}, PatchSize)
{
}

LandScape::~LandScape()
{
	SAFE_DELETE(Shader);
	SAFE_DELETE(HeightMap);
	SAFE_DELETE(DiffuseMap);
	SAFE_DELETE(BumpMap);
	SAFE_DELETE(NormalMap);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(Tf)
	SAFE_DELETE(WVPCBuffer);
	SAFE_DELETE(HeightScalerCBuffer);
}

void LandScape::Tick()
{
	WVP.World = Tf->GetMatrix();
	WVP.View = Context::Get()->GetViewMatrix();
	WVP.Projection = Context::Get()->GetProjectionMatrix();
	WVPCBuffer->UpdateData(&WVP, sizeof(WVPDesc));
	
	ImGui::SliderFloat("LandScape : LOD Power", &TerrainTessData.LODRange.X, 0.1f, 3.f, "%.1f");
	ImGui::SliderFloat("LandScape : Min Screen Diagonal", &TerrainTessData.LODRange.Y, LODRange.X, LODRange.Y, "%.0f");
	TerrainTessData.ScreenDistance = D3D::GetDesc().Height * 0.5f * Context::Get()->GetCamera()->GetProjectionMatrix().M22;
	TerrainTessData.ScreenDiagonal = D3D::GetDesc().Height * D3D::GetDesc().Height + D3D::GetDesc().Width * D3D::GetDesc().Width;
	TerrainTessData.CameraPosition = Context::Get()->GetCamera()->GetPosition();
	HeightScalerCBuffer->UpdateData(&TerrainTessData, sizeof(TerrainTessDesc));

	LightDirectionData.Direction = Context::Get()->GetLightDirection();
	LightDirectionCBuffer->UpdateData(&LightDirectionData, sizeof(LightDirectionDesc));
}

void LandScape::Render() const
{
	if (!Shader) return;
	
	if (!!VBuffer) VBuffer->BindToGPU();
	if (!!IBuffer) IBuffer->BindToGPU();
	
	if (!!WVPCBuffer) WVPCBuffer->BindToGPU();
	if (!!HeightScalerCBuffer) HeightScalerCBuffer->BindToGPU();
	if (!!LightDirectionCBuffer) LightDirectionCBuffer->BindToGPU();

	if (!!HeightMap) HeightMap->BindToGPU(0, static_cast<UINT>(ShaderType::VDP));
	if (!!DiffuseMap) DiffuseMap->BindToGPU(1, static_cast<UINT>(ShaderType::PixelShader));
	if (!!BumpMap) BumpMap->BindToGPU(2, static_cast<UINT>(ShaderType::PixelShader));
	if (!!NormalMap) NormalMap->BindToGPU(3, static_cast<UINT>(ShaderType::PixelShader));
	
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
