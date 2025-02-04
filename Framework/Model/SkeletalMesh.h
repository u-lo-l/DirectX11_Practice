#pragma once
#include "ModelMesh.h"

// NOLINT(cppcoreguidelines-special-member-functions)
class SkeletalMesh final : public ModelMesh 
{
public:
	static constexpr UINT MaxBoneCount = 256;
	friend class Model;
public:
	SkeletalMesh();
private:
	virtual ~SkeletalMesh() override;
	void Tick(const ModelAnimation * CurrentAnimation = nullptr) override;
	void Render() override;
	void CreateBuffers() override;
	
	int GetBoneIndex() const;
	void SetBoneIndex( int InBoneIndex );
	void SetBoneTransforms( Matrix * Transforms );

#pragma region Bone
private:
	Matrix * Transforms;
	ModelBone * Bone = nullptr;

	struct BoneDesc
	{
		Matrix BoneTransforms[MaxBoneCount];
		UINT BoneIndex;
		float Padding[3];
	};
	BoneDesc BoneData;
	ConstantBuffer * BoneDescBuffer;
	IECB_t * ECB_BoneDescBuffer;
#pragma endregion Bone

};
