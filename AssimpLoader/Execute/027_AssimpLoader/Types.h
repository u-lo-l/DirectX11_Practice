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

	vector<string> DiffuseFiles;
	vector<string> SpecularFiles;
	vector<string> NormalFiles;

	void SetTextureFiles(const aiMaterial * const Material, aiTextureType InTextureType)
	{
		vector<string> * TargetList = nullptr;
		string TextureTypeStr;
		switch (InTextureType)
		{
			case aiTextureType_DIFFUSE:
				TargetList = &DiffuseFiles;
				TextureTypeStr = "Diffuse"; 
				break;
			case aiTextureType_SPECULAR:
				TargetList = &SpecularFiles;
				TextureTypeStr = "Specular"; 
				break;
			case aiTextureType_NORMALS:
				TargetList = &NormalFiles;
				TextureTypeStr = "Normal"; 
				break;
			default:
				break;
		}
		if (TargetList == nullptr)
		{
			return;
		}
		
		aiString TextureFile;
		const UINT TextureCount = Material->GetTextureCount(InTextureType);
		TargetList->reserve(TextureCount);

		printf("[%s]. Texture Count : %d\n", TextureTypeStr.c_str(), TextureCount);
		for (UINT TextureIndex = 0; TextureIndex < TextureCount; TextureIndex++)
		{
			Material->GetTexture(InTextureType, TextureIndex, &TextureFile);
			TargetList->emplace_back(TextureFile.C_Str());
		}
	}
};
