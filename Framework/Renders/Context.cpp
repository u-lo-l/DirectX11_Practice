#include "framework.h"
#include "Context.h"

Context * Context::Instance = nullptr;

void Context::Create()
{
	ASSERT(Instance == nullptr, "Context Instance must null on Create()");

	Instance = new Context();
}

void Context::Destroy()
{
	ASSERT(Instance != nullptr, "Context Instance must not null on Destroy()");

	SAFE_DELETE(Instance);
}

Context * Context::Get()
{
	ASSERT(Instance != nullptr, "Context Instance must not null on Get()");

	return Instance;
}

void Context::Tick()
{
	MainCamera->Tick();
}

void Context::Render() const
{
	D3D::Get()->GetDeviceContext()->RSSetViewports(1, this->Viewport);

	string Str = string("FrameRate : ") + to_string(static_cast<int>(ImGui::GetIO().Framerate));
	Gui::Get()->RenderText(5, 5, 1, 1, 1, Str);

	Vector CamPos = MainCamera->GetPosition();
	Vector CamRot = MainCamera->GetEulerAngle();
	Str = "Camera Rotation : " + String::ToString(CamRot.ToString());
	Gui::Get()->RenderText(5, 20, 1, 1, 1, Str);
	
	Str = "Camera Position : " + String::ToString(CamPos.ToString());
	Gui::Get()->RenderText(5, 35, 1, 1, 1, Str);
}

void Context::ResizeScreen()
{
	const float Aspect = D3D::GetDesc().Width / D3D::GetDesc().Height;
	ProjectionMat = Matrix::CreatePerspectiveFieldOfView(Math::Pi * 0.25f, Aspect, 0.1f, 1000.f);
	MainCamera->SetPosition(0, 0, -5);

	this->Viewport->TopLeftX = 0;
	this->Viewport->TopLeftY = 0;
	this->Viewport->Width = D3D::GetDesc().Width;
	this->Viewport->Height = D3D::GetDesc().Height;
	this->Viewport->MinDepth = 0;
	this->Viewport->MaxDepth = 1;
}

Context::Context()
 : Viewport()
{
	MainCamera = new Camera();
	
	const float Aspect = D3D::GetDesc().Width / D3D::GetDesc().Height;
	ProjectionMat = Matrix::CreatePerspectiveFieldOfView(Math::Pi * 0.25f, Aspect, 0.1f, 1000.f);
	MainCamera->SetPosition(0, 0, -5);

	this->Viewport = new D3D11_VIEWPORT();
	this->Viewport->TopLeftX = 0;
	this->Viewport->TopLeftY = 0;
	this->Viewport->Width = D3D::GetDesc().Width;
	this->Viewport->Height = D3D::GetDesc().Height;
	this->Viewport->MinDepth = 0;
	this->Viewport->MaxDepth = 1;
}

Context::~Context()
{
	SAFE_DELETE(Viewport);
	SAFE_DELETE(MainCamera);
}

