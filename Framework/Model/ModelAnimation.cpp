﻿#include "framework.h"
#include "ModelAnimation.h"
#include <fstream>

ModelAnimation::~ModelAnimation()
{
	for (const KeyFrameData * Data : KeyFrames)
		SAFE_DELETE(Data);
}

// Assimp로 읽어서 나름대로 정리하여 저장한 .animation파일을 읽어서 다시 구성하는 과정.
ModelAnimation * ModelAnimation::ReadAnimationFile
(
	const BinaryReader * InReader,
	const Model::CachedBoneTableType * InCachedBoneTable
)
{
	ModelAnimation * ModelAnimationToReturn = new ModelAnimation();

	ModelAnimationToReturn->Name = InReader->ReadString();
	ModelAnimationToReturn->Duration = InReader->ReadFloat();
	ModelAnimationToReturn->TicksPerSecond = InReader->ReadFloat();

	const UINT KeyFrameCount = InReader->ReadUint();
	
	// TODO : InBoneTable이 Nullptr일 떄 다시 생각해보자. 지금은 그냥 넘어감.
	ModelAnimationToReturn->KeyFrames.assign(KeyFrameCount, nullptr);
	for (UINT i = 0; i < KeyFrameCount; i++)
	{
		ModelAnimationToReturn->KeyFrames[i] = new KeyFrameData();
		KeyFrameData * &TargetData = ModelAnimationToReturn->KeyFrames[i];
		const string BoneNameInAnimation = InReader->ReadString();
		if (InCachedBoneTable == nullptr)
			continue;
		const auto Iterator = InCachedBoneTable->find(BoneNameInAnimation);
		if (Iterator != InCachedBoneTable->cend())
		{
			TargetData->BoneIndex = Iterator->second->Index;
			TargetData->BoneName = Iterator->second->Name;
		}
		else
		{
			// 이 친구들은 버린다?
			// 여기서 BoneIndex == -1인 KeyFrame이 의마하는게 뭘까?
			TargetData->BoneIndex = -1;
			TargetData->BoneName = BoneNameInAnimation;
		}
		
		const UINT PosCount = InReader->ReadUint();
		TargetData->Positions.resize(PosCount);
		if (PosCount > 0)
		{
			const UINT ReadSize = sizeof(FrameDataVec) * PosCount;
			void * Ptr = TargetData->Positions.data();
			InReader->ReadByte(&Ptr, ReadSize);
		}

		const UINT ScaleCount = InReader->ReadUint();
		TargetData->Scales.resize(ScaleCount);
		if (ScaleCount > 0)
		{
			const UINT ReadSize = sizeof(FrameDataVec) * ScaleCount;
			void * Ptr = TargetData->Scales.data();
			InReader->ReadByte(&Ptr, ReadSize);
		}

		const UINT RotCount = InReader->ReadUint();
		TargetData->Rotations.resize(RotCount);
		if (RotCount > 0)
		{
			const UINT ReadSize = sizeof(FrameDataQuat) * RotCount;
			void * Ptr = TargetData->Rotations.data();
			InReader->ReadByte(&Ptr, ReadSize);
		}
	}
	return ModelAnimationToReturn;
}

float ModelAnimation::CalculateNextTime( float CurrentTime, float DeltaTime ) const
{
	DeltaTime *= TicksPerSecond * PlayRate;
	CurrentTime += DeltaTime;
	return fmod(CurrentTime, GetAnimationLength());
}

std::pair<int, int> ModelAnimation::GetCurrentAndNextFrame( float CurrentTime ) const
{
	const int Length = static_cast<int>(GetAnimationLength());
	int CurrentFrame = static_cast<int>(CurrentTime);
	int NextFrame = (1 + CurrentFrame) % Length;
	return {CurrentFrame, NextFrame};
}

/*=================================================================================================*/

/**
 * @brief KeyFrame Animation 데이터를 배열로 표현한 구조.
 * @param InSkeleton <code>Model</code> 클래스는 <code>Skeleton*</code>를 갖고 있다.
 * @param CachedBoneTable
 * 
 * @details 
 * - KeyFrameTFTable 배열 구조:<br/>
 *         Bone ｜ _0_  | _1_ | _2_ | _3_ | _4_ | _5_ | _6_ | ... | MaxBoneCount-1      <br/>
 *         _Key ｜ M11 | M12 | M13 | M14 | ...                                         <br/>
 *         __0_ ｜ M21 | M22 | M23 | M24 | ...                                         <br/>
 *         __1_ ｜ M31 | M32 | M33 | M34 | ...                                         <br/>
 *         __2_ ｜ M41 | M42 | M43 | M44 | ...                                         <br/>
 *         __3_ ｜ M51 | M52 | M53 | M54 | ...                                         <br/>
 *          ..<br/>
 *    MaxKeyCount-1｜                                                                  <br/>
 *	 @note
 *   <b>InBone</b>의 ModelBone들의 Transform은 전부 <b>World-Coordinate(=Global-Coordinate)</b>으로 변환돼 있다.<br/>
 *   이를 다시 <b>'Parent-Bone에 대한 Local-Coordinate(=상대좌표)'</b>으로 변환하는 과정이 필요하다.
 *   
 *   <br/><br/><br/>
 *   @note 여기서 주목해야 할 점이 2가지이다:<br/>
 *   1) <b>KeyFrame</b>의 <b>BoneB</b>에 대한 정보는 parent인 <b>BoneA</b>의 <b>Local-Coordinate</b>을 기준으로 표현된 정보라는 점.<br/>
 *   2) 하지만 렌더링하기 위해 지금 프로젝트에서 사용하는 <b>ModelBone</b>들은 <b>World-Coordinate</b>이 기준이라는 점.<br/>
 *   그래서 <b>KeyFrameTable</b>에 기록된 <b>BoneB</b>의 정보를 <b>World-Coord</b>로 변환하는 과정이 필요하다.<br/><br/>
 *   <b>World</b>에서 본 <b>BoneB</b>의 Transform을 <b>W_T_B</b>라고 하고,<br/> <b>BoneA</b>에서 본 <b>BoneB</b>의 Transform을 <b>A_T_B</b>라고 하고<br/> <b>BoneB</b>에서 본 <b>BoneB</b>의
 *   Transform을 <b>B_T_B</b>라고 하자.<br/> <b>B_T_B</b>는 <b>I</b>(4x4 단위행렬)이다.<br/>
 *   이 때 <b>W_T_B = 'AtoW_Mat' * A_T_B</b>, <b>A_T_B = 'WtoA_Mat' * W_T_B</b>라고 할 수 있다.<br/>
 *   <b>AtoW_Mat</b>과 <b>WtoA_Mat</b>은 각각 <b>A</b>좌표계로부터 <b>World</b>좌표계로의 변환, <b>World</b>좌표계로부터 <b>A</b>좌표계로의 변환을 의미한다.<br/>
 *   또한 <b>AtoW_Mat * WtoA_Mat = I</b>이다.<br/><br/>
 *   [!] 여기서 <b>AtoW_Mat</b>은 바로 <b>W_T_A</b>이다. (<b>World</b>에서 본 <b>BoneA</b>의 Transform)<br/>
 *   만약 <b>Bone</b> 구조가 <b>Root->BoneA->BoneB</b>이라고 하자.<br/>
 *   <b>W_T_R</b>은 <b>World</b>에서 본 <b>RootNode</b>의 Transform(location, rotation, scale)이다.<br/>
 *   <b>W_T_A = R_T_A * RootToWorld_Mat = A_T_A * AToRoot_Mat * RootToWorld_Mat = I * AToRoot_Mat * RootToWorld_Mat</b>다.<br>
 *   <b>AToRoot_Mat * RootToWorld_Mat = AToWorld_Mat</b>이므로 위에서 말했던 <b>AtoW_Mat == W_T_A</b>가 맞아.<br/>
 *   즉 <b>KeyFrame</b>에 작성된 <b>BoneB</b>의 <b>Local-Transform(A_T_B)</b>을 <b>World-Transform(W_T_B)</b>으로 변환하기 위해선, <b>BoneA</b>에 대한 <b>World-Transform(W_T_A)</b>이 필요해.<br/>
 *   <b>W_T_B = A_T_B * AtoWorld_Mat = A_T_B * W_T_A</b>가 된다.
 */
ModelAnimation::KeyFrameTFTable * ModelAnimation::CalcClipTransform( const Skeleton * InSkeleton ) const
{
	KeyFrameTFTable * TransformTableToReturn = new KeyFrameTFTable();
	
#pragma region Configure Temp FrameNameBinTree
	map<string, KeyFrameData *> KeyFrameSearchTree;
	const UINT KeyFrameCount = KeyFrames.size();
	for (UINT i = 0; i < KeyFrameCount; i++)
	{
		KeyFrameSearchTree[KeyFrames[i]->BoneName] = KeyFrames[i];
	}
#pragma endregion
	
	// 길이 Frame'0'부터 Frame 'Duration'까지 Animation Length가 Duration + 1인 애니메이션의 각 프레임에 대해서 반복한다.  
	const UINT AnimationLength = static_cast<UINT>(GetAnimationLength());
	for (UINT CurrentFrame = 0; CurrentFrame < AnimationLength ; CurrentFrame++)
	{
		Matrix * const BoneMatrixUpdatingArr = TransformTableToReturn->TransformMats[CurrentFrame];

		// Animation을 수행 할 Model에 속한 모든 Bone에 대해서 반복한다.
		const UINT BoneCount = InSkeleton->Bones.size();
		for (UINT BoneNum = 0; BoneNum < BoneCount ; BoneNum++)
		{
			const ModelBone * const TargetBone = InSkeleton->Bones[BoneNum];

			// 현재 Animation의 모든 NodeData에서 현재 Bone에 대한 NodeData를 찾는다.
			const auto It = KeyFrameSearchTree.find(TargetBone->Name);
			if (It == KeyFrameSearchTree.cend())
				continue;
			const KeyFrameData * const TargetKeyFrameData = It->second;
			
			// 현재 Bone에 대한 NodeData를 찾았다면 해당 Bone의 F번쨰 프레임의 TRS를 가져온다.
			// 이 TRS는 Parent-Coordinate기준 정보다.
			const Vector & Pos     = TargetKeyFrameData->Positions.size() == 1 ? TargetKeyFrameData->Positions[0].Value : TargetKeyFrameData->Positions[CurrentFrame].Value;
			const Vector & Scale   = TargetKeyFrameData->Scales.size()    == 1 ? TargetKeyFrameData->Scales[0].Value    : TargetKeyFrameData->Scales[CurrentFrame].Value;
			const Quaternion & Rot = TargetKeyFrameData->Rotations.size() == 1 ? TargetKeyFrameData->Rotations[0].Value : TargetKeyFrameData->Rotations[CurrentFrame].Value;
			
			Matrix S = Matrix::CreateScale(Scale);
			Matrix R = Matrix::CreateFromQuaternion(Rot);
			Matrix T = Matrix::CreateTranslation(Pos);
			// 여기서 만들어진 AnimationMatrix는 ParentNode의 Local-Coordinate가 기준이다.
			Matrix AnimationMatrix = S * R * T; // A_T_B : SRT of B from A-coordinate
			if (TargetBone->IsRootBone() == true)
			{
				BoneMatrixUpdatingArr[BoneNum] = AnimationMatrix; // BoneMatrixArr[b] = AnimationMatrix * Matrix::Identity;
			}
			else
			{
				const Matrix & ParentMat = BoneMatrixUpdatingArr[TargetBone->ParentIndex]; // W_T_A. 이미 업데이트 된 부모노드의 World-Transform.
				BoneMatrixUpdatingArr[BoneNum] = AnimationMatrix * ParentMat;// W_T_B = A_T_B * W_T_A
			}
		} // for(b)
	} // for(f)
	return TransformTableToReturn;
}

ModelAnimation::KeyFrameData::KeyFrameData( int InBoneIndex, string InBoneName )
	: BoneIndex(InBoneIndex)
	, BoneName(std::move(InBoneName))
{
}

ModelAnimation::KeyFrameData::~KeyFrameData()
{
#ifdef DO_DEBUG
	printf("KeyFrameData Destruct : [%d], <%s>\n", BoneIndex, BoneName.c_str());
#endif
}

ModelAnimation::KeyFrameTFTable::KeyFrameTFTable()
{
	TransformMats = new Matrix* [MaxFrameLength];

	for (UINT i = 0; i < MaxFrameLength; i++)
		TransformMats[i] = new Matrix[SkeletalMesh::MaxBoneCount];
}

ModelAnimation::KeyFrameTFTable::~KeyFrameTFTable()
{
	for (UINT i = 0; i < MaxFrameLength; i++)
		SAFE_DELETE_ARR(TransformMats[i]);
	SAFE_DELETE_ARR(TransformMats);
}