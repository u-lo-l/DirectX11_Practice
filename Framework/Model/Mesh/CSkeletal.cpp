#include "framework.h"
#include "CSkeletal.h"

CSkeletal::CSkeletal(const vector<CBone*>& InBones)
{
	Bones = InBones;

	const UINT BoneCount = Bones.size();
	for (UINT BoneIndex = 0; BoneIndex < BoneCount; BoneIndex++)
	{
		const CBone * const TargetBone = Bones[BoneIndex];
		BoneSearchMap[TargetBone->GetName()] = TargetBone;
		const Matrix & BoneMatrix = TargetBone->GetRootTransform();
		BoneMatrices[BoneIndex] = BoneMatrix;
		OffsetMatrices[BoneIndex] = Matrix::Invert(BoneMatrix, true);
	}
	SB_BoneMatrices = new RWStructuredBuffer(
		static_cast<UINT>(ShaderType::VertexShader),
		0,
		BoneMatrices.data(),
		BoneMatrices.size(),
		sizeof(Matrix)
	);
	CB_OffsetMatrices = new ConstantBuffer(
		ShaderType::VertexShader,
		2,
		OffsetMatrices.data(),
		"Model Offset Matrices",
		OffsetMatrices.size() * sizeof(Matrix),
		true
	);
}

CSkeletal::~CSkeletal()
{
	SAFE_DELETE(SB_BoneMatrices);
	SAFE_DELETE(CB_OffsetMatrices);
}

const CBone* CSkeletal::FindBone(const string& InBoneName) const
{
	BoneSearchMapType::const_iterator It = BoneSearchMap.find(InBoneName);
	if (It == BoneSearchMap.cend())
		return nullptr;
	return It->second; 
}

const CBone * CSkeletal::FindBone(int Index) const
{
	if (Index < 0 || Index >= Bones.size())
		return nullptr;
	return Bones[Index];
}

int CSkeletal::GetBoneCount() const
{
	return BoneSearchMap.size();
}

const array<Matrix, CSkeletal::MAX_BONE_COUNT>& CSkeletal::GetBoneMatrices() const
{ 
	return BoneMatrices; 
}

array<Matrix, CSkeletal::MAX_BONE_COUNT>& CSkeletal::GetBoneMatrices()
{ 
	return BoneMatrices; 
}

const array<Matrix, CSkeletal::MAX_BONE_COUNT>& CSkeletal::GetOffsetMatrices() const
{
	return OffsetMatrices;
}

void CSkeletal::BindToGPU() const
{
	if (!!SB_BoneMatrices)
		SB_BoneMatrices->BindToGPUAsSRV(3, (UINT)ShaderType::VertexShader);
	if (!!CB_OffsetMatrices)
		CB_OffsetMatrices->BindToGPU();
}
