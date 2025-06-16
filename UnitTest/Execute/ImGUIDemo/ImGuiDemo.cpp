#include "Pch.h"
#include "ImGuiDemo.h"

#define ImTextureId ID3D11ShaderResourceView*

void ImGuiDemo::Initialize()
{
	T = new Texture(L"Gaussian Random Complex HSV.png", true);
	Shader = new Hlsl2DTextureShader(T->GetSRV());
	float TextureWidth = 0.2f * D3D::GetDesc().WindowHeight;
	float TextureHeight = TextureWidth;
	const Vector2D Center = {D3D::GetDesc().WindowWidth * 0.5f, D3D::GetDesc().WindowHeight * 0.5f};
	Shader->GetTransform()->SetScale({TextureWidth, TextureHeight, 1});
	Shader->GetTransform()->SetWorldPosition({Center.X,Center.Y, 0});

	ImGuiRT = new RenderTarget(300, 300);
	ImGuiDS = new DepthStencil(300, 300, false);
}

void ImGuiDemo::Destroy()
{
	SAFE_DELETE(T);
}

void ImGuiDemo::Tick()
{
	Shader->Tick();
}

void ImGuiDemo::Render()
{
	ImGui::Begin("ImGuiDemo");
	ImGui::Text("Demo Text");
		ImGui::BeginChild(
			"Child1",
			ImVec2(-1, 200),
			ImGuiChildFlags_NavFlattened | ImGuiChildFlags_Borders
		);
		ImGui::Text("Hello World");
			ImGui::BeginChild("Child1.2", ImVec2(-1, -1), true);
				ImGui::Text("Hello World 2");
			ImGui::EndChild();
		ImGui::EndChild();
		ImGui::BeginChild("Child2", ImVec2(-1, -1), true);
			ImGui::Text("Hello World 3");
		ImGui::EndChild();
	ImGui::End();
	Shader->Render();
}

void ImGuiDemo::PostRender()
{

}
