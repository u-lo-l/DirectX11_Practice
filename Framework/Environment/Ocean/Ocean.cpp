#include "framework.h"
#include "Ocean.h"



Ocean::Ocean(const OceanDesc& Desc)
: PhillipsInitData()
, PhilipsUpdateData()
, TessellationData()
{
	Dimension = Desc.Dimension;
	TextureSize = Desc.FFTSize;
	CellSize = Desc.CellSize;
	GridSize = Desc.GridSize;
	Tf = new Transform();
	// Tf->SetWorldPosition({Desc.WorldPosition.X, Desc.SeaLevel, Desc.WorldPosition.Y});
	SkyTexture = Desc.SkyTexture;
	TerrainHeightMap = Desc.TerrainHeightMap;
	PhillipsInitData.Wind = Desc.Wind;

#pragma region Compute
	SetupComputeShaders();
	SetupComputeResources();

	GaussianRandomTexture2D = Noise::CreateGaussian2DNoise(TextureSize);
	GenerateInitialSpectrum();
#pragma endregion Compute

#pragma region Render
	SetupRenderShaders();
	SetupRenderResources();
	SetupCells();

#pragma endregion Render
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
	MatrixData.World = Tf->GetMatrix();
	MatrixData.View = Context::Get()->GetViewMatrix();
	MatrixData.Projection = Context::Get()->GetProjectionMatrix();
	MatrixData.ViewInverse = Matrix::Invert(MatrixData.View, true);
	CB_WVPI->UpdateData(&MatrixData, sizeof(WVPDesc));

	LightData.LightDirection = Context::Get()->GetLightDirection();
	LightData.LightColor = Context::Get()->GetLightColor();
	CB_Light->UpdateData(&LightData, sizeof(DirectionalLightDesc));
	
	ImGui::SliderFloat("Ocean : Height Scaler", &TessellationData.HeightScaler, 0.f, 50.f, "%.1f");
	ImGui::SliderFloat("Ocean : LOD Power", &TessellationData.LODRange.X, 0.1f, 3.f, "%.1f");
	ImGui::SliderFloat("Ocean : Min Screen Diagonal", &TessellationData.LODRange.Y, 1, 5, "%.0f");
	ImGui::SliderFloat("Ocean : Noise Scaler", &TessellationData.NoiseScaler, 0.1f, 10, "%.1f");
	ImGui::SliderFloat("Ocean : Noise Power", &TessellationData.NoisePower, 0.1f, 5, "%.1f");
	TessellationData.ScreenDistance = D3D::GetDesc().Height * 0.5f * Context::Get()->GetCamera()->GetProjectionMatrix().M22;
	TessellationData.ScreenDiagonal = D3D::GetDesc().Height * D3D::GetDesc().Height + D3D::GetDesc().Width * D3D::GetDesc().Width;

	TessellationData.CameraPosition = Context::Get()->GetCamera()->GetPosition();
	CB_Tessellation->UpdateData(&TessellationData, sizeof(TessellationDesc));
#pragma endregion Render
}

void Ocean::Render(bool bDrawBoundary)
{
#pragma region Render
	if (!OceanRenderer) return;

	const Frustum * ViewFrustum = Context::Get()->GetCamera()->GetViewFrustum();
	
	if (!!CB_WVPI) CB_WVPI->BindToGPU();
	if (!!CB_Light) CB_Light->BindToGPU();
	if (!!CB_Tessellation) CB_Tessellation->BindToGPU();

	if (!!DisplacementMap)
		DisplacementMap->BindToGPUAsSRV(0, static_cast<UINT>(ShaderType::VDP));
	if (!!SkyTexture)
		SkyTexture->BindToGPU(1, static_cast<UINT>(ShaderType::PixelShader));
	if (!!FoamGrid)
		FoamGrid->BindToGPUAsSRV(2, static_cast<UINT>(ShaderType::PixelShader));
	if (!!PerlinNoise)
		PerlinNoise->BindToGPU(3, static_cast<UINT>(ShaderType::VDP));

	static bool bFrustumCull = false;
	ImGui::Checkbox("Frustum Cull", &bFrustumCull);
	int RenderingCell = 0;
	for (SceneryCell * Cell : Cells)
	{
		if (Cell->Render(OceanRenderer, bFrustumCull ? ViewFrustum : nullptr) == true)
			RenderingCell++;
	}
	Gui::Get()->RenderText(5, 160, 1.f, 0.2f, 0.2f, String::Format("Ocean | VisibleCells : %d / %d", RenderingCell, Cells.size()));

	static bool b = false;
	ImGui::Checkbox("Ocean | DrawBoundary", &b);
	if (b)
	{
		CellBoxVBuffer->BindToGPU();
		CellBoxIBuffer->BindToGPU();
		CellBoxInstBuffer->BindToGPU();
		CellBoundaryRenderer->DrawIndexedInstanced(
			CellBoxIBuffer->GetCount(),
			CellLocalTransform.size(),
			0,
			0
		);
	}
#pragma endregion Render
}

void Ocean::SaveHeightMap()
{
	IFFT_Result->SaveOutputAsFile(L"OceanHeightMap");
}

void Ocean::SetWorldPosition(const Vector& Position) const
{ 
	Tf->SetWorldPosition(Position);
	const Matrix & NewTf = Tf->GetMatrix();
	const Vector & OceanOrigin = Tf->GetWorldPosition();
	for (Matrix CellTf : CellLocalTransform)
	{
		CellTf = CellTf * NewTf;
	}
	for (SceneryCell * Cell : Cells)
	{
		Cell->SetCenter(OceanOrigin + Cell->GetLocalPosition());
	}
}

// void Ocean::SetWorldRotation(const Vector& RotationInDeg) const
// { 
// 	Tf->SetWorldRotation(RotationInDeg * Math::DegToRadian);
// 	const Matrix & NewTf = Tf->GetMatrix();
// 	const Vector & OceanOrigin = Tf->GetWorldPosition();
// 	for (Matrix CellTf : CellLocalTransform)
// 	{
// 		CellTf = NewTf * CellTf;
// 	}
// 	for (SceneryCell * Cell : Cells)
// 	{
// 		Cell->SetCenter(OceanOrigin + Cell->GetLocalPosition());
// 	}
// }

void Ocean::SetupRenderShaders()
{
	const vector<D3D_SHADER_MACRO> Macros = { {"TYPE01", ""}, {nullptr,}};
	OceanRenderer = new HlslShader<VertexType>(
		L"Ocean/Ocean.hlsl",
		static_cast<UINT>(ShaderType::VHDP),
		"VSMain",
		"PSMain",
		"",
		Macros.data()
	);
	OceanRenderer->SetTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	CHECK(SUCCEEDED(OceanRenderer->CreateSamplerState_Linear_Border(static_cast<UINT>(ShaderType::VDP))));
	CHECK(SUCCEEDED(OceanRenderer->CreateSamplerState_Anisotropic(static_cast<UINT>(ShaderType::VDP))));
	CHECK(SUCCEEDED(OceanRenderer->CreateBlendState_Multiply()));
	CHECK(SUCCEEDED(OceanRenderer->CreateDepthStencilState_Default()));
	CHECK(SUCCEEDED(OceanRenderer->CreateRasterizerState_Solid()));
	// CHECK(SUCCEEDED(OceanRenderer->CreateRasterizerState_WireFrame()));

	CellBoundaryRenderer = new HlslShader<VertexColor>(L"Debug/Boundary.hlsl");
	CellBoundaryRenderer->SetTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	CHECK(SUCCEEDED(CellBoundaryRenderer->CreateBlendState_Opaque()));
}

void Ocean::SetupRenderResources()
{
	PerlinNoise = new Texture(L"PerlinNoise.png", true);
	// PerlinNoise = new Texture(L"Terrain/T_Perlin_Noise.png", true);
	
	TessellationData.TexelSize = {
		1.f / static_cast<float>(TextureSize),
		1.f / static_cast<float>(TextureSize)
	};
	if (!!DisplacementMap)
		TessellationData.HeightMapTiling =  Vector2D(Dimension.X, Dimension.Z) / static_cast<float>(DisplacementMap->GetWidth());

	if (!!PerlinNoise)
		TessellationData.NoiseTiling = TessellationData.HeightMapTiling / 16.f;
	// TessellationData.NoiseTiling =  Vector2D( (float)PatchSize / (float)PerlinNoise->GetWidth(), (float)PatchSize / (float)PerlinNoise->GetHeight()) ;

	TessellationData.HeightScaler = 4.f;
	TessellationData.NoiseScaler = 1.f;
	TessellationData.NoisePower = 1.f;

	CB_WVPI = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VHDP),
		0,
		nullptr,
		"World, View, Projection",
		sizeof(WVPDesc),
		false
	);
	CB_Light = new ConstantBuffer(
		static_cast<UINT>(ShaderType::PixelShader),
		1,
		nullptr,
		"Light",
		sizeof(DirectionalLightDesc),
		false
	);
	CB_Tessellation = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VHDP),
		2,
		nullptr,
		"Height Scaler",
		sizeof(TessellationDesc),
		false
	);
}

void Ocean::SetupCells()
{
	const float HeightScale = Dimension.Y;
	const float Size = static_cast<float>(CellSize);
	vector<VertexColor> BoxVertices = { 
		{ Vector(0.f, 0.f, 0.f), Color::Green },
		{ Vector(0.f, 0.f, 1.f), Color::Green },
		{ Vector(1.f, 0.f, 1.f), Color::Green },
		{ Vector(1.f, 0.f, 0.f), Color::Green },

		{ Vector(0.f, 1.f, 0.f), Color::Green },
		{ Vector(0.f, 1.f, 1.f), Color::Green },
		{ Vector(1.f, 1.f, 1.f), Color::Green },
		{ Vector(1.f, 1.f, 0.f), Color::Green }
	};
	vector<UINT> BoxIndices = {
		0, 1, 1, 2, 2, 3, 3, 0,
		0, 4, 1, 5, 2, 6, 3, 7,
		4, 5, 5, 6, 6, 7, 7, 4
	};
	const UINT Width = static_cast<UINT>(ceil(Dimension.X / static_cast<float>(CellSize)));
	const UINT Height = static_cast<UINT>(ceil(Dimension.Z / static_cast<float>(CellSize)));
	Cells.clear();
	CellLocalTransform.clear();
	for (UINT h = 0; h < Height; h++)
	{
		for (UINT w = 0; w < Width; w++)
		{
			SceneryCell * NewCell = new SceneryCell (
				Vector(Size, HeightScale, Size),
				Vector2D(static_cast<float>(w), static_cast<float>(h)),
				static_cast<float>(GridSize),
				static_cast<float>(TextureSize) * 0.5f,
				Dimension
			);
			Cells.push_back(NewCell);
			Transform CellLocalTf;
			CellLocalTf.SetScale(NewCell->GetDimension());
			CellLocalTf.SetWorldPosition(NewCell->GetLocalPosition());
			CellLocalTransform.push_back(CellLocalTf.GetMatrix());
		}
	}

#pragma region Bounding Box
	CellBoxVBuffer = new VertexBuffer(BoxVertices.data(), BoxVertices.size(), sizeof(VertexColor));
	CellBoxIBuffer = new IndexBuffer(BoxIndices.data(), BoxIndices.size());
	CellBoxInstBuffer = new InstanceBuffer(
		CellLocalTransform.data(),
		CellLocalTransform.size() + 1, // TODO : 왜 +1 해야 되는지 이유 파악하기.
		sizeof(Matrix)
	);
#pragma endregion Bounding Box
}
// void Ocean::CreateVertex()
// {
// 	const UINT Width = Dimension[0];
// 	const UINT Height = Dimension[1];
//
// 	const UINT PatchWidth = (Dimension[0] / PatchSize) + 1;
// 	const UINT PatchHeight = (Dimension[1] / PatchSize) + 1;
//
// 	Vertices.clear();
// 	Vertices.resize(PatchWidth * PatchHeight);
// 	for (UINT Z = 0 ; Z < PatchHeight ; Z++)
// 	{
// 		for (UINT X = 0 ; X < PatchWidth ; X++)
// 		{
// 			const UINT Index = Z * PatchWidth + X;
// 			Vertices[Index].Position.X = static_cast<float>(X * PatchSize);
// 			Vertices[Index].Position.Y = 0;
// 			Vertices[Index].Position.Z = static_cast<float>(Z * PatchSize);
// 			Vertices[Index].Normal = {0, 1, 0};
// 			Vertices[Index].UV.X = (X * PatchSize) / static_cast<float>(Width);
// 			Vertices[Index].UV.Y = (Z * PatchSize) / static_cast<float>(Height);
// 		}
// 	}
// }
//
// void Ocean::CreateIndex()
// {
// 	const UINT PatchWidth = (Dimension[0] / PatchSize) + 1;
// 	const UINT PatchHeight = (Dimension[1] / PatchSize) + 1;
// 	
// 	const UINT IndexCount = (PatchWidth - 1) * (PatchHeight - 1) * 4;
//
// 	Indices.clear();
// 	Indices.resize(IndexCount);
// 	UINT Index = 0;
// 	for (UINT Z = 0 ; Z < PatchHeight - 1 ; Z++)
// 	{
// 		for (UINT X = 0 ; X < PatchWidth  - 1; X++)
// 		{
// 			Indices[Index++] = PatchWidth * Z + X;
// 			Indices[Index++] = PatchWidth * (Z + 1) + X;
// 			Indices[Index++] = PatchWidth * (Z + 1) + (X + 1);
// 			Indices[Index++] = PatchWidth * Z + (X + 1);
// 		}
// 	}
// }