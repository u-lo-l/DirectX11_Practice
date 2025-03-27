#pragma once
#include "WeatherStructure.h"
#include "Renders/Shader/HlslShader.hpp"
// 강수
// Any form of water, such as rain, snow, or hail, that falls from the atmosphere to the Earth's surface
class Precipitation
{
	static constexpr int Const_Slot_GS_VieProjection = 1;
	static constexpr int Const_Slot_VS_WeatherData = 2;
	static constexpr int Const_Slot_PS_WeatherData = 3;
	static constexpr int Const_Slot_GS_WeatherData = 4;
	static constexpr int Texture_Slot_PS_Diffuse = 0;
	using VertexType = VertexPrecipitation;
	using ShaderType = HlslShader<VertexType>;
public:
	Precipitation(const Vector& InExtent, UINT InDrawCount, const wstring & InFilePath, WeatherType InWeatherType);
	~Precipitation();
	void Tick();
	void Render();
private:
	void CreateWeatherData(const Vector & InExtent);
	void CreateWeatherVertexBuffer();
	void CreateWeatherConstantBuffers();

	WeatherType Type = WeatherType::Rain;  
	
	wstring ShaderName;
	
	ShaderType * WeatherShader = nullptr;

	Transform * World = nullptr;
	
	WeatherData_VS VS_WeatherData {};
	ConstantBuffer * WeatherSettingCBuffer_VS = nullptr;
	VertexBuffer * VBuffer = nullptr;
	
	WeatherData_GS GS_WeatherData {};
	ConstantBuffer * WeatherSettingCBuffer_GS = nullptr;
	ViewProjectionDesc ViewProjectionData {};
	ConstantBuffer * ViewProjectionBuffer_GS = nullptr;
	
	WeatherData_PS PS_WeatherData;
	ConstantBuffer * WeatherSettingCBuffer_PS = nullptr;
	Texture * Image = nullptr;;
	
	UINT DrawCount = 100;
};
