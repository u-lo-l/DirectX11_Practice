#include "Framework.h"
#include "SystemTimer.h"

namespace Sdt
{
#pragma region Timer

	Timer::Timer()
	{
	}

	Timer::~Timer()
	{
	}

	void Timer::Start()
	{
		StartTime = chrono::steady_clock::now();
	}

	float Timer::ElapsedTime()
	{
		return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - StartTime).count();
	}
#pragma endregion Timer

#pragma region SystemTimer
	SystemTimer* SystemTimer::Instance = nullptr;

	void SystemTimer::Create()
	{
		ASSERT(Instance == nullptr, "Timer Instance must null on Create()");

		Instance = new SystemTimer();
	}

	void SystemTimer::Destroy()
	{
		ASSERT(Instance != nullptr, "Timer Instance must not null on Destroy()");

		SAFE_DELETE(Instance);
	}

	SystemTimer * SystemTimer::Get()
	{
		ASSERT(Instance != nullptr, "Timer Instance must not null on Get()");

		return Instance;
	}

	void SystemTimer::Tick()
	{
		const chrono::steady_clock::time_point current = chrono::steady_clock::now();

		const chrono::duration<double> Temp = (current - PrevFrameTime);
		DeltaTime = static_cast<float>(Temp.count());
		PrevFrameTime = current;
		RunningTime += static_cast<double>(DeltaTime);
	}

	SystemTimer::SystemTimer()
	: DeltaTime(0.f), RunningTime(0.f)
	{
		//StartTime = chrono::steady_clock::now();
	}

	SystemTimer::~SystemTimer()
	{
	}
#pragma endregion SystemTimer

}
