#include "framework.h"
#include "CSkeletal.h"

CSkeletal::CSkeletal(const vector<CBone*>& InBones)
{
	Bones = InBones;

	UINT BoneCount = Bones.size();
	
	for (UINT i = 0; i < BoneCount; i++)
	{
		CBone * TargetBone = Bones[i];
		BoneSearchMap[TargetBone->GetName()] = TargetBone;
	}
}

CSkeletal::~CSkeletal()
{
	// SAFE_DELETE(CB_OffsetMatrix);
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
