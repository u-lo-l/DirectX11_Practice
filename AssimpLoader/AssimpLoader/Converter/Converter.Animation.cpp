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
		ClipData * AnimationClipDataToReturn = new ClipData{
			InAnimation->mName.C_Str(),
			static_cast<float>(InAnimation->mDuration),
			static_cast<float>(InAnimation->mTicksPerSecond),
		};
		// mNumChannels : 애니메이션이 영향을 미치는 aiNode의 개수. aiNode는 Bone일수 있지만 반드시 Bone은 아니다
		const UINT ChannelsCount = InAnimation->mNumChannels;
#ifdef DO_DEBUG
		printf("Channels Count: %d\n", ChannelsCount);
#endif
		AnimationClipDataToReturn->NodeDatas.resize(ChannelsCount, nullptr);
		for (UINT i = 0; i < ChannelsCount; i++)
		{
			const aiNodeAnim * const NodeAnim = InAnimation->mChannels[i];

			AnimationClipDataToReturn->NodeDatas[i] = new ClipNodeData();
			ClipNodeData * const ClipDataNowMaking = AnimationClipDataToReturn->NodeDatas[i]; 
			ClipDataNowMaking->BoneName = NodeAnim->mNodeName.C_Str();
			ReadPosKeys(ClipDataNowMaking->PosKeys, NodeAnim);
			ReadScaleKeys(ClipDataNowMaking->ScaleKeys, NodeAnim);
			ReadRotKeys(ClipDataNowMaking->RotKeys, NodeAnim);
		}
		return AnimationClipDataToReturn;
	}

	void Converter::ReadPosKeys( vector<FrameDataVec> & OutPosKeys, const aiNodeAnim * InNodeAnim )
	{
		const UINT PosKeyCount = InNodeAnim->mNumPositionKeys;
		OutPosKeys.reserve(PosKeyCount);
		for (UINT i = 0; i < PosKeyCount; i++)
		{
			const aiVectorKey & PosKey = InNodeAnim->mPositionKeys[i];
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

		// InClipData : 하나의 Animation.
		// ReadClipData를 이용해서 Assimp가 읽은 aiScene데이터를 InClipData에 저장했다.
		// Animation Clip의 NodeData는 특정 Bone에 대한 KeyFrameData (Pos, Rot, Scale)를 갖는다.
		// 이 때 NodeData와 BoneName을 비교하여 AnimationClip의 NodeData가 어떤 Bone에 적용될 지 식별하기 위한 과정이다.
		const UINT NodeCount = InClipData->NodeDatas.size();
		for (UINT i = 0 ; i < NodeCount; i++)
		{
			ClipNodeData * NodeData = InClipData->NodeDatas[i];

			const string & NodeName = InNode->mName.C_Str();
			if (NodeName == NodeData->BoneName)
			{
				// NodeData와 Bone이 매칭됨.
				bFound = true;
				break;
			}
		}

		// InNode에 맞는 ClipData가 없다? 그럼 우짜노 -> 만들어준다.
		
		if (bFound == false)
		{
// #ifdef DO_DEBUG
// 			printf("Bone which is not connected to animation : %s\n", InNode->mName.C_Str());
// #endif
			ClipNodeData * NodeData = new ClipNodeData();

			Matrix TempMat = InNode->mTransformation;
			TempMat.Transpose();
			Vector T, S;
			Quaternion R;
			TempMat.Decompose(S, R, T);
			
			NodeData->BoneName = InNode->mName.C_Str();
			const UINT AnimationLength = static_cast<UINT>(InClipData->Duration) + 1;
			for (UINT f = 0; f < AnimationLength ; f++)
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