#include "Pch.h"
#include "Converter.h"
#include <fstream>

namespace Sdt
{
	void Converter::ExportAnimation( const string & InSaveFileName, int InClipIndex ) const
	{
		const string SaveFileName = MODEL_PATH + InSaveFileName + ".animation";
		// if (InClipIndex < 0)
		// {
		// 	vector<string> ClipNames;
		// 	ReadClips(ClipNames);
		//
		// 	for (UINT i = 0; i < ClipNames.size(); i++)
		// 	{
		// 		ReadClipData(Scene->mAnimations[i]);
		// 	}
		// 	return ;
		// }
		const ClipData * const Clip = ReadClipData(Scene->mAnimations[InClipIndex]);
		WriteClipData(SaveFileName, Clip);
	}

	// void Converter::ReadClips( vector<string> & OutClips ) const
	// {
	// 	ASSERT(Scene != nullptr, "aiScene is not valid");
	//
	// 	const UINT ClipCount = this->Scene->mNumAnimations;
	// 	for (UINT i = 0; i < ClipCount; i++)
	// 	{
	// 		OutClips.push_back( Scene->mAnimations[i]->mName.C_Str());
	// 	}
	// }

	ClipData * Converter::ReadClipData( const aiAnimation * InAnimation )
	{
		ClipData * Data = new ClipData{
			InAnimation->mName.C_Str(),
			static_cast<float>(InAnimation->mDuration),
			static_cast<float>(InAnimation->mTicksPerSecond),
		};

		const UINT ChannelsCount = InAnimation->mNumChannels;
		for (UINT i = 0; i < ChannelsCount; i++)
		{
			const aiNodeAnim * const NodeAnim = InAnimation->mChannels[i];

			ClipNodeData * NodeData = new ClipNodeData();
			NodeData->BoneName = NodeAnim->mNodeName.C_Str();
			ReadPosKeys(NodeData->PosKeys, NodeAnim);
			ReadScaleKeys(NodeData->ScaleKeys, NodeAnim);
			ReadRotKeys(NodeData->RotKeys, NodeAnim);

			Data->NodeDatas.push_back(NodeData);
		}
		return Data;
	}

	void Converter::ReadPosKeys( vector<FrameDataVec> & OutPosKeys, const aiNodeAnim * InNodeAnim )
	{
		const UINT PosKeyCount = InNodeAnim->mNumPositionKeys;
		OutPosKeys.reserve(PosKeyCount);
		for (UINT i = 0; i < PosKeyCount; i++)
		{
			const aiVectorKey & PosKey =  InNodeAnim->mPositionKeys[i];
			FrameDataVec Key;
			Key.Time = static_cast<float>(PosKey.mTime);
			memcpy(&Key.Value, &PosKey.mValue, sizeof(Vector));
			OutPosKeys.push_back(Key);
		}
	}

	void Converter::ReadScaleKeys( vector<FrameDataVec> & OutScaleKeys, const aiNodeAnim * InNodeAnim )
	{
		const UINT ScaleKeyCount = InNodeAnim->mNumScalingKeys;
		OutScaleKeys.reserve(ScaleKeyCount);
		for (UINT i = 0; i < ScaleKeyCount; i++)
		{
			const aiVectorKey & PosKey =  InNodeAnim->mScalingKeys[i];
			FrameDataVec Key;
			Key.Time = static_cast<float>(PosKey.mTime);
			memcpy(&Key.Value, &PosKey.mValue, sizeof(Vector));
			OutScaleKeys.push_back(Key);
		}
	}

	void Converter::ReadRotKeys( vector<FrameDataQuat> & OutRotKeys, const aiNodeAnim * InNodeAnim )
	{
		const UINT RotKeyCount = InNodeAnim->mNumRotationKeys;
		OutRotKeys.reserve(RotKeyCount);
		for (UINT i = 0; i < RotKeyCount; i++)
		{
			const aiQuatKey & RotKey =  InNodeAnim->mRotationKeys[i];
			FrameDataQuat Key;
			Key.Time = static_cast<float>(RotKey.mTime);
			memcpy(&Key.Value, &RotKey.mValue, sizeof(Quaternion));
			OutRotKeys.push_back(Key);
		}
	}

	void Converter::WriteClipData( const string & InSaveFileName, const ClipData * InClipData )
	{
		Path::CreateFolders(Path::GetDirectoryName(InSaveFileName));

		const BinaryWriter * BinWriter = new BinaryWriter(InSaveFileName);

		BinWriter->WriteString(InClipData->Name);
		BinWriter->WriteFloat(InClipData->Duration);
		BinWriter->WriteFloat(InClipData->TicksPerSecond);

		BinWriter->WriteUint(InClipData->NodeDatas.size());
		for (const ClipNodeData * const clipNodeData : InClipData->NodeDatas)
		{
			BinWriter->WriteString(clipNodeData->BoneName);

			const UINT PosKeyCount = clipNodeData->PosKeys.size();
			BinWriter->WriteUint(PosKeyCount);
			BinWriter->WriteByte(&clipNodeData->PosKeys[0], sizeof(FrameDataVec) * PosKeyCount);

			const UINT ScaleKeyCount = clipNodeData->ScaleKeys.size();
			BinWriter->WriteUint(ScaleKeyCount);
			BinWriter->WriteByte(&clipNodeData->ScaleKeys[0], sizeof(FrameDataVec) * ScaleKeyCount);

			const UINT RotKeyCount = clipNodeData->RotKeys.size();
			BinWriter->WriteUint(RotKeyCount);
			BinWriter->WriteByte(&clipNodeData->RotKeys[0], sizeof(FrameDataQuat) * RotKeyCount);
		}

		SAFE_DELETE(BinWriter);
	}
}