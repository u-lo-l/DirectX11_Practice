#include "Pch.h"
#include "ExportFile.h"
#include "Converter.h"

namespace Sdt
{

	void ExportFile::Initialize()
	{
		this->Mousey();
		// this->Shannon();
		// this->Airplane();
		//this->Cube();
	}

	void ExportFile::Airplane()
	{
		Converter* converter = new Converter();
		converter->ReadFile(L"Airplane/Airplane.fbx");
		converter->ExportMaterial(L"Airplane/Airplane");
		SAFE_DELETE(converter);
	}

	void ExportFile::Cube()
	{
		Converter* converter = new Converter();
		converter->ReadFile(L"Cube/Cube.fbx");
		converter->ExportMaterial(L"Cube/Cube");
		converter->ExportMesh(L"Cube/Cube");
		SAFE_DELETE(converter);
	}

	void ExportFile::Shannon()
	{
		Converter* converter = new Converter();
		converter->ReadFile(L"Shannon/Shannon.fbx");
		converter->ExportMaterial(L"Shannon/Shannon");
		SAFE_DELETE(converter);
	}
	
	void ExportFile::Mousey()
	{
		Converter* converter = new Converter();
		converter->ReadFile(L"Mousey/Mousey.fbx");
		converter->ExportMaterial(L"Mousey/Mousey");
		SAFE_DELETE(converter);
	}
}
