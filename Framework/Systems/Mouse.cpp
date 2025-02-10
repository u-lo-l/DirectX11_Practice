#include "Framework.h"
#include "Mouse.h"

namespace Sdt
{
	Mouse* Mouse::Instance = nullptr;

	void Mouse::Create()
	{
		ASSERT(Instance == nullptr, "Mouse Instance must null on Create()")

		Instance = new Mouse();
	}

	void Mouse::Destroy()
	{
		ASSERT(Instance != nullptr, "Mouse Instance must not null on Destroy()")

		SAFE_DELETE(Instance);
	}

	Mouse* Mouse::Get()
	{
		//ASSERT(Instance != nullptr, "Mouse Instance must not null on Get()")

		return Instance;
	}

	void Mouse::Tick()
	{
		memcpy(ButtonPrevStatus, ButtonStatus, sizeof(BYTE) * MAX_MOUSE_BUTTON);
		ZeroMemory(ButtonStatus, sizeof(BYTE) * MAX_MOUSE_BUTTON);
		ZeroMemory(ButtonMaps, sizeof(MouseButtonState) * MAX_MOUSE_BUTTON);

		ButtonStatus[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000 ? 1 : 0;
		ButtonStatus[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000 ? 1 : 0;
		ButtonStatus[2] = GetAsyncKeyState(VK_MBUTTON) & 0x8000 ? 1 : 0;

		for (DWORD i = 0; i < MAX_MOUSE_BUTTON; i++)
		{
			const int currStatus = ButtonStatus[i];
			const int prevStatus = ButtonPrevStatus[i];

			if (prevStatus == 0 && currStatus == 1)
				ButtonMaps[i] = MouseButtonState::Down;
			else if (prevStatus == 1 && currStatus == 0)
				ButtonMaps[i] = MouseButtonState::Up;
			else if (prevStatus == 1 && currStatus == 1)
				ButtonMaps[i] = MouseButtonState::Press;
			else
				ButtonMaps[i] = MouseButtonState::None;
		}

		POINT CursorPoint;
		GetCursorPos(&CursorPoint);
		// GetCursorPos로 얻은 Cursor 위치는, 윈도우의 타이틀바, 경계도 포함한 위치이다.
		// 이를 Client(출력되는 화면 내부)로 다시 매핑해준다.
		ScreenToClient(D3D::GetDesc().Handle, &CursorPoint);

		WheelPrevStatus.X = WheelStatus.X;
		WheelPrevStatus.Y = WheelStatus.Y;

		WheelStatus.X = static_cast<float>(CursorPoint.x);
		WheelStatus.Y = static_cast<float>(CursorPoint.y);

		WheelMoveDelta = WheelStatus - WheelPrevStatus;
		WheelPrevStatus.Z = WheelStatus.Z;
	}

	void Mouse::WndProc(UINT InMessage, WPARAM InWParam, LPARAM InLParam)
	{
		if (InMessage == WM_MOUSEMOVE)
		{
			Position.X = static_cast<float>(LOWORD(InLParam));
			Position.Y = static_cast<float>(HIWORD(InLParam));
		}
		if (InMessage == WM_MOUSEWHEEL)
		{
			const short Temp = static_cast<short>(HIWORD(InWParam));
			WheelPrevStatus.Z = WheelStatus.Z;
			WheelStatus.Z += static_cast<float>(Temp);
		}
	}

	bool Mouse::IsDown(MouseButton InType) const
	{
		return ButtonMaps[static_cast<int>(InType)] == MouseButtonState::Down;
	}

	bool Mouse::IsUp(MouseButton InType) const
	{
		return ButtonMaps[static_cast<int>(InType)] == MouseButtonState::Up;
	}

	bool Mouse::IsPress(MouseButton InType) const
	{
		return ButtonMaps[static_cast<int>(InType)] == MouseButtonState::Press;
	}

	bool Mouse::IsDown(int InType) const
	{
		return this->IsDown(static_cast<MouseButton>(InType));
	}

	bool Mouse::IsUp(int InType) const
	{
		return this->IsUp(static_cast<MouseButton>(InType));
	}

	bool Mouse::IsPress(int InType) const
	{
		return this->IsPress(static_cast<MouseButton>(InType));
	}

	Mouse::Mouse()
	{
		Position = {0, 0, 0};
		ZeroMemory(ButtonStatus, sizeof(BYTE) * MAX_MOUSE_BUTTON);
		ZeroMemory(ButtonPrevStatus, sizeof(BYTE) * MAX_MOUSE_BUTTON);
		ZeroMemory(ButtonMaps, sizeof(MouseButtonState) * MAX_MOUSE_BUTTON);

		WheelStatus = WheelPrevStatus = WheelMoveDelta = { 0, 0, 0 };


		DWORD Line = 0;
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &Line, 0);
	}

	Mouse::~Mouse()
	{
	}
}
