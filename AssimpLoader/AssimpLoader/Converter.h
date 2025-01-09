#pragma once
#include "Types.h"

namespace Sdt
{
	class Converter
	{
	private:
		using ThisClass = Converter;
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
		/**
		 * @brief FBX model의 texture정보 종류 <br/>
		 * 1) 파일 내부에 저장된 texture (embedded texture) <br/>
		 * 2) 파일 외부에 별도로 지정된 texture. (external texture) <br/>
		 * @param InSaveFolder : material파일이 저장될 경로
		 * @param InFileName : material의 전체 경로포함 이름. 함수 내부적으로 마지막 파일명만 추출됨.
		 */
		string SaveTextureAsFile(const string & InSaveFolder, const string & InFileName) const;
	private:
		vector<MaterialData*> Materials;
#pragma endregion
		
#pragma region ExtractMesh
	public:
		void ExportMesh(const wstring& InSaveFileName);
	private:
		void ReadBoneData(const aiNode * InNode, int InIndex, int InParent);
		void ReadMeshData();
		static MeshData::VertexType ReadSingleVertexDataFromAiMesh(const aiMesh * Mesh, UINT VertexIndex);

		void WriteMesh(const wstring& InSaveFileName) const;
	private:
		vector<BoneData*> Bones;
		vector<MeshData*> Meshes;
#pragma endregion

	};
}
