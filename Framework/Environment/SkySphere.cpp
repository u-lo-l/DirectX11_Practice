#include "framework.h"
#include "SkySphere.h"

SkySphere::SkySphere(Desc InDesc)
: ARenderable(), Info(std::move(InDesc))
{
	Tf->SetParent(nullptr);
	SkyMaterial::MaterialDesc MatDesc;
	MatDesc.Name = "M_Sky";
	MatDesc.ShaderName = Info.ShaderName;
	MatDesc.SkyTextureName =  L"Environments/SkyDawn.dds";
	this->Mat = new SkyMaterial(MatDesc);
	ARenderable::SetName(Info.Name);
	ARenderable::SetMaterial(this->Mat);
	ARenderable::SetShader();

	CreateVertices();
	ASSERT(this->Vertices.empty() == false, "Vertices should not be empty");
	CreateIndices();
	ASSERT(this->Indices.empty() == false, "Indices should not be empty");

	ARenderable::CreateVertexBuffer(Vertices);
	ARenderable::CreateIndexBuffer(Indices);

	CB_PerElement = new ConstantBuffer(
		ShaderType::VertexShader,
		1,
		nullptr,
		sizeof(Matrix),
		false
	);

	RenderManager::Get()->AddRenderable(this);
}

SkySphere::~SkySphere()
{
	SAFE_DELETE(Mat);
}

void SkySphere::BindResources() const
{
	if (!!CB_PerElement)
		CB_PerElement->BindToGPU(ShaderType::VP, 1);
}

void SkySphere::Tick()
{
	const Vector & CamPos =Context::Get()->GetCamera()->GetPosition(); 
	Matrix CameraWorldTf = Matrix::CreateTranslation(CamPos);
	CB_PerElement->UpdateData(&CameraWorldTf, sizeof(Matrix));
}

void SkySphere::CreateVertices()
{
	Vertices.clear();
	Vertices.push_back(VertexType({0, Radius, 0}));
	const float Denominator = 1 / static_cast<float>(SliceCount);
	const float DeltaPhi = Math::PI * Denominator;
	const float DeltaTheta = 2.0f * Math::PI * Denominator;
	for (UINT i = 1; i <= SliceCount - 1; i++)
	{
		float Phi = static_cast<float>(i) * DeltaPhi;

		for (UINT k = 0; k <= SliceCount; k++)
		{
			float Theta = static_cast<float>(k) * DeltaTheta;

			Vector Point {
				(Radius * sinf(Phi) * cosf(Theta)),
				(Radius * cos(Phi)),
				(Radius * sinf(Phi) * sinf(Theta))
			};
			Vertices.push_back(VertexType({Point.X, Point.Y, Point.Z}));
		}
	}
	Vertices.push_back(VertexType({0, -Radius, 0}));
}

void SkySphere::CreateIndices()
{
	CHECK(Vertices.empty() == false);

	Indices.clear();
	for (UINT k = 1; k <= SliceCount; k++)
	{
		Indices.push_back(0);
		Indices.push_back(k + 1);
		Indices.push_back(k);
	}
	UINT baseIndex = 1;
	UINT ringVertexCount = SliceCount + 1;
	for (UINT k = 0; k < SliceCount - 2; k++)
	{
		for (UINT j = 0; j < SliceCount; j++)
		{
			Indices.push_back(baseIndex + k * ringVertexCount + j);
			Indices.push_back(baseIndex + k * ringVertexCount + j + 1);
			Indices.push_back(baseIndex + (k + 1) * ringVertexCount + j);

			Indices.push_back(baseIndex + (k + 1) * ringVertexCount + j);
			Indices.push_back(baseIndex + k * ringVertexCount + j + 1);
			Indices.push_back(baseIndex + (k + 1) * ringVertexCount + j + 1);
		}
	}
	const UINT SouthPoleIndex = Vertices.size() - 1;
	baseIndex = SouthPoleIndex - ringVertexCount;
	for (UINT k = 0; k < SliceCount; k++)
	{
		Indices.push_back(SouthPoleIndex);
		Indices.push_back(baseIndex + k);
		Indices.push_back(baseIndex + k + 1);
	}
}
