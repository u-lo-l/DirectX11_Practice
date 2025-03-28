#pragma once

namespace sdt
{
// #pragma region Timer
// 	class Timer
// 	{
// 	public:
// 		Timer();
// 		~Timer();
//
// 		void Start();
// 		float ElapsedTime() const;
//
// 	private:
// 		chrono::steady_clock::time_point StartTime;
// 		
// 	};
// #pragma endregion Timer

#pragma region SystemTimer
	class SystemTimer
	{
	public:
		static void Create();
		static void Destroy();
		static SystemTimer* Get();
		void Tick();
		float GetDeltaTime() const { return DeltaTime; }
		float GetRunningTime() const { return RunningTime; }
	private:
		static SystemTimer* Instance;
		
	private:
		SystemTimer();
		~SystemTimer();

	private:
		chrono::steady_clock::time_point StartTime;
		chrono::steady_clock::time_point PrevFrameTime;
		
		float DeltaTime;	// Unit : Second
		float RunningTime;	// Unit : Second
	};
#pragma endregion SystemTimer
}
