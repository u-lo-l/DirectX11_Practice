#include "Pch.h"
#include "Converter.h"
#include <fstream>

namespace Sdt
{
	void Converter::ExportAnimation( const string & InSaveFileName, int InClipIndex )
	{
		const string SaveFileName = MODEL_PATH + InSaveFileName + ".animation";

		ClipData * const Clip = ReadClipData(Scene->mAnimations[InClipIndex]);

		ConnectNodeWithBone(Clip, Scene->mRootNode);
		
		WriteClipData(SaveFileName, Clip);
	}

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

	/**
	 * @breif Missing된 Bonen에 정보를 써 준다?
	 * @param InClipData 
	 * @param InNode 
	 */
	void Converter::ConnectNodeWithBone( ClipData * InClipData, const aiNode * InNode )
	{
		bool bFound = false;
		const UINT NodeCount = InClipData->NodeDatas.size();
		for (UINT i = 0 ; i < NodeCount; i++)
		{
			ClipNodeData * NodeData = InClipData->NodeDatas[i];

			const string & NodeName = InNode->mName.C_Str();
			if (NodeName == NodeData->BoneName)
			{
				bFound = true;
				break;
			}
		}

		// InNode에 맞는 ClipData가 없다?
		if (bFound == true)
		{
#ifdef DO_DEBUG
			printf("Bone which is not connected to animation : %s\n", InNode->mName.C_Str());
#endif
			ClipNodeData * NodeData = new ClipNodeData();

			Matrix TempMat = InNode->mTransformation;
			TempMat.Transpose();
			Vector T, S;
			Quaternion R;
			TempMat.Decompose(S, R, T);
			const UINT TotalFrameCount = static_cast<UINT>(InClipData->Duration);
			NodeData->BoneName = InNode->mName.C_Str();
			for (UINT f = 0; f < TotalFrameCount; f++)
			{
				// 해당 Bone에 대한 NodeData생성
				NodeData->PosKeys.emplace_back(static_cast<float>(f), T);
				NodeData->ScaleKeys.emplace_back(static_cast<float>(f), S);
				NodeData->RotKeys.emplace_back(static_cast<float>(f), R);
			}
			InClipData->NodeDatas.push_back(NodeData);
		}
		
		const UINT ChildCount = InNode->mNumChildren;
		for (UINT i = 0 ; i < ChildCount; i++)
		{
			ConnectNodeWithBone(InClipData, InNode->mChildren[i]);
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