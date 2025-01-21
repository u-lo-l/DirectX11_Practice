#pragma once

namespace Sdt
{
	class Timer
	{
	public:
		static void Create();
		static void Destroy();
		static Timer* Get();
		void Tick();
		float GetDeltaTime() const { return DeltaTime; }

	private:
		static Timer* Instance;
	private:
		Timer();
		~Timer();

	private:
		chrono::steady_clock::time_point StartTime;
		chrono::steady_clock::time_point PrevFrameTime;
		float DeltaTime;
	};
}
