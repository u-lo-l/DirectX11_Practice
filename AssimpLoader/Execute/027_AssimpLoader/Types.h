#pragma once
#include "Pch.h"


struct MaterialData
{
	string Name;
	string ShaderName;

	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	string DiffuseFile;
	string SpecularFile;
	string NormalFile;

	float Shininess;
};
