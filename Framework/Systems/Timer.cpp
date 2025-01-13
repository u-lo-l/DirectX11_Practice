#include "Framework.h"
#include "Timer.h"

namespace Sdt
{
	Timer* Timer::Instance = nullptr;

	void Timer::Create()
	{
		ASSERT(Instance == nullptr, "Timer Instance must null on Create()");

		Instance = new Timer();
	}

	void Timer::Destroy()
	{
		ASSERT(Instance != nullptr, "Timer Instance must not null on Destroy()");

		SAFE_DELETE(Instance);
	}

	Timer * Timer::Get()
	{
		ASSERT(Instance != nullptr, "Timer Instance must not null on Get()");

		return Instance;
	}

	void Timer::Tick()
	{
		const chrono::steady_clock::time_point current = chrono::steady_clock::now();

		const chrono::duration<double> Temp = (current - PrevFrameTime);
		DeltaTime = static_cast<float>(Temp.count());
		PrevFrameTime = current;
	}

	Timer::Timer()
	{
		//StartTime = chrono::steady_clock::now();
	}


	Timer::~Timer()
	{
	}
}
