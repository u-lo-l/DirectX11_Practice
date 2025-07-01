#pragma once
#include <mutex>

namespace std
{
	class thread;
}

class Window
{
public:
	static WPARAM Run(class IExecutable* InMain);

private:
	static std::mutex g_printMutex;
	static void Create();
	static void Destroy();

	static LRESULT CALLBACK WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam);

	static void MainRender();

private:
	static class IExecutable* Main;
	static bool HandleOSEvent();
	static void RunPhysics();
	static void RunRenderer();
	static void Print(const char* format, ...);

	static std::atomic<bool> bRenderThreadReady;
	static std::atomic<bool> bPhysicsThreadReady;
	static std::atomic<bool> bProgramFinished;
	static std::atomic<bool> bMainThreadReady;

	static std::thread * RenderThread;
	static std::thread * PhysicsThread;
};

