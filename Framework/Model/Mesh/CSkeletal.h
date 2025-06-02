#pragma once

class CBone;

class CSkeletal
{
public:
	constexpr static int MAX_BONE_COUNT = 512;
	using BoneSearchMapType = unordered_map<string, CBone *>;
	explicit CSkeletal(const vector<CBone *> & InBones);
	~CSkeletal();
	const CBone * FindBone(const string & InBoneName) const;
	const CBone * FindBone(int Index) const;
private:
	vector<CBone *> Bones;
	BoneSearchMapType BoneSearchMap;
	
	// Matrix OffsetMatrix[MAX_BONE_COUNT];
	// ConstantBuffer * CB_OffsetMatrix = nullptr;
};
