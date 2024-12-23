#include "Framework.h"
#include "Gui.h"

Gui * Gui::Instance = nullptr;

void Gui::Create()
{
	ASSERT(Instance == nullptr, "GUI Instance must null on Create()");

	Instance = new Gui();
}

void Gui::Destroy()
{
	ASSERT(Instance != nullptr, "GUI Instance must not null on Destroy()");

	SAFE_DELETE(Instance);
}

Gui* Gui::Get()
{
	ASSERT(Instance == nullptr, "GUI Instance must null on Get()");

	return Instance;
}

Gui::Gui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	const D3DDesc & Desc = D3D::GetDesc();
	ImGui_ImplWin32_Init(Desc.Handle);
	ImGui_ImplDX11_Init(D3D::Get()->GetDevice(), D3D::Get()->GetDeviceContext());
}

Gui::~Gui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

LRESULT Gui::WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
	if (ImGui::GetCurrentContext() == nullptr)
		return 0;

	return ImGui_ImplWin32_WndProcHandler(InHandle, InMessage, InwParam, InlParam);
}

void Gui::Tick()
{
	// 새로운 프레임이 시작되었다고 ImGui에 알려줌
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Gui::Render()
{
	//렌더링 할 위치 알려줌
	const ImGuiViewport * ViewPort = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ViewPort->Pos);
	ImGui::SetNextWindowSize(ViewPort->Size);
	ImGui::SetNextWindowBgAlpha(0.0f);
	// 기본 창 띄우기
	ImGui::Begin
	(
		"TextWindow", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus
	);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
