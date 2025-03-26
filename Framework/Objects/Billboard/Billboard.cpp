#include "framework.h"
#include "Billboard.h"

Billboard::Billboard()
 : ShaderName(L"00_GS_Billboard.hlsl")
 , WorldTF(new Transform())
 , VertexCount(0)
 , VBuffer(nullptr)
 , BillboardTextures(nullptr)
{
	UINT ShaderTypeFlag = 0;
	ShaderTypeFlag |= static_cast<UINT>(::ShaderType::VertexShader);
	ShaderTypeFlag |= static_cast<UINT>(::ShaderType::PixelShader);
	ShaderTypeFlag |= static_cast<UINT>(::ShaderType::GeometryShader);
	BillboardShader = new ShaderType(ShaderName, ShaderTypeFlag);
	CHECK(BillboardShader->CreateRasterizerState_Solid_NoCull() >= 0);
	CHECK(BillboardShader->CreateBlendState_AlphaBlend() >= 0);

	GS_ViewProjectionBuffer = new ConstantBuffer(
		::ShaderType::GeometryShader,
		GS_ViewProjection,
		nullptr,
		"ViewProjection",
		sizeof(ViewProjectionDesc),
		false
	);
}

Billboard::~Billboard()
{
	SAFE_DELETE(BillboardShader);
	SAFE_DELETE(WorldTF);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(BillboardTextures);
}

void Billboard::Tick()
{
	WorldTF->Tick();
	
	ViewProjectionData.View = Context::Get()->GetViewMatrix();
	ViewProjectionData.ViewInv = Matrix::Invert(ViewProjectionData.View); 
	ViewProjectionData.Projection = Context::Get()->GetProjectionMatrix();
	GS_ViewProjectionBuffer->UpdateData(&ViewProjectionData, sizeof(ViewProjectionDesc));
	
	UpdateTextures();
	UpdateVBuffer();
}

void Billboard::Render() const
{
	Context::Get()->GetViewProjectionCBuffer()->BindToGPU();
	GS_ViewProjectionBuffer->BindToGPU();
	VBuffer->BindToGPU();
	WorldTF->BindToGPU();
	BillboardTextures->BindToGPU(PS_Billboard);

	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	BillboardShader->Draw(VertexCount);
}

void Billboard::Add(const Vector& InPosition, const Vector2D& InScale, UINT InMapIndex)
{
	Vertices.emplace_back(InPosition, InScale, InMapIndex);
}

void Billboard::AddTexture(const wstring& InPath)
{
	TextureNames.push_back(InPath);
}

void Billboard::UpdateTextures()
{
	if (TextureNames.size() > 0 && BillboardTextures == nullptr)
	{
		BillboardTextures = new TextureArray(TextureNames);
	}
}

void Billboard::UpdateVBuffer()
{
	if (VertexCount != Vertices.size())
	{
		VertexCount = Vertices.size();
		SAFE_DELETE(VBuffer);
		VBuffer = new VertexBuffer(Vertices.data(), VertexCount, sizeof(VertexType));
	}
}