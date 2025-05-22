#include "framework.h"
#include "Foliage.h"

Foliage::Foliage(const LandScape * InTerrain, const float MinAltitude, const float MaxAltitude)
	: ShaderName(L"Terrain/Foliage.hlsl")
	, VertexCount(0)
	, VBuffer(nullptr)
	, CrossQuadTextures(nullptr)
	, Terrain(InTerrain)
	, TerrainHeightCBuffer(nullptr)
{
	FoliageStride = 1.f;
	ASSERT(InTerrain, "Height map must not be null");
	DensityDistanceData.MinAltitude = MinAltitude;
	DensityDistanceData.MaxAltitude = MaxAltitude;

	// const vector<D3D_SHADER_MACRO> Macros{
	// 	{"TRIANGLE", "0"},
	// 	{nullptr,}
	// };
	CrossQuadShader = new HlslShader<VertexType>(
		ShaderName,
		static_cast<UINT>(ShaderType::VGP),
		"VSMain",
		"PSMain",
		"GSMain",
		nullptr
	);
	CrossQuadShader->SetTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	CHECK(SUCCEEDED(CrossQuadShader->CreateRasterizerState_Solid_NoCull()));
	CHECK(SUCCEEDED(CrossQuadShader->CreateBlendState_AlphaBlendCoverage()));
	CHECK(SUCCEEDED(CrossQuadShader->CreateDepthStencilState_Default()));

	WVPBuffer = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VG),
		0,
		nullptr,
		"World, View, Projection",
		sizeof(WVPDesc),
		false
	);
	TerrainHeightCBuffer = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VP),
		1,
		nullptr,
		"TerrainHeight",
		sizeof(TerrainHeightDesc),
		false
	);
	DensityDistanceCBuffer = new ConstantBuffer(
		static_cast<UINT>(ShaderType::VGP),
		2,
		nullptr,
		"DensityDistance",
		sizeof(DensityDistanceDesc),
		false
	);

	TerrainHeightData.TerrainSize = { Terrain->GetExtent().X, Terrain->GetExtent().Z };
	const Texture * HeightMap = Terrain->GetHeightMap();
	TerrainHeightData.TexelSize = {1.f / static_cast<float>(HeightMap->GetWidth()), 1.f / static_cast<float>(HeightMap->GetHeight()) }; 

	DensityDistanceData.Near = 75.f;
	DensityDistanceData.Far = 300.f;
	
	AddTexture(L"Terrain/Foliage/grass_07.tga");
	AddTexture(L"Terrain/Foliage/grass_11.tga");
	AddTexture(L"Terrain/Foliage/grass_14.tga");
	AddTexture(L"Terrain/Foliage/grass_01.tga");
	AddTexture(L"Terrain/Foliage/grass_02.tga");
	AddTexture(L"Terrain/Foliage/grass_03.tga");
	AddTexture(L"Terrain/Foliage/grass_04.tga");
	CreateRandomFoliage();
}

Foliage::~Foliage()
{
	SAFE_DELETE(CrossQuadShader);
	SAFE_DELETE(CrossQuadTextures);
	SAFE_DELETE(WVPBuffer);
}

void Foliage::Tick()
{
	WVPData.World = Matrix::Identity;
	WVPData.View = Context::Get()->GetViewMatrix();
	WVPData.Projection = Context::Get()->GetProjectionMatrix();
	WVPBuffer->UpdateData(&WVPData, sizeof(WVPDesc));

	TerrainHeightData.HeightScaler = Terrain->GetExtent().Y;
	TerrainHeightData.LightDirection = Context::Get()->GetLightDirection();
	TerrainHeightData.LightColor = Context::Get()->GetLightColor();

	TerrainHeightCBuffer->UpdateData(&TerrainHeightData, sizeof(TerrainHeightDesc));

	ImGui::SliderFloat("DensityNear", &DensityDistanceData.Near, 1.f, DensityDistanceData.Far * 0.5f, "%.0f");
	ImGui::SliderFloat("DensityFar", &DensityDistanceData.Far, DensityDistanceData.Near + 1, 1000.f, "%.0f");
	DensityDistanceData.CameraPosition = Context::Get()->GetCamera()->GetPosition();
	DensityDistanceCBuffer->UpdateData(&DensityDistanceData, sizeof(DensityDistanceDesc));
}

void Foliage::Render() const
{
	if (!CrossQuadShader) return;

	if (!!VBuffer) VBuffer->BindToGPU();
	
	if (!!WVPBuffer) WVPBuffer->BindToGPU();
	if (!!TerrainHeightCBuffer) TerrainHeightCBuffer->BindToGPU();
	if (!!DensityDistanceCBuffer) DensityDistanceCBuffer->BindToGPU();

	if (!!Terrain) Terrain->GetHeightMap()->BindToGPU(0, static_cast<UINT>(ShaderType::VP));
	if (!!CrossQuadTextures) CrossQuadTextures->BindToGPU(1); //PS
	
	CrossQuadShader->Draw(Vertices.size());
}

void Foliage::Add(const vector<VertexType> & InVertices)
{
	Vertices.insert(Vertices.end(), InVertices.begin(), InVertices.end());
	UpdateVBuffer();
}

void Foliage::Add(const Vector& InPosition, const Vector2D& InScale, UINT InMapIndex)
{
	Vertices.emplace_back(InPosition, InScale, InMapIndex);
	UpdateVBuffer();
}

void Foliage::AddTexture(const wstring& InPath)
{
	TextureNames.push_back(InPath);
	SAFE_DELETE(CrossQuadTextures);
	CrossQuadTextures = new TextureArray(TextureNames);
}

void Foliage::CreateRandomFoliage()
{
	if (TextureNames.empty() == true)
		return ;
	if (!Terrain || !Terrain->GetHeightMap())
		return;
	const float TerrainHeightScaler = Terrain->GetExtent().Y;
	const Texture * HeightMap = Terrain->GetHeightMap();
	
	// UINT HorizontalCount = Terrain->GetWidth() / FoliageStride + 1;
	// UINT VerticalCount = Terrain->GetHeight() / FoliageStride + 1;

	const float TerrainWidth = Terrain->GetExtent().X;
	const float TerrainHeight = Terrain->GetExtent().Z;

	// 소수 단위 stride 지원
	const UINT HorizontalCount = static_cast<UINT>(floor(TerrainWidth / FoliageStride)) + 1;
	const UINT VerticalCount = static_cast<UINT>(floor(TerrainHeight / FoliageStride)) + 1;
	
	for (UINT W = 0; W < HorizontalCount ; W++)
	{
		for (UINT H = 0; H < VerticalCount ; H++)
		{
			Vector Random = { Math::Random(-0.5f, 0.5f), 0 , Math::Random(-0.5f, 0.5f)};
			Vector Pos = {static_cast<float>(W), 0, static_cast<float>(H)};
			Vector2D Scale = {Math::Random(0.8f, 1.2f), Math::Random(0.8f, 1.2f)};
			int TextureIndex = Math::Random(0, TextureNames.size());
			Vertices.emplace_back((Pos + Random) * FoliageStride, Scale, TextureIndex);
		}
	}
	UpdateVBuffer();
}

void Foliage::SetTerrainHeightScaler(float InTerrainHeightScaler)
{
	TerrainHeightData.HeightScaler = InTerrainHeightScaler;
}

void Foliage::UpdateVBuffer()
{
	if (VertexCount != Vertices.size())
	{
		VertexCount = Vertices.size();
		SAFE_DELETE(VBuffer);
		VBuffer = new VertexBuffer(Vertices.data(), VertexCount, sizeof(VertexType));
	}
}
