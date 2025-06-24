#pragma once

struct FrameDataVec
{
	FrameDataVec() = default;
	FrameDataVec(float InTime, const Vector& InValue)
		: Time(InTime), Value(InValue) {}
	float Time = 0;
	Vector Value;
};

struct FrameDataQuat
{
	FrameDataQuat() = default;
	FrameDataQuat(float InTime, const Quaternion& InValue)
		: Time(InTime), Value(InValue) {}
	float Time = 0;
	Quaternion Value;
};