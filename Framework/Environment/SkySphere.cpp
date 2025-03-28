#include "framework.h"
#include "SkySphere.h"

SkySphere::SkySphere(wstring InFilePath, float InRadius, UINT InSliceCount)
	: Radius(InRadius), SliceCount(InSliceCount)
{
	SkyShader = new ShaderType(L"Weather/40_SkyBox.hlsl");
	CHECK(SkyShader->CreateSamplerState_Linear() >= 0);	
	CHECK(SkyShader->CreateRasterizerState_Solid_CW() >= 0);
	CHECK(SkyShader->CreateDepthStencilState_NoDepth() >= 0);
	World = new Transform();

	CreateVertexBuffer();
	CreateIndexBuffer();

	InFilePath = W_TEXTURE_PATH + InFilePath;

	CHECK(Helper::CreateShaderResourceViewFromFile(D3D::Get()->GetDevice(), InFilePath, &SkyBoxSRV) >= 0);
}

SkySphere::~SkySphere()
{
	SAFE_RELEASE(SkyBoxSRV);

	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);

	SAFE_DELETE(World);

	SAFE_DELETE(SkyShader);
}

void SkySphere::Tick()
{
	Vector position = Context::Get()->GetCamera()->GetPosition();
	World->SetPosition(position);
	World->Tick();
}

void SkySphere::Render()
{
	ID3D11DeviceContext * Context = D3D::Get()->GetDeviceContext();
	World->BindToGPU();
	Context::Get()->GetViewProjectionCBuffer()->BindToGPU();
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();

	Context->PSSetShaderResources(PS_SkyBox, 1, &SkyBoxSRV);
	
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	SkyShader->DrawIndexed(IndexCount);
}

void SkySphere::CreateVertexBuffer()
{
	vector<VertexType> VertexData;
	VertexData.push_back(VertexType({0, Radius, 0}));

	const float Denominator = 1 / static_cast<float>(SliceCount);
	const float DeltaPhi = Math::Pi * Denominator;
	const float DeltaTheta = 2.0f * Math::Pi * Denominator;

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

			VertexData.push_back(VertexType({Point.X, Point.Y, Point.Z}));
		}
	}
	VertexData.push_back(VertexType({0, -Radius, 0}));

	VertexCount = VertexData.size();
	VBuffer = new VertexBuffer(VertexData.data(), VertexCount, sizeof(VertexType));
}

void SkySphere::CreateIndexBuffer()
{
	vector<UINT> IndexData;
	for (UINT k = 1; k <= SliceCount; k++)
	{
		IndexData.push_back(0);
		IndexData.push_back(k + 1);
		IndexData.push_back(k);
	}

	UINT baseIndex = 1;
	UINT ringVertexCount = SliceCount + 1;
	for (UINT k = 0; k < SliceCount - 2; k++)
	{
		for (UINT j = 0; j < SliceCount; j++)
		{
			IndexData.push_back(baseIndex + k * ringVertexCount + j);
			IndexData.push_back(baseIndex + k * ringVertexCount + j + 1);
			IndexData.push_back(baseIndex + (k + 1) * ringVertexCount + j);

			IndexData.push_back(baseIndex + (k + 1) * ringVertexCount + j);
			IndexData.push_back(baseIndex + k * ringVertexCount + j + 1);
			IndexData.push_back(baseIndex + (k + 1) * ringVertexCount + j + 1);
		}
	}

	const UINT SouthPoleIndex = VertexCount - 1;
	baseIndex = SouthPoleIndex - ringVertexCount;

	for (UINT k = 0; k < SliceCount; k++)
	{
		IndexData.push_back(SouthPoleIndex);
		IndexData.push_back(baseIndex + k);
		IndexData.push_back(baseIndex + k + 1);
	}
	IndexCount = IndexData.size();
	IBuffer = new IndexBuffer(IndexData.data(), IndexCount);
}
