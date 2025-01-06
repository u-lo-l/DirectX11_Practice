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
	private:
		string ColorToJson(const Color & InColor) const;
	private:
		wstring FilePath;
		Assimp::Importer* Loader;
		const aiScene * Scene;
		
#pragma region ExtractMaterial
	public:
		void ExportMaterial(const wstring& InSaveFileName, bool InbOverwrite = true);
	private:
		void ReadMaterial();
		void WriteMaterial(const wstring& InSaveFileName, bool InbOverwrite) const;
		string SaveTexture(const string & InSaveFolder, const string & InFileName) const;
	private:
		vector<MaterialData*> Materials;
#pragma endregion
		
#pragma region ExtractMesh
	public:
		void ExportMesh(const wstring& InSaveFileName);
	private:
		void ReadBoneData(const aiNode * InRootNode, int InIndex, int InParent);
#pragma endregion

	};
}
