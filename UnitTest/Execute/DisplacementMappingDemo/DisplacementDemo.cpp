#include "Pch.h"
#include "DisplacementDemo.h"

void sdt::DisplacementDemo::Initialize()
{
	Camera * const MainCamera = Context::Get()->GetCamera();
	MainCamera->SetRotation( -151, -254, 180);
	MainCamera->SetPosition( -40, 25, -10);

	Vertices.resize(6);
	Vertices[0].Position = {  0,  10,  0 };
	Vertices[1].Position = { -10,  0,  0 };
	Vertices[2].Position = {  0,  0, -10 };
	Vertices[3].Position = {  10,  0,  0 };
	Vertices[4].Position = {  0,  0,  10 };
	Vertices[5].Position = {  0, -10,  0 };
	
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
	vector<D3D_SHADER_MACRO> Macros = { {"TRI", "0"}, {nullptr, nullptr}};
	// vector<D3D_SHADER_MACRO> Macros = { {"QUAD", "0"}, {nullptr, nullptr}};
	Shader = new HlslShader<VertexType>(
		L"Tessellation/Displacement.hlsl",
		TargetShader,
		"VSMain",
		"PSMain",
		"",
		Macros.data()
	);
	Shader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	// Shader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	
	CHECK(SUCCEEDED(Shader->CreateRasterizerState_WireFrame()));
	
	VBuffer = new VertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexType));
	IBuffer = new IndexBuffer(Indices.data(), Indices.size());

	WVPBuffer_DS = new ConstantBuffer(
		ShaderType::DomainShader,
		0,
		&WVP,
		"World View Projection",
		sizeof(WVPDesc),
		false
	);
	
	TessellationFactorBuffer_HS = new ConstantBuffer(
		ShaderType::HullShader,
		1,
		&TessellationFactor,
		"TessellationFactor",
		sizeof(TessellationFactorDesc),
		false
	);
	WVP.World = Matrix::Identity;
}

void sdt::DisplacementDemo::Destroy()
{
	SAFE_DELETE(Shader);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(TessellationFactorBuffer_HS);
	SAFE_DELETE(WVPBuffer_DS);
}

void sdt::DisplacementDemo::Tick()
{
	WVP.View = Context::Get()->GetViewMatrix();
	WVP.Projection = Context::Get()->GetProjectionMatrix();
	WVPBuffer_DS->UpdateData(&WVP, sizeof(WVPDesc));

	ImGui::SeparatorText("Tessellation Factor");
	ImGui::SliderInt("Edge[0]",  TessellationFactor.Edge  + 0, 1, 10);
	ImGui::SliderInt("Edge[1]",  TessellationFactor.Edge  + 1, 1, 10);
	ImGui::SliderInt("Edge[2]",  TessellationFactor.Edge  + 2, 1, 10);
	// ImGui::SliderInt("Edge[3]",  TessellationFactor.Edge  + 3, 0, 10);
	ImGui::SliderInt("Inner[0]", TessellationFactor.Inner + 0, 0, 10);
	// ImGui::SliderInt("Inner[1]", TessellationFactor.Inner + 1, 0, 10);
	TessellationFactorBuffer_HS->UpdateData(&TessellationFactor, sizeof(TessellationFactorDesc));
}


void sdt::DisplacementDemo::Render()
{
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	WVPBuffer_DS->BindToGPU();
	TessellationFactorBuffer_HS->BindToGPU();
	Shader->DrawIndexed(Indices.size());
}

