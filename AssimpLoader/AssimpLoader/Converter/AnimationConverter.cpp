#include "Pch.h"
#include "AnimationConverter.h"
#include "AssimpLoader/Types.h"

struct ClipNodeData;

AnimationConverter::AnimationConverter()
{
}

AnimationConverter::~AnimationConverter()
{
}

void AnimationConverter::ReadAiScene(const wstring& InFileName)
{
	const string FullPath = ASSET_PATH + String::ToString(InFileName);
	const aiScene * Scene = Importer->ReadFile(FullPath.c_str(),ConvertFlag);
	ASSERT(!!Scene, Importer->GetErrorString());

	ExportAnimation(InFileName, Scene);

	Importer->FreeScene();
}

void AnimationConverter::ExportAnimation(const wstring& InFileName, const aiScene* InScene)
{
	const wstring AnimationDirectory = Path::GetDirectoryName(W_ANIMATION_PATH + InFileName);
	const wstring AnimationName = Path::GetFileNameWithoutExtension(InFileName);

	// Scene에 있는 ClipFrameData(aiNodeAnim)정보를 읽는다.
	const unsigned int AnimationCount = InScene->mNumAnimations;

	ASSERT(AnimationCount != 0, "Animation Not Found");
	ASSERT(AnimationCount <= 1, "Too Many Animations In One File");

	const wstring AnimName = AnimationDirectory + AnimationName + L".anim";
	ClipData * const Clip = ReadClipData(InScene->mAnimations[0], InScene->mRootNode);

	// ReadClipData내부에서 만들어도 되는데 일단은 여기 두자.
	set<string> BoneSearchTree;
	const unsigned int NodeCount = Clip->NodeDatas.size();
	for (unsigned int j = 0; j < NodeCount; ++j)
	{
		const ClipNodeData * Node = Clip->NodeDatas[j];
		BoneSearchTree.insert(Node->BoneName);
	}
	ConnectNodeWithBone(Clip, InScene->mRootNode, BoneSearchTree);
	WriteClipData(String::ToString(AnimName), Clip);
}

ClipData * AnimationConverter::ReadClipData( const aiAnimation * InAnimation, const aiNode * RootNode )
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

	AnimationClipDataToReturn->NodeDatas.resize(ChannelsCount + 1, nullptr);
	{
		AnimationClipDataToReturn->NodeDatas[0] = new ClipNodeData();
		ClipNodeData * const ClipDataNowMaking = AnimationClipDataToReturn->NodeDatas[0]; 
		ClipDataNowMaking->BoneName = RootNode->mName.C_Str();
		ClipDataNowMaking->PosKeys.push_back({0, {0,0,0}});
		ClipDataNowMaking->ScaleKeys.push_back({0, {1,1,1}});
		ClipDataNowMaking->RotKeys.push_back({0, Quaternion::CreateFromEulerAngleInRadian({0, PRE_Y_ROTATION, 0})});
	}
	for (UINT i = 0; i < ChannelsCount; i++)
	{
		// aiNodeAnim이란 특정 Node에 대한 애니메이션 데이터를 저장하며, TRS KeySquence를 갖는다.
		const aiNodeAnim * const NodeAnim = InAnimation->mChannels[i];

		UINT j = i + 1;
		AnimationClipDataToReturn->NodeDatas[j] = new ClipNodeData();
		ClipNodeData * const ClipDataNowMaking = AnimationClipDataToReturn->NodeDatas[j]; 
		ClipDataNowMaking->BoneName = NodeAnim->mNodeName.C_Str();

		// 모든 KeySequence들은 부모 노드에 상대적인 값으로 저장된다. 따라서 이후에 사용할 때는 좌표변환 해서 사용해야 한다.
		ReadPosKeySequences(ClipDataNowMaking->PosKeys, NodeAnim, AnimationClipDataToReturn);
		ReadScaleKeySequences(ClipDataNowMaking->ScaleKeys, NodeAnim, AnimationClipDataToReturn);
		ReadRotKeySequences(ClipDataNowMaking->RotKeys, NodeAnim, AnimationClipDataToReturn);
	}
	return AnimationClipDataToReturn;
}

void AnimationConverter::ReadPosKeySequences( vector<FrameDataVec> & OutPosKeys, const aiNodeAnim * InNodeAnim, const ClipData * const InClipData)
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
			const UINT TargetFrame = static_cast<UINT>(NextPosKey.mTime * (DefaultTicksPerSecond));
			if (i > TargetFrame)
			{
				KeySequenceIndex = min(KeySequenceIndex + 1, MaxKeySequenceIndex);
				NextKeySequenceIndex = min(NextKeySequenceIndex + 1, MaxKeySequenceIndex);
			}
			OutPosKeys.emplace_back(static_cast<float>(i), static_cast<Vector>(PosKey.mValue));
		}
	}
}

void AnimationConverter::ReadScaleKeySequences( vector<FrameDataVec> & OutScaleKeys, const aiNodeAnim * InNodeAnim, const ClipData * const InClipData)
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
			const UINT TargetFrame = static_cast<UINT>(NextScaleKey.mTime * (DefaultTicksPerSecond));
			if (i > TargetFrame)
			{
				KeySequenceIndex = min(KeySequenceIndex + 1, MaxKeySequenceIndex);
				NextKeySequenceIndex = min(NextKeySequenceIndex + 1, MaxKeySequenceIndex);
			}
			OutScaleKeys.emplace_back(static_cast<float>(i), static_cast<Vector>(ScaleKey.mValue));
		}
	}
}
void AnimationConverter::ReadRotKeySequences( vector<FrameDataQuat> & OutRotKeys, const aiNodeAnim * InNodeAnim, const ClipData * const InClipData)
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
			const UINT TargetFrame = static_cast<UINT>(NextRotKey.mTime * (DefaultTicksPerSecond));
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
void AnimationConverter::ConnectNodeWithBone( ClipData * InOutClipData, const aiNode * InNode,  const set<string> & InBoneNames_BinTree )
{
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


void AnimationConverter::WriteClipData( const string & InSaveFileName, const ClipData * InClipData )
{
	Path::CreateFolders(Path::GetDirectoryName(InSaveFileName));

	const BinaryWriter * BinWriter = new BinaryWriter(InSaveFileName);

	// BinWriter->WriteString(InClipData->Name);
	BinWriter->WriteString(Path::GetFileNameWithoutExtension(InSaveFileName));
	BinWriter->WriteFloat(InClipData->Duration);
	BinWriter->WriteFloat(InClipData->TicksPerSecond);

	BinWriter->WriteUint(InClipData->NodeDatas.size());
	for (const ClipNodeData * const ClipNodeData : InClipData->NodeDatas)
	{
		BinWriter->WriteString(ClipNodeData->BoneName);
		BinWriter->WriteSTDVector<FrameDataVec>(ClipNodeData->PosKeys);
		BinWriter->WriteSTDVector<FrameDataVec>(ClipNodeData->ScaleKeys);
		BinWriter->WriteSTDVector<FrameDataQuat>(ClipNodeData->RotKeys);
	}
	SAFE_DELETE(BinWriter);
}
