#include "Pch.h"
#include "Convert.h"
#include "Converter.h"

namespace Sdt
{

	void Convert::Initialize()
	{
		this->Airplane();
		//this->Cube();
	}

	void Convert::Airplane()
	{
		Converter* converter = new Converter();
		converter->ReadFile(L"Airplane/Airplane.fbx");
		converter->ExportMaterial(L"Airplane/Airplane");
		SAFE_DELETE(converter);
	}

	void Convert::Cube()
	{
		// Converter* converter = new Converter();
		// converter->ReadFile(L"Cube/Cube.fbx");
		// SAFE_DELETE(converter);
	}
}
