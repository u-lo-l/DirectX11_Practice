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
	MainCamera->Tick();
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

	Gui::Get()->RenderText(5, 20, 1, 1, 1, String::Format("Camera Rotation : %3.0f, %3.0f, %3.0f", CamRot.X, CamRot.Y, CamRot.Z));
	Gui::Get()->RenderText(5, 35, 1, 1, 1, String::Format("Camera Position : %3.0f, %3.0f, %3.0f", CamPos.X, CamPos.Y, CamPos.Z));
}

void Context::ResizeScreen()
{
	const float Aspect = D3D::GetDesc().Width / D3D::GetDesc().Height;
	Projection = Matrix::CreatePerspectiveFieldOfView(Math::Pi * 0.25f, Aspect, 0.1f, 10000.f);

	this->Viewport->TopLeftX = 0;
	this->Viewport->TopLeftY = 0;
	this->Viewport->Width = D3D::GetDesc().Width;
	this->Viewport->Height = D3D::GetDesc().Height;
	this->Viewport->MinDepth = 0;
	this->Viewport->MaxDepth = 1;

	D3D::Get()->GetDeviceContext()->RSSetViewports(1, this->Viewport);
}

Context::Context()
 : MainCamera(new Camera())
{
	const float Aspect = D3D::GetDesc().Width / D3D::GetDesc().Height;
	Projection = Matrix::CreatePerspectiveFieldOfView(Math::Pi * 0.25f, Aspect, 0.1f, 10000.f);
	MainCamera->SetPosition(0, 0, -5);

	this->Viewport = new D3D11_VIEWPORT();
	this->Viewport->TopLeftX = 0;
	this->Viewport->TopLeftY = 0;
	this->Viewport->Width = D3D::GetDesc().Width;
	this->Viewport->Height = D3D::GetDesc().Height;
	this->Viewport->MinDepth = 0;
	this->Viewport->MaxDepth = 1;

	D3D::Get()->GetDeviceContext()->RSSetViewports(1, this->Viewport);
}

Context::~Context()
{
	SAFE_DELETE(Viewport);
	SAFE_DELETE(MainCamera);
}

