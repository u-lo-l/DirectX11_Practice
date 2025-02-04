#pragma once
#include "ModelMesh.h"

// NOLINT(cppcoreguidelines-special-member-functions)
class SkeletalMesh final : public ModelMesh 
{
public:
	static constexpr UINT MaxBoneCount = 256;
	friend class Model;
	friend class Skeleton;
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
	void SetOffsetMatrix( Matrix * Transforms );

#pragma region Bone
private:
	Matrix * OffsetMatrix;
	Matrix * BoneTransforms;
	ModelBone * Bone = nullptr;

	struct BoneDesc
	{
		Matrix OffsetMatrix[MaxBoneCount];
		Matrix BoneTransform[MaxBoneCount];
		UINT BoneIndex;
		float Padding[3];
	};
	// TODO : 얘도 Skeleton으로 옮길 수 있을 거 같은데,,,,
	BoneDesc BoneData;
	ConstantBuffer * BoneDescBuffer;
	IECB_t * ECB_BoneDescBuffer;
#pragma endregion Bone

};
