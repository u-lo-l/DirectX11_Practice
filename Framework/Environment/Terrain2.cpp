#include "framework.h"
#include "Terrain2.h"

Terrain2::Terrain2(const wstring& InHeightMapFilename, UINT PatchSize)
	: PatchSize(PatchSize)
{
	Shader = new HlslShader<VertexType>(
		L"Tessellation/TerrainTessellation.hlsl",
		static_cast<UINT>(ShaderType::VHDP)
	);
	Shader->SetTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	CHECK(SUCCEEDED(Shader->CreateSamplerState_Linear(static_cast<UINT>(ShaderType::VDP))));
	CHECK(SUCCEEDED(Shader->CreateBlendState_Opaque()));
	CHECK(SUCCEEDED(Shader->CreateDepthStencilState_Default()));
	CHECK(SUCCEEDED(Shader->CreateRasterizerState_WireFrame()));
	
	HeightMap = new Texture(InHeightMapFilename, true);
	Tf = new Transform();

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
}

Terrain2::~Terrain2()
{
	SAFE_DELETE(Shader);
	SAFE_DELETE(HeightMap);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(Tf)
	SAFE_DELETE(WVPCBuffer);
	SAFE_DELETE(HeightScalerCBuffer);
}

void Terrain2::Tick()
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	WVP.World = Tf->GetMatrix();
	WVP.View = Context::Get()->GetViewMatrix();
	WVP.Projection = Context::Get()->GetProjectionMatrix();
	WVPCBuffer->UpdateData(&WVP, sizeof(WVPDesc));
	
	ImGui::SliderFloat("Height Scaler", &TerrainTessData.HeightScaler, 1.f, 100.f, "%.0f");
	ImGui::SliderFloat("TriSize", &TerrainTessData.TriSize, 4.f, 12.f, "%.0f");
	TerrainTessData.ScreenDistance = D3D::GetDesc().Height * 0.5f / tanf(Math::Pi * 0.25f / 2);
	TerrainTessData.ScreenDiagonal = sqrt(D3D::GetDesc().Height * D3D::GetDesc().Height + D3D::GetDesc().Width * D3D::GetDesc().Width);
	TerrainTessData.CameraPosition = Context::Get()->GetCamera()->GetPosition();
	HeightScalerCBuffer->UpdateData(&TerrainTessData, sizeof(TerrainTessDesc));
}

void Terrain2::Render() const
{
	if (!Shader) return;
	if (!!VBuffer) VBuffer->BindToGPU();
	if (!!IBuffer) IBuffer->BindToGPU();
	if (!!WVPCBuffer) WVPCBuffer->BindToGPU();
	if (!!HeightScalerCBuffer) HeightScalerCBuffer->BindToGPU();
	if (!!HeightMap) HeightMap->BindToGPU(0, static_cast<UINT>(ShaderType::VDP));
	Shader->DrawIndexed(Indices.size());
}

UINT Terrain2::GetWidth() const
{
	return (!!HeightMap) ? HeightMap->GetWidth() : 0; 
}

UINT Terrain2::GetHeight() const
{
	return (!!HeightMap) ? HeightMap->GetHeight() : 0; 
}

float Terrain2::GetAltitude(const Vector2D & InPositionXZ) const
{
	return 0.f;
}

UINT Terrain2::GetPatchSize() const
{
	return PatchSize;
}

// void Terrain2::SetPatchSize(UINT InPatchSize)
// {
// 	PatchSize = InPatchSize < 2 ? 2 : InPatchSize;
// }

void Terrain2::CreateVertex()
{
	const UINT Width = GetWidth();
	const UINT Height = GetHeight();

	const UINT PatchWidth = Width / PatchSize;
	const UINT PatchHeight = Height / PatchSize;

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
			Vertices[Index].UV.X = (X * PatchSize) / static_cast<float>(Width - 1);
			Vertices[Index].UV.Y = (Z * PatchSize) / static_cast<float>(Height - 1);
		}
	}
}

void Terrain2::CreateIndex()
{
	const UINT Width = GetWidth();
	const UINT Height = GetHeight();

	const UINT PatchWidth = Width / PatchSize;
	const UINT PatchHeight = Height / PatchSize;
	
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
