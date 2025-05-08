#include "framework.h"
#include "Ocean.h"

Ocean::Ocean(const UINT InPatchSize) : PatchSize(InPatchSize)
{
#pragma region Compute
	SetupShaders();
	SetupResources();

	GenerateGaussianRandoms();
	GenerateInitialSpectrum();
#pragma endregion Compute

#pragma region Render
	const vector<D3D_SHADER_MACRO> Macros = { {"TYPE01", ""}, {nullptr,}};
	Shader = new HlslShader<VertexType>(
		L"Terrain/TerrainTessellation.hlsl",
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
	
	Tf = new Transform();
	TessellationData.TexelSize = {
		1.f / static_cast<float>(HeightMapTexture2D->GetWidth()),
		1.f / static_cast<float>(HeightMapTexture2D->GetHeight())
	};
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
	CB_LightDirection = new ConstantBuffer(
		ShaderType::PixelShader,
		2,
		nullptr,
		"LightDirection",
		sizeof(LightDirectionDesc),
		false
	);
#pragma endregion Render
}

Ocean::~Ocean()
{
#pragma region Compute
	// Resources
	SAFE_DELETE(GaussianRandomTexture2D);
	SAFE_DELETE(InitialSpectrumTexture2D);
	SAFE_DELETE(SpectrumTexture2D);
	SAFE_DELETE(IFFT_Row);
	SAFE_DELETE(IFFT_Row_Transposed);
	SAFE_DELETE(HeightMapTexture2D);
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
	PhilipsUpdateData.RunningTime = (sdt::SystemTimer::Get()->GetRunningTime() - PhilipsUpdateData.InitTime) / 10.0f;
	CB_PhillipsUpdate->UpdateData(&PhilipsUpdateData, sizeof(PhilipsUpdateDesc));
		
	UpdateSpectrum();
	GetHeightMap();
#pragma endregion Compute

#pragma region Render
	WVP.World = Tf->GetMatrix();
	WVP.View = Context::Get()->GetViewMatrix();
	WVP.Projection = Context::Get()->GetProjectionMatrix();
	CB_WVP->UpdateData(&WVP, sizeof(WVPDesc));

	ImGui::SliderFloat("Ocean : Height Scaler", &TessellationData.HeightScaler, 0.f, 100.f, "%.1f");
	ImGui::SliderFloat("Ocean : LOD Power", &TessellationData.LODRange.X, 0.1f, 3.f, "%.1f");
	ImGui::SliderFloat("Ocean : Min Screen Diagonal", &TessellationData.LODRange.Y, LODRange.X, LODRange.Y, "%.0f");
	TessellationData.ScreenDistance = D3D::GetDesc().Height * 0.5f * Context::Get()->GetCamera()->GetProjectionMatrix().M22;
	TessellationData.ScreenDiagonal = D3D::GetDesc().Height * D3D::GetDesc().Height + D3D::GetDesc().Width * D3D::GetDesc().Width;
	TessellationData.CameraPosition = Context::Get()->GetCamera()->GetPosition();
	CB_Tessellation->UpdateData(&TessellationData, sizeof(TessellationData));

	LightDirectionData.Direction = Context::Get()->GetLightDirection();
	CB_LightDirection->UpdateData(&LightDirectionData, sizeof(LightDirectionDesc));
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
	if (!!CB_LightDirection) CB_LightDirection->BindToGPU();
	
	HeightMapTexture2D->BindToGPUAsSRV(0, static_cast<UINT>(ShaderType::VDP));
	Shader->DrawIndexed(Indices.size());
#pragma endregion Render
}

void Ocean::SaveHeightMap()
{
	HeightMapTexture2D->SaveOutputAsFile(L"OceanHeightMap");
}


void Ocean::CreateVertex()
{
	const UINT Width = HeightMapTexture2D->GetWidth();
	const UINT Height = HeightMapTexture2D->GetHeight();

	const UINT PatchWidth = (Width / PatchSize) + 1;
	const UINT PatchHeight = (Height / PatchSize) + 1;

	Vertices.clear();
	Vertices.resize(PatchWidth * PatchHeight);
	for (UINT Z = 0 ; Z < PatchHeight ; Z++)
	{
		for (UINT X = 0 ; X < PatchWidth ; X++)
		{
			const UINT Index = Z * PatchWidth + X;
			Vertices[Index].Position.X = static_cast<float>(X * PatchSize) * 2.f;
			Vertices[Index].Position.Y = 0;
			Vertices[Index].Position.Z = static_cast<float>(Z * PatchSize) * 2.f;
			Vertices[Index].Normal = {0, 1, 0};
			Vertices[Index].UV.X = (X * PatchSize) / static_cast<float>(Width);
			Vertices[Index].UV.Y = (Z * PatchSize) / static_cast<float>(Height);
		}
	}
}

void Ocean::CreateIndex()
{
	const UINT Width = HeightMapTexture2D->GetWidth();
	const UINT Height = HeightMapTexture2D->GetHeight();

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