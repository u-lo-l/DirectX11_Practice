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

	View = GetViewMatrix();
	// ViewInv = Matrix::Invert(GetViewMatrix());
	// this->Desc.ViewProjection = this->Desc.View * this->Desc.Projection;

	ImGui::SliderFloat3("LightDirection", LightDirection, -1, +1);
}

/**
 *	@brief :
 *	1. GUI를 활용해서, FPS, Camera transform 정보등을 표시한다.
 *	2. Constant Buffer를 GPU에 Bind한다.
 */
void Context::Render() const
{
	string Str = string("FrameRate : ") + to_string(static_cast<int>(ImGui::GetIO().Framerate));
	Gui::Get()->RenderText(5, 5, 1, 1, 1, Str);

	const Vector & CamPos = MainCamera->GetPosition();
	const Vector & CamRot = MainCamera->GetEulerAngle();
	Str = "Camera Rotation : " + String::ToString(CamRot.ToString());
	Gui::Get()->RenderText(5, 20, 1, 1, 1, Str);
	
	Str = "Camera Position : " + String::ToString(CamPos.ToString());
	Gui::Get()->RenderText(5, 35, 1, 1, 1, Str);
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

