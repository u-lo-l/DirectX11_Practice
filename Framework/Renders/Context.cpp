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
	if (!!ShadowMap)
		ShadowMap->Tick();
	ImGui::SliderFloat3("LightDirection", LightDirection, -1, +1);
	LightDirection.Y = -abs(LightDirection.Y);
	ImGui::ColorEdit4("LightColor", LightColor);
}
/**
 *	@brief :
 *	1. Displays information such as FPS and Camera transform using GUI.
 *	2. Binds the Constant Buffer to the GPU.
 */
void Context::Render() const
{
	const int Fps = static_cast<int>(ImGui::GetIO().Framerate);
	
	Vp->SetViewPort(D3D::GetDesc().Width, D3D::GetDesc().Height, 0, 0, 0, 1);
	Gui * const GuiInst = Gui::Get();
	GuiInst->RenderText(5, 5, 1, 1, 1,  "FrameRate : " + to_string(Fps));
	
	const Vector & CamPos = MainCamera->GetPosition();
	const Vector & CamRot = MainCamera->GetEulerAngleInDegree();
	
	Gui::Get()->RenderText(5, 35, 1, 1, 1, String::Format("Camera Position : %3.0f, %3.0f, %3.0f", CamPos.X , CamPos.Y, CamPos.Z));
	Gui::Get()->RenderText(5, 20, 1, 1, 1, String::Format("Camera Rotation : %3.0f, %3.0f, %3.0f", CamRot.X, CamRot.Y, CamRot.Z));

	const Frustum * ViewFrustum = MainCamera->GetViewFrustum();
	const array<Plane, 6> & Planes = ViewFrustum->GetPlanes();
	float a, b, c, d;
	Planes[0].GetEquation(a,b,c,d);
	Gui::Get()->RenderText(5,  70, 1, 1, 0.5f, String::Format("ViewFrustum Near   : %3.0f, %3.0f, %3.0f, %3.0f", a,b,c,d));
	Planes[1].GetEquation(a,b,c,d);
	Gui::Get()->RenderText(5,  80, 1, 1, 0.5f, String::Format("ViewFrustum Far    : %3.0f, %3.0f, %3.0f, %3.0f", a,b,c,d));
	Planes[2].GetEquation(a,b,c,d);
	Gui::Get()->RenderText(5,  90, 1, 1, 0.5f, String::Format("ViewFrustum Left   : %3.0f, %3.0f, %3.0f, %3.0f", a,b,c,d));
	Planes[3].GetEquation(a,b,c,d);
	Gui::Get()->RenderText(5, 100, 1, 1, 0.5f, String::Format("ViewFrustum Right  : %3.0f, %3.0f, %3.0f, %3.0f", a,b,c,d));
	Planes[4].GetEquation(a,b,c,d);
	Gui::Get()->RenderText(5, 110, 1, 1, 0.5f, String::Format("ViewFrustum Top    : %3.0f, %3.0f, %3.0f, %3.0f", a,b,c,d));
	Planes[5].GetEquation(a,b,c,d);
	Gui::Get()->RenderText(5, 120, 1, 1, 0.5f, String::Format("ViewFrustum Bottom : %3.0f, %3.0f, %3.0f, %3.0f", a,b,c,d));
}

void Context::ResizeScreen()
{
	const Projection * const Proj = MainCamera->GetProjection();
	MainCamera->SetPerspective(D3D::GetDesc().Width, D3D::GetDesc().Height, Proj->GetNear(), Proj->GetFar(), Proj->GetFOV());
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

const Color& Context::GetLightColor() const
{
	return LightColor;
}

Context::Context()
 : MainCamera(new Camera())
{
	MainCamera->SetPerspective(D3D::GetDesc().Width, D3D::GetDesc().Height, 0.1f, 5000.f, Math::ToRadians(60.f));
	Vp = new ViewPort(D3D::GetDesc().Width, D3D::GetDesc().Height, 0, 0, 0, 1);
	VP_CBuffer_VS = new GlobalViewProjectionCBuffer();
	ShadowMap = new Shadow({0,0,0}, 100, 1024, 1024);
}

Context::~Context()
{
	SAFE_DELETE(Vp);
	SAFE_DELETE(MainCamera);
	SAFE_DELETE(VP_CBuffer_VS);
}

Shadow* Context::GetShadowMap() const
{
	return ShadowMap;
}

Matrix Context::GetViewMatrix() const
{
	return MainCamera->GetViewMatrix();
}

Matrix Context::GetProjectionMatrix() const
{
	return MainCamera->GetProjectionMatrix();
}



