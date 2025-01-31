#include "framework.h"
#include "SystemTimer.h"

namespace Sdt
{
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
		StartTime = chrono::steady_clock::now();
		PrevFrameTime = chrono::steady_clock::now();
	}

	SystemTimer::~SystemTimer()
	{
	}
}
