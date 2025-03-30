#pragma once
#include "LigthtingStruct.h"

class LightingManager
{
public:
	static void Create();
	static void Destroy();

	static LightingManager* Get();
public:
	void Tick();
	void Render();
private:
	LightingManager();
	~LightingManager();

public:
	void AddPointLight(const PointLight & light);
	void AddSpotLight(const SpotLight & light);
private:
	static LightingManager * Instance;
	
private:
	struct LightDesc
	{
		PointLightDesc PointLightData;
		SpotLightDesc SpotLightData;
	} LightData;

	ConstantBuffer * LightCBuffer = nullptr;
};
