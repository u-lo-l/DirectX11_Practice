#pragma once

namespace std
{
	class thread;
}

class Window
{
public:
	static WPARAM Run(class IExecutable* InMain);

private:
	static void Create();
	static void Destroy();

	static LRESULT CALLBACK WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam);

	static void MainRender();

private:
	static class IExecutable* Main;
	static bool HandleOSEvent();
	static void RunGameLogic();
	static void RunPhysics();
	static void RunRenderer();

	static std::thread * GameThread;
	static std::thread * RenderThread;
	static std::thread * PhysicsThread;
};

