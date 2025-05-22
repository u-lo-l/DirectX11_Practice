#include "framework.h"
#include "LandScape.h"

LandScape::LandScape(const LandScapeDesc& InDesc)
{
	Extent = InDesc.Extent;
	CellSize = InDesc.CellSize;
	GridSize = InDesc.GridSize;
	
	SetupShaders();
	SetupResources(InDesc);
	SetupCells();
}

LandScape::~LandScape()
{
	for (SceneryCell * Cell : Cells)
		SAFE_DELETE(Cell);
	
	SAFE_DELETE(CellRenderer);
	SAFE_DELETE(CellBoundaryRenderer);

	SAFE_DELETE(HeightMap);
	SAFE_DELETE(VariationMap);
	SAFE_DELETE(DiffuseMaps);
	SAFE_DELETE(NormalMaps);

	SAFE_DELETE(Tf)
	
	SAFE_DELETE(CB_WVP);
	SAFE_DELETE(CB_Light);
	SAFE_DELETE(CB_Blending);
	SAFE_DELETE(CB_Tessellation);
}

void LandScape::Tick()
{
	MatrixData.World = Tf->GetMatrix();
	MatrixData.View = Context::Get()->GetCamera()->GetViewMatrix();
	MatrixData.Projection = Context::Get()->GetCamera()->GetProjectionMatrix();
	CB_WVP->UpdateData(&MatrixData, sizeof(WVPDesc));

	ImGui::SliderFloat("LandScape_Cell : LOD Power", &TessellationData.LODRange.X, 0.1f, 3.f, "%.1f");
	ImGui::SliderFloat("LandScape_Cell : Min Screen Diagonal", &TessellationData.LODRange.Y, 1, 10, "%.0f");
	TessellationData.ScreenDistance = D3D::GetDesc().Height * 0.5f * Context::Get()->GetCamera()->GetProjectionMatrix().M22;
	TessellationData.ScreenDiagonal = D3D::GetDesc().Height * D3D::GetDesc().Height + D3D::GetDesc().Width * D3D::GetDesc().Width;
	TessellationData.CameraWorldPosition = Context::Get()->GetCamera()->GetPosition();
	CB_Tessellation->UpdateData(&TessellationData, sizeof(LandScapeTessellationDesc));

	LightData.LightDirection = Context::Get()->GetLightDirection();
	LightData.LightColor = Context::Get()->GetLightColor();
	CB_Light->UpdateData(&LightData, sizeof(DirectionalLightDesc));

	ImGui::SliderFloat("LandScape_Cell : NearSize ", &BlendingData.NearSize, 0.5f, 2.f, "%.2f");
	ImGui::SliderFloat("LandScape_Cell : Far Size", &BlendingData.FarSize, 0.1f, 0.5f, "%.3f");
	ImGui::SliderFloat("LandScape_Cell : StartOffset", &BlendingData.StartOffset, -3000, 0.f, "%.0f");
	ImGui::SliderFloat("LandScape_Cell : Range", &BlendingData.Range, 3000, 10000, "%.0f");

	ImGui::SliderFloat("LandScape_Cell : NoiseAmount", &BlendingData.NoiseAmount, 1, 3, "%.1f");
	ImGui::SliderFloat("LandScape_Cell : NoisePower", &BlendingData.NoisePower, 1, 3, "%.1f");

	ImGui::SliderFloat("LandScape_Cell : SlopBias", &BlendingData.SlopBias, 0, 60, "%.0f");
	ImGui::SliderFloat("LandScape_Cell : SlopSharpness", &BlendingData.SlopSharpness, 1, 3, "%.1f");
	ImGui::SliderFloat("LandScape_Cell : LowHeight", &BlendingData.LowHeight, 0, Extent.Y / 10, "%.0f");
	ImGui::SliderFloat("LandScape_Cell : HighHeight", &BlendingData.HighHeight, BlendingData.LowHeight, Extent.Y, "%.0f");
	ImGui::SliderFloat("LandScape_Cell : HeightSharpness", &BlendingData.HeightSharpness, 1, 3, "%.1f");
	CB_Blending->UpdateData(&BlendingData, sizeof(LandScapeBlendingDesc));
}

void LandScape::Render(bool bDrawBoundary)
{
	ASSERT(!!CellRenderer, "Shader Doesn't Created")
	
	const Frustum * const ViewFrustum = Context::Get()->GetCamera()->GetViewFrustum();
	
	CB_WVP->BindToGPU();
	CB_Light->BindToGPU();
	CB_Blending->BindToGPU();
	CB_Tessellation->BindToGPU();
	
	if (!!HeightMap)
		HeightMap->BindToGPU(0, static_cast<UINT>(ShaderType::VDP));
	if (!!VariationMap)
		VariationMap->BindToGPU(1);
	if (!!DiffuseMaps)
		DiffuseMaps->BindToGPU(2);
	if (!!NormalMaps)
		NormalMaps->BindToGPU(3);
	if (!!PerlinNoise)
		PerlinNoise->BindToGPU(4);

	int RenderingCell = 0;
	for (SceneryCell * Cell : Cells)
	{
		if (Cell->Render(CellRenderer, ViewFrustum) == true)
			RenderingCell++;
	}

	Gui::Get()->RenderText(5, 150, 1.f, 0.2f, 0.2f, String::Format("VisibleCells : %d / %d", RenderingCell, Cells.size()));
	
	if (bDrawBoundary)
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
}

void LandScape::SetupShaders()
{
	const wstring ShaderPath[2]
	{
		L"Terrain/TerrainCell.hlsl",
		L"Terrain/TerrainTessellation.hlsl"
	};
	// const string GridSizeStr = std::to_string(min(GridSize, 64));
	const string GridSizeStr = std::to_string(64);
	const vector<D3D_SHADER_MACRO> Defines = {
		{"TYPE01", "0"},
		{"MAX_TESS_FACTOR", GridSizeStr.c_str()},
		{nullptr, nullptr}
	};
	CellRenderer = new HlslShader<VertexType>(
		ShaderPath[0],
		static_cast<UINT>(ShaderType::VHDP),
		"VSMain",
		"PSMain",
		"",
		Defines.data()
	);
	CellRenderer->SetTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	CHECK(SUCCEEDED(CellRenderer->CreateSamplerState_Linear_Clamp(static_cast<UINT>(ShaderType::VDP)))); // Clamp
	CHECK(SUCCEEDED(CellRenderer->CreateSamplerState_Anisotropic_Wrap(static_cast<UINT>(ShaderType::VDP))));
	CHECK(SUCCEEDED(CellRenderer->CreateBlendState_Opaque()));
	CHECK(SUCCEEDED(CellRenderer->CreateDepthStencilState_Default()));
	CHECK(SUCCEEDED(CellRenderer->CreateRasterizerState_Solid()));
	// CHECK(SUCCEEDED(Shader->CreateRasterizerState_WireFrame()));

	CellBoundaryRenderer = new HlslShader<VertexColor>(L"Debug/Boundary.hlsl");
	CellBoundaryRenderer->SetTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	CHECK(SUCCEEDED(CellRenderer->CreateBlendState_Opaque()));
}

void LandScape::SetupResources(const LandScapeDesc& InDesc)
{
	VariationMap = new Texture(L"Terrain/T_MacroVariation.png", true);
	PerlinNoise = new Texture(L"Terrain/T_Perlin_Noise.png", true);

	if (!InDesc.HeightMapName.empty())
	{
		HeightMap = new Texture(InDesc.HeightMapName, true);
	}
	if (!InDesc.DiffuseMaps.empty())
	{
		DiffuseMaps = new TextureArray(InDesc.DiffuseMaps, 1024, 1024, 5);
		TessellationData.DiffuseMapCount = InDesc.DiffuseMaps.size();
	}
	if (!InDesc.NormalMaps.empty())
	{
		NormalMaps = new TextureArray(InDesc.NormalMaps, 1024, 1024, 5);
		TessellationData.NormalMapCount = InDesc.NormalMaps.size();
	}
	// NormalMap = new RWTexture2D(HeightMap->GetWidth(), HeightMap->GetHeight());

	Tf = new Transform();

	TessellationData.HeightScaler = Extent.Y;
	TessellationData.DiffuseMapCount = InDesc.DiffuseMaps.size();
	TessellationData.NormalMapCount = InDesc.NormalMaps.size();
	TessellationData.LODRange = {1, 3};
	TessellationData.TexelSize.X = 1.f / static_cast<float>(HeightMap->GetWidth());
	TessellationData.TexelSize.Y = 1.f / static_cast<float>(HeightMap->GetHeight());
	TessellationData.TerrainSize = Extent.X;
	TessellationData.GridSize = static_cast<float>(GridSize);
	TessellationData.TextureSize = static_cast<float>(HeightMap->GetWidth());

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

void LandScape::SetupCells()
{
	const float HeightScale = Extent.Y;
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
	vector<Color> HeightMapValues;
	Vector2D VertexNum = Vector2D(1.f + Extent.X / static_cast<float>(GridSize), 1.f + Extent.Z / static_cast<float>(GridSize)); 
	HeightMap->ExtractTextureColors(HeightMapValues, VertexNum);
	const UINT Width = static_cast<UINT>(ceil(Extent.X / static_cast<float>(CellSize)));
	const UINT Height = static_cast<UINT>(ceil(Extent.Z / static_cast<float>(CellSize)));
	Cells.clear();
	CellLocalTransform.clear();
	for (UINT h = 0; h < Height; h++)
	{
		for (UINT w = 0; w < Width; w++)
		{
			SceneryCell * NewCell = new SceneryCell (
				Vector(Size, HeightScale, Size),
				Vector2D(static_cast<float>(w), static_cast<float>(h)),
				GridSize,
				HeightMapValues,
				Extent
			);
			Cells.push_back(NewCell);
			Transform CellLocalTf;
			CellLocalTf.SetScale(NewCell->GetExtent());
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
