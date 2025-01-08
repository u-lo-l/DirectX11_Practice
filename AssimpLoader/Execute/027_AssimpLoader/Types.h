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
		switch (InTextureType)
		{
			case aiTextureType_DIFFUSE:
				TargetList = &DiffuseFiles;
				break;
			case aiTextureType_SPECULAR:
				TargetList = &SpecularFiles;
				break;
			case aiTextureType_NORMALS:
				TargetList = &NormalFiles;
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

		for (UINT TextureIndex = 0; TextureIndex < TextureCount; TextureIndex++)
		{
			Material->GetTexture(InTextureType, TextureIndex, &TextureFile);
			TargetList->emplace_back(TextureFile.C_Str());
		}
	}
};

struct BoneData
{
	UINT Index;
	string Name;
	int Parent;
	Matrix Transform;

	vector<UINT> MeshIndices;
};

struct MeshData
{
	using VertexType = ModelVertex;
	
	string Name;

	string MaterialName;

	vector<VertexType> Vertices;
	vector<UINT> Indices;
};