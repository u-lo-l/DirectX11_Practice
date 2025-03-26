#include "framework.h"
#include "CrossQuad.h"

CrossQuad::CrossQuad()
 : ShaderName(L"00_GS_CrossQuad.hlsl")
 , WorldTF(new Transform())
 , VertexCount(0)
 , VBuffer(nullptr)
 , CrossQuadTextures(nullptr)
{
	UINT ShaderTypeFlag = 0;
	ShaderTypeFlag |= static_cast<UINT>(::ShaderType::VertexShader);
	ShaderTypeFlag |= static_cast<UINT>(::ShaderType::PixelShader);
	ShaderTypeFlag |= static_cast<UINT>(::ShaderType::GeometryShader);
	CrossQuadShader = new ShaderType(ShaderName, ShaderTypeFlag);
	CHECK(CrossQuadShader->CreateRasterizerState_Solid_NoCull() >= 0);
	CHECK(CrossQuadShader->CreateBlendState_AlphaBlendCoverage() >= 0);

	GS_ViewProjectionBuffer = new ConstantBuffer(
		::ShaderType::GeometryShader,
		GS_ViewProjection,
		nullptr,
		"ViewProjection",
		sizeof(ViewProjectionDesc),
		false
	);
}

CrossQuad::~CrossQuad()
{
	SAFE_DELETE(CrossQuadShader);
	SAFE_DELETE(WorldTF);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(CrossQuadTextures);
}

void CrossQuad::Tick()
{
	WorldTF->Tick();
	
	ViewProjectionData.View = Context::Get()->GetViewMatrix();
	ViewProjectionData.ViewInv = Matrix::Invert(ViewProjectionData.View); 
	ViewProjectionData.Projection = Context::Get()->GetProjectionMatrix();
	GS_ViewProjectionBuffer->UpdateData(&ViewProjectionData, sizeof(ViewProjectionDesc));
	
	UpdateTextures();
	UpdateVBuffer();
}

void CrossQuad::Render() const
{
	Context::Get()->GetViewProjectionCBuffer()->BindToGPU();
	GS_ViewProjectionBuffer->BindToGPU();
	VBuffer->BindToGPU();
	WorldTF->BindToGPU();
	CrossQuadTextures->BindToGPU(PS_Billboard);

	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	CrossQuadShader->Draw(VertexCount);
}

void CrossQuad::Add(const Vector& InPosition, const Vector2D& InScale, UINT InMapIndex)
{
	Vertices.emplace_back(InPosition, InScale, InMapIndex);
}

void CrossQuad::AddTexture(const wstring& InPath)
{
	TextureNames.push_back(InPath);
}

void CrossQuad::UpdateTextures()
{
	if (TextureNames.size() > 0 && CrossQuadTextures == nullptr)
	{
		CrossQuadTextures = new TextureArray(TextureNames);
	}
}

void CrossQuad::UpdateVBuffer()
{
	if (VertexCount != Vertices.size())
	{
		VertexCount = Vertices.size();
		SAFE_DELETE(VBuffer);
		VBuffer = new VertexBuffer(Vertices.data(), VertexCount, sizeof(VertexType));
	}
}

