#pragma once

class Skeleton
{
public:
	using CachedBoneTableType = unordered_map<string, ModelBone *>;
	Skeleton();
	~Skeleton();

	int GetBoneCount() const { return Bones.size(); }
	void SetUpBoneTable(const vector<ModelMesh *> & Meshes);
	void ClearBoneTable();
	const CachedBoneTableType * GetCachedBoneTable() const { return CachedBoneTable; }
private:
	friend class Model;
	friend class ModelBone;
	friend class ModelAnimation;
private:
	ModelBone * RootBone;
	vector<ModelBone *> Bones;
	Matrix BoneTransforms[SkeletalMesh::MaxBoneCount];
	CachedBoneTableType * CachedBoneTable = nullptr;
};
