#pragma once

class CBone;

class CSkeletal
{
public:
	constexpr static int MAX_BONE_COUNT = 256;
	using BoneSearchMapType = unordered_map<string, CBone *>;
	explicit CSkeletal(const vector<CBone *> & InBones);
	~CSkeletal();
	// const CBone * FindBone(const string & InBoneName) const;
	// const CBone * FindBone(int Index) const;

	// const array<Matrix, MAX_BONE_COUNT> & GetBoneMatrices() const;
	// array<Matrix, MAX_BONE_COUNT> & GetBoneMatrices();
	// const array<Matrix, MAX_BONE_COUNT> & GetOffsetMatrices() const;
	
	void BindToGPU() const;
private:
	vector<CBone *> Bones;
	// BoneSearchMapType BoneSearchMap;

	struct BoneMatrixDesc
	{
		Matrix BoneMatrices[MAX_BONE_COUNT];
		Matrix OffsetMatrices[MAX_BONE_COUNT];
	} BoneMatrixData;
	ConstantBuffer * CB_BoneMatrices = nullptr;
	// ConstantBuffer * CB_OffsetMatrices = nullptr;
};
