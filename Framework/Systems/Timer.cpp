#include "Framework.h"
#include "SystemTimer.h"

namespace Sdt
{
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

	float Timer::ElapsedTime() const
	{
		return static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - StartTime).count());
	}
}
