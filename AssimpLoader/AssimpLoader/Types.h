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

	void SetTextureFilesFromAiMaterialByTextureType(const aiMaterial * Material, aiTextureType InTextureType);
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