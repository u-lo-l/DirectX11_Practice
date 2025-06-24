#pragma once
#include "AConverterBase.h"
#include "AssimpLoader/Types.h"

class AnimationConverter : public AConverterBase
{
// protected:
// 	wstring FileName;
// 	Assimp::Importer * Importer;
// 	aiScene * Scene;
public:
	AnimationConverter();
	virtual ~AnimationConverter() override;
	virtual void ReadAiScene(const wstring& InFileName) override;
private:
	static constexpr int DefaultTicksPerSecond = 30;
	static void ExportAnimation(const wstring& InFileName, const aiScene* InScene);
	static ClipData* ReadClipData(const aiAnimation* InAnimation, const aiNode* RootNode);
	static void ReadPosKeySequences(
		vector<FrameDataVec>& OutPosKeys,
 		const aiNodeAnim* InNodeAnim,
  		const ClipData* InClipData
	);
	static void ReadScaleKeySequences(
		vector<FrameDataVec>& OutScaleKeys,
		const aiNodeAnim* InNodeAnim,
		const ClipData* InClipData
	);
	static void ReadRotKeySequences(
		vector<FrameDataQuat>& OutRotKeys,
		const aiNodeAnim* InNodeAnim,
		const ClipData* InClipData
	);
	static void ConnectNodeWithBone(
		ClipData* InOutClipData,
		const aiNode* InNode,
		const set<string> & InBoneNames_BinTree
	);
	static void WriteClipData(const string& InSaveFileName, const ClipData* InClipData);
};
