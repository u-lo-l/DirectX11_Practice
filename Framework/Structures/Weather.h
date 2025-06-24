#pragma once

enum WeatherType : uint8_t
{
	None = 0, Rain, Snow, Max,
};

struct WeatherData_VS
{
	Vector Velocity;
	float Padding;
	
	Vector Origin = Vector::Zero;
	float Turbulence = 0.0f;
	
	Vector Extent = Vector::Zero;
	float Time = 0.0f;
};

struct WeatherData_PS
{
	Color PrecipitationColor;
};

struct WeatherData_GS
{
	Vector Velocity;
	float DrawDistance;
};