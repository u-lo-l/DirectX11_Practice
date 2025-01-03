#pragma once
#include "Types.h"

namespace Sdt
{
	class Converter
	{
	public:
		Converter();
		~Converter();

		void ReadFile(const wstring& InFileName);

		void ExportMaterial(const wstring& InSaveFileName, bool InbOverwrite = true);

	private:
		void ReadMaterial();
		void WriteMaterial(const wstring& InSaveFileName, bool InbOverwrite) const;

		Assimp::Importer* Loader;
		const aiScene * Scene;

		vector<MaterialData*> Materials;
	};
}
