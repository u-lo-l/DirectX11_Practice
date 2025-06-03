#include "framework.h"
#include "CSkeletal.h"

CSkeletal::CSkeletal(const vector<CBone*>& InBones)
{
	Bones = InBones;

	const UINT BoneCount = Bones.size();
	for (UINT BoneIndex = 0; BoneIndex < BoneCount; BoneIndex++)
	{
		const CBone * const TargetBone = Bones[BoneIndex];
		// BoneSearchMap[TargetBone->GetName()] = TargetBone;
		const Matrix & BoneMatrix = TargetBone->GetRootTransform();
		BoneMatrixData.BoneMatrices[BoneIndex] = BoneMatrix;
		BoneMatrixData.OffsetMatrices[BoneIndex] = Matrix::Invert(BoneMatrix, true);
	}
	CB_BoneMatrices = new ConstantBuffer(
		ShaderType::VertexShader,
		2,
		&BoneMatrixData,
		"Model Bone Matrices",
		sizeof(BoneMatrixDesc)
	);
}

CSkeletal::~CSkeletal()
{
	SAFE_DELETE(CB_BoneMatrices);
}

// const CBone* CSkeletal::FindBone(const string& InBoneName) const
// {
// 	BoneSearchMapType::const_iterator It = BoneSearchMap.find(InBoneName);
// 	if (It == BoneSearchMap.cend())
// 		return nullptr;
// 	return It->second; 
// }
//
// const CBone * CSkeletal::FindBone(int Index) const
// {
// 	if (Index < 0 || Index >= Bones.size())
// 		return nullptr;
// 	return Bones[Index];
// }

// const array<Matrix, CSkeletal::MAX_BONE_COUNT>& CSkeletal::GetBoneMatrices() const
// { 
// 	return BoneMatrices; 
// }
//
// array<Matrix, CSkeletal::MAX_BONE_COUNT>& CSkeletal::GetBoneMatrices()
// { 
// 	return BoneMatrices; 
// }
//
// const array<Matrix, CSkeletal::MAX_BONE_COUNT>& CSkeletal::GetOffsetMatrices() const
// {
// 	return OffsetMatrices;
// }

void CSkeletal::BindToGPU() const
{
	if (!!CB_BoneMatrices)
		CB_BoneMatrices->BindToGPU();
}
