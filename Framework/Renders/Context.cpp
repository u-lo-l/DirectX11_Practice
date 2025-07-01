#include "framework.h"
#include "Context.h"

#include <numeric>

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
	if (!!ShadowMap)
		ShadowMap->Tick();
	
#ifdef DISPLAY_IMGUI_DEBUG_INFO
	ImGui::Begin("Directional Light Setting");
	ImGui::SliderFloat3("LightDirection", LightDirection, -1, +1);
	LightDirection.Y = -abs(LightDirection.Y);
	ImGui::ColorEdit4("LightColor", LightColor);
	ImGui::End();
#endif 
}
/**
 *	@brief :
 *	1. Displays information such as FPS and Camera transform using GUI.
 *	2. Binds the Constant Buffer to the GPU.
 */
void Context::Render() const
{
	// Vp->SetViewPort(D3D::GetDesc().WindowWidth, D3D::GetDesc().WindowHeight, 0, 0, 0, 1);
// #ifdef DISPLAY_IMGUI_DEBUG_INFO
	ImGui::Begin("Debug");
		const int Fps = static_cast<int>(ImGui::GetIO().Framerate);
		ImGui::TextColored({255, 255, 255, 255},  "FrameRate : %d", Fps);
		ImGui::TextColored({255, 255, 255, 255},  "Resolution : %d x %d", (int)D3D::GetDesc().WindowWidth, (int)D3D::GetDesc().WindowHeight);
	ImGui::End();
// #endif
#ifdef DISPLAY_IMGUI_DEBUG_INFO
	ImGui::Begin("Camera Settings");
		const Vector & CamPos = MainCamera->GetPosition();
		const Vector & CamRot = MainCamera->GetEulerAngleInDegree();
		ImGui::TextColored({255, 255, 255, 255}, "Camera Rotation : %3.0f, %3.0f, %3.0f", CamRot.X, CamRot.Y, CamRot.Z);
		ImGui::TextColored({255, 255, 255, 255}, "Camera Position : %3.0f, %3.0f, %3.0f", CamPos.X , CamPos.Y, CamPos.Z);
		const Vector CamForward = MainCamera->GetForward();
		const Vector CamRight = MainCamera->GetRight();
		const Vector CamUp = MainCamera->GetUp();
		
		ImGui::TextColored({255, 200, 200, 255}, "Camera Forward : %.3f, %.3f, %.3f", CamForward.X , CamForward.Y, CamForward.Z);
		ImGui::TextColored({255, 200, 200, 255}, "Camera Right   : %.3f, %.3f, %.3f", CamRight.X , CamRight.Y, CamRight.Z);
		ImGui::TextColored({255, 200, 200, 255}, "Camera Up      : %.3f, %.3f, %.3f", CamUp.X , CamUp.Y, CamUp.Z);
	
		const Frustum * ViewFrustum = MainCamera->GetViewFrustum();
		const array<Plane, 6> & Planes = ViewFrustum->GetPlanes();
		float a, b, c, d;
		Planes[0].GetEquation(a,b,c,d);
		ImGui::TextColored({255, 255, 122, 255}, "ViewFrustum Near   : %+4.3f, %+4.3f, %+4.3f, %+4.3f", a,b,c,d);
		Planes[1].GetEquation(a,b,c,d);
		ImGui::TextColored({255, 255, 122, 255}, "ViewFrustum Far    : %+4.3f, %+4.3f, %+4.3f, %+4.3f", a,b,c,d);
		Planes[2].GetEquation(a,b,c,d);
		ImGui::TextColored({255, 255, 122, 255}, "ViewFrustum Left   : %+4.3f, %+4.3f, %+4.3f, %+4.3f", a,b,c,d);
		Planes[3].GetEquation(a,b,c,d);
		ImGui::TextColored({255, 255, 122, 255}, "ViewFrustum Right  : %+4.3f, %+4.3f, %+4.3f, %+4.3f", a,b,c,d);
		Planes[4].GetEquation(a,b,c,d);
		ImGui::TextColored({255, 255, 122, 255}, "ViewFrustum Top    : %+4.3f, %+4.3f, %+4.3f, %+4.3f", a,b,c,d);
		Planes[5].GetEquation(a,b,c,d);
		ImGui::TextColored({255, 255, 122, 255}, "ViewFrustum Bottom : %+4.3f, %+4.3f, %+4.3f, %+4.3f", a,b,c,d);
	ImGui::End();
#endif
}

void Context::ResizeScreen()
{
	const Projection * const Proj = MainCamera->GetProjection();
	MainCamera->SetPerspective(D3D::GetDesc().WindowWidth, D3D::GetDesc().WindowHeight, Proj->GetNear(), Proj->GetFar(), Proj->GetFOV());
	RenderManager::Get()->SetViewPort(D3D::GetDesc().WindowWidth, D3D::GetDesc().WindowHeight, 0, 0, 0, 1);
}

Camera * Context::GetCamera() const
{
	return MainCamera;
}

const Vector & Context::GetLightDirection() const
{
	return LightDirection;
}

const Color& Context::GetLightColor() const
{
	return LightColor;
}

Context::Context()
 : MainCamera(new Camera())
{
	MainCamera->SetPerspective(D3D::GetDesc().WindowWidth, D3D::GetDesc().WindowHeight, 0.1f, 5000.f, Math::ToRadians(60.f));
	ShadowMap = new Shadow({0,0,0}, 100, 1024, 1024);
}

Context::~Context()
{
	SAFE_DELETE(MainCamera);
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



