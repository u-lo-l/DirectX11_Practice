// ReSharper disable CppClangTidyPerformanceEnumSize
#pragma once
#include "../Core/Vector.h"

constexpr int MAX_INPUT_MOUSE = 8;

enum class MouseButton
{
	Left = 0, Right, Middle, Max
};
constexpr int MAX_MOUSE_BUTTON = 4;
//constexpr int MAX_MOUSE_BUTTON = static_cast<int>(MouseButton::Max);

namespace Sdt
{
	class Mouse
	{
	private:
		enum class MouseButtonState
		{
			None = 0, Down, Up, Press, DoubleClick, Max,
		};
	public:
		static void Create();
		static void Destroy();
		static Mouse* Get();
		void Tick();
		void WndProc(UINT InMessage, WPARAM InWParam, LPARAM InLParam);

	public:
		__forceinline Vector GetMoveDelta() const { return WheelMoveDelta; }
		__forceinline Vector GetPosition() const { return Position; }

	public:
		bool IsDown(MouseButton InType) const;
		bool IsUp(MouseButton InType) const;
		bool IsPress(MouseButton InType) const;

		bool IsDown(int InType) const;
		bool IsUp(int InType) const;
		bool IsPress(int InType) const;

	private:
		static Mouse* Instance;
	private:
		Mouse();
		~Mouse();
	private:
		Vector Position;

		BYTE ButtonStatus[MAX_MOUSE_BUTTON];
		BYTE ButtonPrevStatus[MAX_MOUSE_BUTTON];
		MouseButtonState ButtonMaps[MAX_MOUSE_BUTTON];

		Vector WheelStatus;
		Vector WheelPrevStatus;
		Vector WheelMoveDelta;

	};
}
