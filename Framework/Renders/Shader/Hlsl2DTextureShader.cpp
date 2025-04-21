#include "framework.h"
#include "Hlsl2DTextureShader.h"

Hlsl2DTextureShader::Hlsl2DTextureShader(ID3D11ShaderResourceView * InSRV)
	: Shader(nullptr), World(nullptr), VBuffer(nullptr), SRV(InSRV)
{
	Data.View = Matrix::CreateLookAt(Vector(0, 0, -1), Vector::Zero, Vector::Up);
	Data.Projection = Matrix::CreateOrthographicOffCenter(0, D3D::GetDesc().Width, 0, D3D::GetDesc().Height, -1, +1);
	
	Shader = new HlslShader<VertexType>(L"2D/Renderer2D.hlsl");
	CHECK(SUCCEEDED(Shader->CreateRasterizerState_Solid_NoCull()));
	CHECK(SUCCEEDED(Shader->CreateSamplerState_Linear()));
	CHECK(SUCCEEDED(Shader->CreateDepthStencilState_NoDepth()) );

	World = new Transform();

	CBuffer = new ConstantBuffer(
		(UINT)ShaderType::VertexShader,
		Const_ViewProjection_VS,
		&Data,
		"VP",
		sizeof(Desc),
		false
	);

	VertexTexture vertices[6];
	
	vertices[0].Position = {-1.f, -1.f, 0.0f};
	vertices[1].Position = {-1.f, +1.f, 0.0f};
	vertices[2].Position = {+1.f, -1.f, 0.0f};
	vertices[3].Position = {+1.f, -1.f, 0.0f};
	vertices[4].Position = {-1.f, +1.f, 0.0f};
	vertices[5].Position = {+1.f, +1.f, 0.0f};

	vertices[0].UV = {0, 1};
	vertices[1].UV = {0, 0};
	vertices[2].UV = {1, 1};
	vertices[3].UV = {1, 1};
	vertices[4].UV = {0, 0};
	vertices[5].UV = {1, 0};

	VBuffer = new VertexBuffer(vertices, 6, sizeof(VertexType));
}

Hlsl2DTextureShader::~Hlsl2DTextureShader()
{
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(World);
	SAFE_DELETE(Shader);
}

void Hlsl2DTextureShader::Tick()
{
	World->Tick();
	CBuffer->UpdateData(&Data, sizeof(Desc));
}

void Hlsl2DTextureShader::Render()
{
	D3D::Get()->GetDeviceContext()->PSSetShaderResources(Texture_Texture_PS, 1, &this->SRV);
	World->BindToGPU();
	VBuffer->BindToGPU();
	CBuffer->BindToGPU();

	Shader->Draw(6);
}

void Hlsl2DTextureShader::SetSRV(ID3D11ShaderResourceView* InSRV)
{
	SRV = InSRV;
}
