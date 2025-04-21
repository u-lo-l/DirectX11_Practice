#include "Pch.h"
#include "PNTriangleDemo.h"

void sdt::PNTriangleDemo::Initialize()
{
	Camera * const MainCamera = Context::Get()->GetCamera();
	MainCamera->SetRotation( -151, -254, 180);
	MainCamera->SetPosition( -40, 25, -10);

	Vertices.resize(6);
	Vertices[0].Position = {  0,  10,  0 }; Vertices[0].Normal = {  0,  +1,  0 };
	Vertices[1].Position = { -10,  0,  0 }; Vertices[1].Normal = {  -1,  0,  0 };
	Vertices[2].Position = {  0,  0, -10 }; Vertices[2].Normal = {  0,  0,  -1 };
	Vertices[3].Position = {  10,  0,  0 }; Vertices[3].Normal = {  +1,  0,  0 };
	Vertices[4].Position = {  0,  0,  10 }; Vertices[4].Normal = {  0,  0,  +1 };
	Vertices[5].Position = {  0, -10,  0 }; Vertices[5].Normal = {  0,  -1,  0 };
	
	Indices = {
		0, 2, 1,
		0, 3, 2,
		0, 4, 3,
		0, 1, 4,
		
		5, 1, 2,
		5, 2, 3,
		5, 3, 4,
		5, 4, 1 
	};

	UINT TargetShader =	static_cast<UINT>(ShaderType::VertexShader) 
						| static_cast<UINT>(ShaderType::PixelShader)
						| static_cast<UINT>(ShaderType::HullShader)   
						| static_cast<UINT>(ShaderType::DomainShader);
	Shader = new HlslShader<VertexType>(
		L"Tessellation/PNAENTriangle.hlsl",
		TargetShader
	);
	Shader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	CHECK(SUCCEEDED(Shader->CreateRasterizerState_WireFrame()));
	
	VBuffer = new VertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexType));
	IBuffer = new IndexBuffer(Indices.data(), Indices.size());

	WVPBuffer_DS = new ConstantBuffer(
		(UINT)ShaderType::DomainShader,
		0,
		&WVP,
		"World View Projection",
		sizeof(WVPDesc),
		false
	);
	
	TessellationFactorBuffer_HS = new ConstantBuffer(
		(UINT)ShaderType::HullShader,
		1,
		&TessellationFactor,
		"TessellationFactor",
		sizeof(TessellationFactorDesc),
		false
	);
	WVP.World = Matrix::Identity;
}

void sdt::PNTriangleDemo::Destroy()
{
	SAFE_DELETE(Shader);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(TessellationFactorBuffer_HS);
	SAFE_DELETE(WVPBuffer_DS);
}

void sdt::PNTriangleDemo::Tick()
{
	WVP.View = Context::Get()->GetViewMatrix();
	WVP.Projection = Context::Get()->GetProjectionMatrix();
	WVPBuffer_DS->UpdateData(&WVP, sizeof(WVPDesc));

	ImGui::SeparatorText("Tessellation Factor");
	ImGui::SliderInt("Tessellation Factor", TessellationFactor.Edge, 1, 32);
	TessellationFactorBuffer_HS->UpdateData(&TessellationFactor, sizeof(TessellationFactorDesc));
}


void sdt::PNTriangleDemo::Render()
{
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	WVPBuffer_DS->BindToGPU();
	TessellationFactorBuffer_HS->BindToGPU();
	Shader->DrawIndexed(Indices.size());
}

