#pragma once
#ifndef MAX_POINT_LIGHTS
	#define MAX_POINT_LIGHTS 256
#endif	
struct PointLight
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	Vector Position;
	float Range;

	float Intensity;
	Vector Padding;
};

struct PointLightDesc
{
	UINT Count = 0;
	Vector Padding;

	PointLight Lights[MAX_POINT_LIGHTS];
};

#ifndef MAX_SPOT_LIGHTS
	#define MAX_SPOT_LIGHTS 256
#endif
struct SpotLight
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	Vector Position;
	float Range;

	Vector Direction;
	float Intensity;

	float Theta;
	float Phi;
	Vector2D Padding;
};
struct SpotLightDesc
{
	UINT Count = 0;
	Vector Padding;

	SpotLight Lights[MAX_POINT_LIGHTS];
};