#include "Pch.h"
#include "Converter.h"
#include <fstream>

namespace sdt
{
	const float Converter::PreYRotation = Math::Pi;
	
	Converter::Converter()
	 : Loader(nullptr), Scene(nullptr)
	{
		Loader = new Assimp::Importer();
	}

	Converter::~Converter()
	{
		Loader->FreeScene();
		SAFE_DELETE(Loader);
	}

	void Converter::ReadAiSceneFromFile( const wstring & InFileName )
	{
		FilePath = W_ASSET_PATH + InFileName;

		if (this->Scene != nullptr)
		{
			Loader->FreeScene();
		}
	
		this->Scene = Loader->ReadFile(
			String::ToString(FilePath).c_str(),
			aiProcess_ConvertToLeftHanded
			| aiProcess_Triangulate
			| aiProcess_GenNormals
			| aiProcess_CalcTangentSpace
			| aiProcess_TransformUVCoords
			| aiProcess_GenUVCoords
			| aiProcess_OptimizeMeshes
		);

		ASSERT(Scene != nullptr, Loader->GetErrorString())
	}
}
