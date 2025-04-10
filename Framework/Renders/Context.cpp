#include "framework.h"
#include "Context.h"

Context * Context::Instance = nullptr;

void Context::Create()
{
	ASSERT(Instance == nullptr, "Context Instance must null on Create()")

	Instance = new Context();
}

void Context::Destroy()
{
	ASSERT(Instance != nullptr, "Context Instance must not null on Destroy()")

	SAFE_DELETE(Instance);
}

Context * Context::Get()
{
	ASSERT(Instance != nullptr, "Context Instance must not null on Get()")

	return Instance;
}

void Context::Tick()
{
	if (!!MainCamera)
		MainCamera->Tick();
	if (!!VP_CBuffer_VS)
		VP_CBuffer_VS->Tick();
	ImGui::SliderFloat3("LightDirection", LightDirection, -1, +1);
}

/**
 *	@brief :
 *	1. Displays information such as FPS and Camera transform using GUI.
 *	2. Binds the Constant Buffer to the GPU.
 */
void Context::Render() const
{
	Gui * const GuiInst = Gui::Get();
	GuiInst->RenderText(5, 5, 1, 1, 1,  "FrameRate : " + to_string(static_cast<int>(ImGui::GetIO().Framerate)));
	
	const Vector & CamPos = MainCamera->GetPosition();
	const Vector & CamRot = MainCamera->GetEulerAngle();
	
	Gui::Get()->RenderText(5, 20, 1, 1, 1, String::Format("Camera Rotation : %3.0f, %3.0f, %3.0f", CamRot.X* Math::RadianToDeg, CamRot.Y* Math::RadianToDeg, CamRot.Z* Math::RadianToDeg));
	Gui::Get()->RenderText(5, 35, 1, 1, 1, String::Format("Camera Position : %3.0f, %3.0f, %3.0f", CamPos.X , CamPos.Y, CamPos.Z));
}

void Context::ResizeScreen()
{
	Projection->Set(D3D::GetDesc().Width, D3D::GetDesc().Height, 0.1f, 10000.f, Math::Pi * 0.25f);
	Vp->SetViewPort(D3D::GetDesc().Width, D3D::GetDesc().Height, 0, 0, 0, 1);
}

Camera * Context::GetCamera() const
{
	return MainCamera;
}

const Vector & Context::GetLightDirection() const
{
	return LightDirection;
}

GlobalViewProjectionCBuffer * Context::GetViewProjectionCBuffer() const
{
	return VP_CBuffer_VS;
}

Context::Context()
 : MainCamera(new Camera())
{
	Vp = new ViewPort(D3D::GetDesc().Width, D3D::GetDesc().Height, 0, 0, 0, 1);
	Projection = new Perspective(D3D::GetDesc().Width, D3D::GetDesc().Height, 0.1f, 10000.f, Math::Pi * 0.25f);
	
	MainCamera->SetPosition(0, 0, 10);
	MainCamera->SetRotation(180, 0, 0);

	VP_CBuffer_VS = new GlobalViewProjectionCBuffer();
}

Context::~Context()
{
	SAFE_DELETE(Vp);
	SAFE_DELETE(MainCamera);
	SAFE_DELETE(VP_CBuffer_VS);
}

