#pragma once
#include "Core/Vector2D.h"

class Gui
{
public:
	static void Create();
	static void Destroy();
	static Gui * Get();
	static void Tick();
	void Render();
	static LRESULT WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam);

public:
	void RenderText(float x, float y, float r, float g, float b, const string & Context);
	void RenderText(Vector2D Position, Color Color, const string & context);
private:
	static Gui * Instance;
	Gui();
	~Gui();

private:
	struct GuiText
	{
		GuiText() {}
		GuiText(const Vector2D & Position, const Color & Color, const string & Context );
		Vector2D Position;
		Color Color;
		string Content;
	};
	vector<GuiText> Contents;
};