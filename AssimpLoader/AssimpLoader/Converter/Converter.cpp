#include "Pch.h"
#include "Converter.h"
#include <fstream>

namespace Sdt
{
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

		if (Scene != nullptr)
		{
			Loader->FreeScene();
		}

		Scene = Loader->ReadFile(
			String::ToString(FilePath).c_str(),
			aiProcess_ConvertToLeftHanded
			| aiProcess_Triangulate
			| aiProcess_GenUVCoords
			| aiProcess_GenNormals
			| aiProcess_CalcTangentSpace
			| aiProcess_GenBoundingBoxes
		);

		ASSERT(Scene != nullptr, Loader->GetErrorString());
	}

	string Converter::ColorToJson( const Color & InColor )
	{
		return String::Format("%0.6f,%0.6f,%0.6f,%0.6f", InColor.R, InColor.G, InColor.B, InColor.A);
	}
}
