#include "framework.h"
#include "Projector.h"

Projector::Projector(const wstring& InTextureFile, float InWidth, float InHeight, float InNear, float InFar)
{
	DecalTexture = new Texture(InTextureFile, true);
	
	World = new Transform();
	World->SetRotation({90, 0, 0});
	Proj = new Orthographic(InWidth, InHeight, InNear, InFar);
	
	ProjectorCBuffer_VS = new ConstantBuffer(
		ShaderType::VertexShader,
		VS_CONST_DECALPROJECTOR,
		&ProjectorData,
		"Decal Projector Data",
		sizeof(ProjectorDesc),
		false
	);
}

Projector::~Projector()
{
	SAFE_DELETE(Proj);
	SAFE_DELETE(DecalTexture);
}

void Projector::Tick()
{
	ImGui::BeginGroup();
	{
		static Vector position = World->GetPosition();
		static Vector rotation = World->GetRotationInDegree();
		ImGui::SliderFloat("PositionX", &position.X, -5, +5);
		ImGui::SliderFloat("PositionY", &position.Y, 0, 20);
		ImGui::SliderFloat("PositionZ", &position.Z, -5, +5);
		World->SetPosition(position);
		ImGui::SliderFloat("Roll", &rotation.X, -90, +90);
		ImGui::SliderFloat("Pitch", &rotation.Y, -90, +90);
		ImGui::SliderFloat("Yaw", &rotation.Z, -90, +90);
		World->SetRotation(rotation);

		ImGui::InputFloat("Width", &Width, 1.0f);
		ImGui::InputFloat("Height", &Height, 1.0f);
		ImGui::InputFloat("Near", &Near, 1.0f);
		ImGui::InputFloat("Far", &Far, 1.0f);
		ImGui::InputFloat("FOV", &FOV, 0.01f);
	}
	ImGui::EndGroup();
	Proj->Set(Width, Height, Near, Far, 0);
	ProjectorData.View = Matrix::CreateLookAt(World->GetPosition(), World->GetPosition() + World->GetForward(), World->GetUp());
	ProjectorData.Projection = Proj->GetMatrix();
	ProjectorCBuffer_VS->UpdateData(&ProjectorData, sizeof(ProjectorDesc));
}

void Projector::Render()
{
	ProjectorCBuffer_VS->BindToGPU();
}
