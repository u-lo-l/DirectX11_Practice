#pragma once
#include "Converter2.h"
#include "AssimpLoader/Types.h"

class AnimationConverter : public Converter2
{
// protected:
// 	wstring FileName;
// 	Assimp::Importer * Importer;
// 	aiScene * Scene;
public:
	AnimationConverter();
	~AnimationConverter() override;
	void ReadAiScene(const wstring& InFileName) override;
private:
	static constexpr int DefaultTicksPerSecond = 30;
	static void ExportAnimation(const wstring& InFileName, const aiScene* InScene);
	static ClipData* ReadClipData(const aiAnimation* InAnimation);
	static void ReadPosKeySequences(vector<FrameDataVec>& OutPosKeys, const aiNodeAnim* InNodeAnim, const ClipData* InClipData);
	static void ReadScaleKeySequences(vector<FrameDataVec>& OutScaleKeys, const aiNodeAnim* InNodeAnim, const ClipData* InClipData);
	static void ReadRotKeySequences(vector<FrameDataQuat>& OutRotKeys, const aiNodeAnim* InNodeAnim, const ClipData* InClipData);
	static void ConnectNodeWithBone(ClipData* InOutClipData, const aiNode* InNode, const set<string>& InBoneNames_BinTree);
	static void WriteClipData(const string& InSaveFileName, const ClipData* InClipData);
};
