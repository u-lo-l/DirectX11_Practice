#include "Pch.h"
#include "Converter2.h"

const unsigned int Converter2::ConvertFlag =
	aiProcess_ConvertToLeftHanded
	| aiProcess_Triangulate
	| aiProcess_GenNormals
	| aiProcess_CalcTangentSpace
	| aiProcess_TransformUVCoords
	| aiProcess_GenUVCoords
	| aiProcess_OptimizeMeshes;

const float Converter2::PreYRotation = Math::Pi;

Converter2::Converter2()
	: Importer(new Assimp::Importer()) 
{
}

Converter2::~Converter2()
{
	SAFE_DELETE(Importer);
}

void Converter2::ReadAiScene(const wstring& InFileName)
{

}
