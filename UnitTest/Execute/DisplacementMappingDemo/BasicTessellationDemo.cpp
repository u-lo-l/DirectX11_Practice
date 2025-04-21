#include "Pch.h"
#include "BasicTessellationDemo.h"

void sdt::BasicTessellationDemo::Initialize()
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
	// Vertices.resize(8);
	// Vertices[0].Position = { -10,  10, -10 };
	// Vertices[1].Position = {  10,  10, -10 };
	// Vertices[2].Position = {  10, -10, -10 };
	// Vertices[3].Position = { -10, -10, -10 };
	// Vertices[4].Position = { -10,  10,  10 };
	// Vertices[5].Position = {  10,  10,  10 };
	// Vertices[6].Position = {  10, -10,  10 };
	// Vertices[7].Position = { -10, -10,  10 };
	//
	// // Quad 패치 인덱스 (4개 꼭짓점씩 묶음)
	// Indices = {
	// 	0, 1, 2, 3,
	// 	5, 4, 7, 6,
	// 	4, 0, 3, 7,
	// 	1, 5, 6, 2,
	// 	4, 5, 1, 0,
	// 	3, 2, 6, 7
	// };
	
	UINT TargetShader =	static_cast<UINT>(ShaderType::VertexShader) 
						| static_cast<UINT>(ShaderType::PixelShader)
						| static_cast<UINT>(ShaderType::HullShader)   
						| static_cast<UINT>(ShaderType::DomainShader);
	vector<D3D_SHADER_MACRO> Macros = { {"TRI", "0"}, {nullptr, nullptr}};
	// vector<D3D_SHADER_MACRO> Macros = { {"QUAD", "0"}, {nullptr, nullptr}};
	Shader = new HlslShader<VertexType>(
		L"Tessellation/BasicTessellation.hlsl",
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

void sdt::BasicTessellationDemo::Destroy()
{
	SAFE_DELETE(Shader);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(TessellationFactorBuffer_HS);
	SAFE_DELETE(WVPBuffer_DS);
}

void sdt::BasicTessellationDemo::Tick()
{
	WVP.View = Context::Get()->GetViewMatrix();
	WVP.Projection = Context::Get()->GetProjectionMatrix();
	WVPBuffer_DS->UpdateData(&WVP, sizeof(WVPDesc));

	ImGui::SeparatorText("Tessellation Factor");
	ImGui::SliderInt("Edge[0]",  TessellationFactor.Edge  + 0, 1, 10);
	ImGui::SliderInt("Edge[1]",  TessellationFactor.Edge  + 1, 1, 10);
	ImGui::SliderInt("Edge[2]",  TessellationFactor.Edge  + 2, 1, 10);
	ImGui::SliderInt("Edge[3]",  TessellationFactor.Edge  + 3, 1, 10);
	ImGui::SliderInt("Inner[0]", TessellationFactor.Inner + 0, 0, 10);
	ImGui::SliderInt("Inner[1]", TessellationFactor.Inner + 1, 0, 10);
	TessellationFactorBuffer_HS->UpdateData(&TessellationFactor, sizeof(TessellationFactorDesc));
}


void sdt::BasicTessellationDemo::Render()
{
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	WVPBuffer_DS->BindToGPU();
	TessellationFactorBuffer_HS->BindToGPU();
	Shader->DrawIndexed(Indices.size());
}

