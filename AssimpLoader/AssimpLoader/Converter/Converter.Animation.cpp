#include "Pch.h"
#include "Converter.h"
#include <fstream>

namespace Sdt
{
	void Converter::ExportAnimation( const string & InSaveFileName, int InClipIndex )
	{
		const string SaveFileName = MODEL_PATH + InSaveFileName + ".animation";

		// Scene에 있는 ClipFrameData(aiNodeAnim)정보를 읽는다.
		ClipData * const Clip = ReadClipData(Scene->mAnimations[InClipIndex]);

		
		// ReadClipData내부에서 만들어도 되는데 일단은 여기 두자.
		set<string> BoneNamesInModel_BinTree;
		const UINT NodeCount = Clip->NodeDatas.size();
		for (UINT i = 0; i < NodeCount; ++i)
		{
			ClipNodeData * Node = Clip->NodeDatas[i];
			BoneNamesInModel_BinTree.insert(Node->BoneName);
		}		
		ConnectNodeWithBone(Clip, Scene->mRootNode, BoneNamesInModel_BinTree);
		
		WriteClipData(SaveFileName, Clip);
	}

	
	ClipData * Converter::ReadClipData( const aiAnimation * InAnimation )
	{
		ClipData * AnimationClipDataToReturn = new ClipData{
			(InAnimation->mTicksPerSecond != 1),
			InAnimation->mName.C_Str(),
			static_cast<float>(InAnimation->mDuration),
			static_cast<float>(InAnimation->mTicksPerSecond),
		};
		// mNumChannels : 애니메이션이 영향을 미치는 aiNode의 개수. aiNode는 Bone일수 있지만 반드시 Bone은 아니다
		if (AnimationClipDataToReturn->bDiscrete == false)
		{
			AnimationClipDataToReturn->Duration = static_cast<float>(static_cast<UINT>(InAnimation->mDuration * DefaultTicksPerSecond));
			AnimationClipDataToReturn->TicksPerSecond = DefaultTicksPerSecond;
		}
		const UINT ChannelsCount = InAnimation->mNumChannels;
		
#ifdef DO_DEBUG
		printf("Channels Count: %d\n", ChannelsCount);
#endif
		
		AnimationClipDataToReturn->NodeDatas.resize(ChannelsCount, nullptr);
		for (UINT i = 0; i < ChannelsCount; i++)
		{
			// aiNodeAnim이란 특정 Node에 대한 애니메이션 데이터를 저장하며, TRS KeySquence를 갖는다.
			const aiNodeAnim * const NodeAnim = InAnimation->mChannels[i];

			AnimationClipDataToReturn->NodeDatas[i] = new ClipNodeData();
			ClipNodeData * const ClipDataNowMaking = AnimationClipDataToReturn->NodeDatas[i]; 
			ClipDataNowMaking->BoneName = NodeAnim->mNodeName.C_Str();

			// 모든 KeySequence들은 부모 노드에 상대적인 값으로 저장된다. 따라서 이후에 사용할 때는 좌표변환 해서 사용해야 한다.
			ReadPosKeySequences(ClipDataNowMaking->PosKeys, NodeAnim, AnimationClipDataToReturn);
			ReadScaleKeySequences(ClipDataNowMaking->ScaleKeys, NodeAnim, AnimationClipDataToReturn);
			ReadRotKeySequences(ClipDataNowMaking->RotKeys, NodeAnim, AnimationClipDataToReturn);
		}
		return AnimationClipDataToReturn;
	}

	void Converter::ReadPosKeySequences( vector<FrameDataVec> & OutPosKeys, const aiNodeAnim * InNodeAnim, const ClipData * const InClipData)
	{
		if (InClipData->bDiscrete == true)
		{
			const UINT PosKeyCount = InNodeAnim->mNumPositionKeys;
			OutPosKeys.reserve(PosKeyCount);
			for (UINT i = 0; i < PosKeyCount; i++)
			{
				const aiVectorKey & PosKey = InNodeAnim->mPositionKeys[i];
				OutPosKeys.emplace_back(static_cast<float>(PosKey.mTime), static_cast<Vector>(PosKey.mValue));
			}
		}
		else
		{
			const UINT PosKeyCount = InNodeAnim->mNumPositionKeys == 1 ? 1 : 1 + static_cast<UINT>(InClipData->Duration);
			OutPosKeys.reserve(PosKeyCount);
			UINT KeySequenceIndex = 0;
			UINT NextKeySequenceIndex = 1;
			const UINT MaxKeySequenceIndex = InNodeAnim->mNumPositionKeys - 1;
			for (UINT i = 0; i < PosKeyCount; i++) // PosKeyCount = Duration + 1 = Animation Clip Length
			{
				const aiVectorKey & PosKey = InNodeAnim->mPositionKeys[KeySequenceIndex];
				const aiVectorKey & NextPosKey = InNodeAnim->mPositionKeys[NextKeySequenceIndex];
				const UINT TargetFrame = static_cast<UINT>(NextPosKey.mTime * (ThisClass::DefaultTicksPerSecond));
				if (i > TargetFrame)
				{
					KeySequenceIndex = min(KeySequenceIndex + 1, MaxKeySequenceIndex);
					NextKeySequenceIndex = min(NextKeySequenceIndex + 1, MaxKeySequenceIndex);
				}
				OutPosKeys.emplace_back(static_cast<float>(i), static_cast<Vector>(PosKey.mValue));
			}
		}
	}
	void Converter::ReadScaleKeySequences( vector<FrameDataVec> & OutScaleKeys, const aiNodeAnim * InNodeAnim, const ClipData * const InClipData)
	{
		if (InClipData->bDiscrete == true)
		{
			const UINT ScaleKeyCount = InNodeAnim->mNumScalingKeys;
			OutScaleKeys.reserve(ScaleKeyCount);
			for (UINT i = 0; i < ScaleKeyCount; i++)
			{
				const aiVectorKey & ScaleKey =  InNodeAnim->mScalingKeys[i];
				OutScaleKeys.emplace_back(static_cast<float>(ScaleKey.mTime), static_cast<Vector>(ScaleKey.mValue));
			}
		}
		else
		{
			const UINT ScaleKeyCount = InNodeAnim->mNumScalingKeys == 1 ? 1 : 1 + static_cast<UINT>(InClipData->Duration);
			OutScaleKeys.reserve(ScaleKeyCount);
			UINT KeySequenceIndex = 0;
			UINT NextKeySequenceIndex = 1;
			const UINT MaxKeySequenceIndex = InNodeAnim->mNumScalingKeys - 1;
			for (UINT i = 0; i < ScaleKeyCount; i++) // ScaleKeyCount = Duration + 1 = Animation Clip Length
			{
				const aiVectorKey & ScaleKey = InNodeAnim->mScalingKeys[KeySequenceIndex];
				const aiVectorKey & NextScaleKey = InNodeAnim->mScalingKeys[NextKeySequenceIndex];
				const UINT TargetFrame = static_cast<UINT>(NextScaleKey.mTime * (ThisClass::DefaultTicksPerSecond));
				if (i > TargetFrame)
				{
					KeySequenceIndex = min(KeySequenceIndex + 1, MaxKeySequenceIndex);
					NextKeySequenceIndex = min(NextKeySequenceIndex + 1, MaxKeySequenceIndex);
				}
				OutScaleKeys.emplace_back(static_cast<float>(i), static_cast<Vector>(ScaleKey.mValue));
			}
		}
	}
	void Converter::ReadRotKeySequences( vector<FrameDataQuat> & OutRotKeys, const aiNodeAnim * InNodeAnim, const ClipData * const InClipData)
	{
		if (InClipData->bDiscrete == true)
		{
			const UINT RotKeyCount = InNodeAnim->mNumRotationKeys;
			OutRotKeys.reserve(RotKeyCount);
			for (UINT i = 0; i < RotKeyCount; i++)
			{
				const aiQuatKey & RotKey =  InNodeAnim->mRotationKeys[i];
				OutRotKeys.emplace_back(static_cast<float>(RotKey.mTime), static_cast<Quaternion>(RotKey.mValue));
			}
		}
		else
		{
			const UINT RotKeyCount = InNodeAnim->mNumRotationKeys == 1 ? 1 : 1 + static_cast<UINT>(InClipData->Duration);
			OutRotKeys.reserve(RotKeyCount);
			UINT KeySequenceIndex = 0;
			UINT NextKeySequenceIndex = 1;
			const UINT MaxKeySequenceIndex = InNodeAnim->mNumRotationKeys - 1;
			for (UINT i = 0; i < RotKeyCount; i++) // RotKeyCount = Duration + 1 = Animation Clip Length
			{
				const aiQuatKey & RotKey = InNodeAnim->mRotationKeys[KeySequenceIndex];
				const aiQuatKey & NextRotKey = InNodeAnim->mRotationKeys[NextKeySequenceIndex];
				const UINT TargetFrame = static_cast<UINT>(NextRotKey.mTime * (ThisClass::DefaultTicksPerSecond));
				if (i > TargetFrame)
				{
					KeySequenceIndex = min(KeySequenceIndex + 1, MaxKeySequenceIndex);
					NextKeySequenceIndex = min(NextKeySequenceIndex + 1, MaxKeySequenceIndex);
				}
				OutRotKeys.emplace_back(static_cast<float>(i), static_cast<Quaternion>(RotKey.mValue));
			}
		}
	}

	/**
	 * @breif Missing된 Bonen에 정보를 써 준다?
	 * @param InOutClipData : 수정될 ClipData
	 * @param InNode : ClipData를 수정하기 위해 참조될 aiNode
	 * @param InBoneNames_BinTree : BoneName검색을 위한 이진탐색트리
	 */
	void Converter::ConnectNodeWithBone( ClipData * InOutClipData, const aiNode * InNode,  const set<string> & InBoneNames_BinTree )
	{
		// InClipData : 하나의 Animation.
		// ReadClipData를 이용해서 Assimp가 읽은 aiScene데이터를 InClipData에 저장했다.
		// Animation Clip의 NodeData는 특정 Bone에 대한 KeyFrameData (Pos, Rot, Scale)를 갖는다.
		// 이 때 NodeData와 BoneName을 비교하여 AnimationClip의 NodeData가 어떤 Bone에 적용될 지 식별하기 위한 과정이다.
		const string BoneName = InNode->mName.C_Str();
		// Matching Bone Not Found
		if (InBoneNames_BinTree.find(BoneName) == InBoneNames_BinTree.cend())
		{
			Matrix Mat = InNode->mTransformation;
			Mat.Transpose();

			Vector S, T;
			Quaternion R;
			Mat.Decompose(S, R, T);

			// 어차피 다 같은거 들어가는데, 길이 1이여도 똑같음. -> for문 제거
			ClipNodeData * NodeData = new ClipNodeData();
			NodeData->BoneName = move(BoneName);
			NodeData->PosKeys.emplace_back(0.f, T);
			NodeData->ScaleKeys.emplace_back(0.f, S);
			NodeData->RotKeys.emplace_back(0.f, R);
			InOutClipData->NodeDatas.push_back(NodeData);
		}
		
		const UINT ChildCount = InNode->mNumChildren;
		for (UINT i = 0 ; i < ChildCount; i++)
		{
			ConnectNodeWithBone(InOutClipData, InNode->mChildren[i], InBoneNames_BinTree);
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
			BinWriter->WriteSTDVector<FrameDataVec>(clipNodeData->PosKeys);
			BinWriter->WriteSTDVector<FrameDataVec>(clipNodeData->ScaleKeys);
			BinWriter->WriteSTDVector<FrameDataQuat>(clipNodeData->RotKeys);
		}
		SAFE_DELETE(BinWriter);
	}
}