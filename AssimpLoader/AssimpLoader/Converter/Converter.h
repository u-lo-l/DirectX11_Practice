﻿#pragma once
#include "../Types.h"

namespace sdt
{
	class Converter
	{
	private:
		static constexpr int DefaultTicksPerSecond = 30;
	private:
		using ThisClass = Converter;
	public:
		Converter();
		~Converter();
		void ReadAiSceneFromFile(const wstring& InFileName);

	private:
		wstring FilePath;
		Assimp::Importer* Loader;
		const aiScene * Scene;
		
#pragma region ExtractMaterial
	public:
		void ExportMaterial( const wstring& InSaveFileName, const string & InShaderFileName, bool InbOverwrite );
	private:
		void ReadMaterial(const string & InShaderName);
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
		static MeshData::VertexType ReadSingleVertexDataFromAiMesh(const aiMesh * Mesh, UINT VertexIndex, const aiMatrix4x4 & InMeshTransform);
		void WriteAndClearBonesAndMeshes(const wstring& InSaveFileName);

		void ReadSkinData();
	private:
		// Scene의 모든 BoneNode정보.
		vector<BoneData*> Bones;
		vector<MeshData*> Meshes;
#pragma endregion

		// vector<pair<string, Matrix>> BoneMatrix;
		// vector<pair<string, Matrix>> OffsetMatrix;

#pragma region Extract Animation
	public:
		void ExportAnimation( const string & InSaveFileName, int InClipIndex = -1);
	private:
		// 애니메이션 하나하나를 Clip이라 한다.
		// void ReadClips( vector<string> & OutClips ) const;
		static ClipData * ReadClipData(const aiAnimation * InAnimation);
		static void WriteClipData( const string & InSaveFileName, const ClipData * InClipData );
		static void ReadPosKeySequences(vector<FrameDataVec> & OutPosKeys, const aiNodeAnim * InNodeAnim, const ClipData * InClipData );
		static void ReadScaleKeySequences(vector<FrameDataVec> & OutScaleKeys, const aiNodeAnim * InNodeAnim, const ClipData * InClipData );
		static void ReadRotKeySequences(vector<FrameDataQuat> & OutRotKeys, const aiNodeAnim * InNodeAnim, const ClipData * InClipData );

		static void ConnectNodeWithBone(ClipData * InOutClipData, const aiNode * InNode, const set<string> & InBoneNames_BinTree);
#pragma endregion
	};
}
