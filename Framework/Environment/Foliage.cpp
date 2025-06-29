#include "framework.h"
#include "Foliage.h"

#include "Material/FoliageMaterial.h"

Foliage::Foliage(const FoliageDesc& InDesc)
	: ARenderable(), Info(InDesc), CellSize(InDesc.TargetTerrain->GetCellSize())
{
	FoliageMaterial::MaterialDesc Desc = {
		InDesc.MaterialName,
		InDesc.ShaderName,
		InDesc.DensityMapName,
		InDesc.Textures
	};
	Tf->SetParent(InDesc.TargetTerrain->GetTransform());
	ARenderable::SetName(InDesc.Name);
	ARenderable::SetMaterial(new FoliageMaterial(Desc));
	ARenderable::SetShader();

	Vector TerrainDimension = InDesc.TargetTerrain->GetDimension();

	CreateVertices(TerrainDimension.X, TerrainDimension.Z, this->CellSize);
	ASSERT(this->Vertices.empty() == false, "Vertices should not be empty");

	ARenderable::CreateVertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexType));

	NormalMap = Info.TargetTerrain->GetNormalMap();

	CB_PerFoliageData.TerrainBaseWorldTF = Tf->GetWorldMatrix();

	CB_PerFoliageData.HeightScaler = Info.TargetTerrain->GetHeightScaler();
	CB_PerFoliageData.DistanceRange = {50, 250, 1000};
	
	CB_PerFoliageData.AltitudeRange = InDesc.AltitudeRange;
	CB_PerFoliageData.TerrainMapSize = {Info.TargetTerrain->GetDimension().X, Info.TargetTerrain->GetDimension().Z};
	CB_PerFoliageData.TerrainMapSize.X *= Tf->GetScale().X;
	CB_PerFoliageData.TerrainMapSize.Y *= Tf->GetScale().Z;
	
	CB_PerFoliage = new ConstantBuffer(
		ShaderType::VGP,
		2,
		nullptr,
		sizeof(CB_PerFoliageData),
		false
	);
	RenderManager::Get()->AddRenderable(this);
}

Foliage::~Foliage()
{
	SAFE_DELETE(CB_PerFoliage);
}

void Foliage::BindResources() const
{
	Info.TargetTerrain->GetHeightMap()->BindToGPU(0, ShaderType::VP);
	NormalMap->BindToGPUAsSRV(1, ShaderType::GP);
	CB_PerFoliage->BindToGPU(ShaderType::VGP, 2);
}

void Foliage::Tick()
{
	ImGui::Begin("Foliage");
	ImGui::SliderFloat("Near", &CB_PerFoliageData.DistanceRange.X, 0, CB_PerFoliageData.DistanceRange.Y - 1, "%.0f");
	ImGui::SliderFloat("Mid ", &CB_PerFoliageData.DistanceRange.Y, CB_PerFoliageData.DistanceRange.X, CB_PerFoliageData.DistanceRange.Z - 1, "%.0f");
	ImGui::SliderFloat("Far ", &CB_PerFoliageData.DistanceRange.Z, CB_PerFoliageData.DistanceRange.Y, 1500, "%.0f");

	ImGui::SliderFloat("LOW ", &CB_PerFoliageData.AltitudeRange.X, 0, CB_PerFoliageData.DistanceRange.Y, "%.2f", ImGuiSliderFlags_Logarithmic);
	ImGui::SliderFloat("HIGH", &CB_PerFoliageData.AltitudeRange.Y, CB_PerFoliageData.DistanceRange.X, 2000, "%.0f", ImGuiSliderFlags_Logarithmic);

	ImGui::End();
	CB_PerFoliage->UpdateData(&CB_PerFoliageData, sizeof(CB_PerFoliageDesc));
}

void Foliage::CreateVertices(float TerrainDimensionX, float TerrainDimensionZ, UINT CellSize)
{
	// TODO : PerlinNoiseMap읽어서 Vertex생성하기.
	// 소수 단위 stride 지원
	const UINT HorizontalCount = static_cast<UINT>(floor(TerrainDimensionX / Info.Stride)) + 1;
	const UINT VerticalCount = static_cast<UINT>(floor(TerrainDimensionZ / Info.Stride)) + 1;

	Vertices.reserve(VerticalCount * HorizontalCount);
	for (UINT W = 0; W < HorizontalCount ; W++)
	{
		for (UINT H = 0; H < VerticalCount ; H++)
		{
			Vector Random = { Math::Random(-0.5f, 0.5f), 0 , Math::Random(-0.5f, 0.5f)};
			Vector Pos = {static_cast<float>(W), 0, static_cast<float>(H)};
			Vector2D Scale = Info.Scaler * Vector2D(Math::Random(0.8f, 1.2f), Math::Random(0.8f, 1.2f));
			int TextureIndex = Math::Random(0, Info.Textures.size());

			Vector FoliagePosition = Pos * Info.Stride + Vector(Scale.X * Random.X, 0.f, Scale.Y * Random.Z);
			Vertices.emplace_back(FoliagePosition, Scale, TextureIndex, Math::Random(0.f, 1.f));
		}
	}
}

