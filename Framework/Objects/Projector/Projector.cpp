#include "framework.h"
#include "Projector.h"

Projector::Projector(const wstring& InTextureFile, float InWidth, float InHeight, float InNear, float InFar)
	: Width(InWidth), Height(InHeight), Near(InNear), Far(InFar), FOV(0)
{
	DecalTexture = new Texture(InTextureFile, true);
	
	World = new Transform();
	Proj = new Orthographic(InWidth, InHeight, InNear, InFar);
	
	ProjectorCBuffer_VS = new ConstantBuffer(
		ShaderType::VertexShader,
		VS_CONST_DECAL_PROJECTOR,
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
		static Vector position = World->GetWorldPosition();
		static Vector rotation = World->GetWorldZYXEulerAngleInDegree();
		ImGui::SliderFloat("PositionX", &position.X, -30, +30);
		ImGui::SliderFloat("PositionY", &position.Y, 0, 20);
		ImGui::SliderFloat("PositionZ", &position.Z, -30, +30);
		World->SetWorldPosition(position);
		ImGui::SliderFloat("Roll", &rotation.X, -90, +90);
		ImGui::SliderFloat("Pitch", &rotation.Y, -90, +90);
		ImGui::SliderFloat("Yaw", &rotation.Z, -90, +90);
		World->SetWorldRotation(rotation * Math::DegToRadian);

		ImGui::InputFloat("Width", &Width, 1.0f);
		ImGui::InputFloat("Height", &Height, 1.0f);
		ImGui::InputFloat("Near", &Near, 1.0f);
		ImGui::InputFloat("Far", &Far, 1.0f);
		ImGui::InputFloat("FOV", &FOV, 0.01f);
	}
	ImGui::EndGroup();
	Proj->Set(Width, Height, Near, Far, 0);
	ProjectorData.View = Matrix::CreateLookAt(World->GetWorldPosition(), World->GetWorldPosition() + World->GetForward(), World->GetUp());
	ProjectorData.Projection = Proj->GetMatrix();
	ProjectorCBuffer_VS->UpdateData(&ProjectorData, sizeof(ProjectorDesc));
}

void Projector::Render()
{
	ProjectorCBuffer_VS->BindToGPU();
	DecalTexture->BindToGPU(4);
}

void Projector::SetPosition(const Vector& InPosition) const
{
	World->SetWorldPosition(InPosition);
}

void Projector::SetScale(const Vector& InScale) const
{
	World->SetScale(InScale);
}

void Projector::SetRotation(const Vector& InEulerDegree) const
{
	World->SetWorldRotation(InEulerDegree);
}
