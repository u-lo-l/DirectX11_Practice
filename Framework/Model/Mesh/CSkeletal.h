#pragma once
#include <unordered_map>

class RWStructuredBuffer;
class StructuredBuffer;
class CBone;

class CSkeletal
{
public:
	constexpr static int MAX_BONE_COUNT = 256;
	using BoneSearchMapType = unordered_map<string, const CBone *>;
	explicit CSkeletal(const vector<CBone *> & InBones);
	~CSkeletal();
	const CBone * FindBone(const string & InBoneName) const;
	const CBone * FindBone(int Index) const;

	int GetBoneCount() const;
	const array<Matrix, MAX_BONE_COUNT> & GetBoneMatrices() const;
	array<Matrix, MAX_BONE_COUNT> & GetBoneMatrices();
	RWStructuredBuffer* GetBoneMatrices_Buffer() const;
	const array<Matrix, MAX_BONE_COUNT> & GetOffsetMatrices() const;
	
	void BindToGPU(int OffsetMatrixCBRegister, int BoneMatrixSRVRegister) const;
private:
	vector<CBone *> Bones;
	BoneSearchMapType BoneSearchMap;

	array<Matrix, MAX_BONE_COUNT> BoneMatrices;
	array<Matrix, MAX_BONE_COUNT> OffsetMatrices;

	RWStructuredBuffer * SB_BoneMatrices = nullptr;
	ConstantBuffer * CB_OffsetMatrices = nullptr;
};
