// #pragma once
//
// class Skeleton
// {
// public:
// 	using CachedBoneTableType = unordered_map<string, ModelBone *>;
// 	Skeleton();
// 	~Skeleton();
//
// 	int GetBoneCount() const { return Bones.size(); }
// 	void SetUpBoneTable(const vector<ModelMesh *> & Meshes);
// 	void ClearBoneTable();
// 	const CachedBoneTableType * GetCachedBoneTable() const { return CachedBoneTable; }
// 	// BoneDesc ConstantBuffer 생성
// 	void CreateBuffer( const map<string, Material *> & MaterialsInModel);
// 	// BoneDesc ConstantBuffer를 GPU에 바인딩.
// 	void BindToGPU() const;
// private:
// 	friend class Model;
// 	friend class ModelBone;
// 	friend class ModelAnimation;
// private:
// 	vector<ModelBone *> Bones;
// 	CachedBoneTableType * CachedBoneTable = nullptr;
//
// 	struct BoneDesc
// 	{
// 		Matrix OffsetMatrix[SkeletalMesh::MaxBoneCount];
// 		Matrix BoneTransform[SkeletalMesh::MaxBoneCount];
// 	};
// 	BoneDesc BoneData;
// 	ConstantBuffer * BoneDescBuffer;
// 	string CBufferName = "CB_ModelBones";
// 	// CB_MOdelBones라는 이름의 ConstantBuffer를 다루는 Handle
// 	vector<IECB_t *> ECB_BoneDescBuffers; 
// };
