#pragma once

class Gui
{
public:
	static void Create();
	static void Destroy();
	static Gui * Get();
	static void Tick();
	static void Render();
	static LRESULT WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam);

private:
	static Gui * Instance;
	Gui();
	~Gui();

};