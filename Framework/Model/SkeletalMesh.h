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
	~SkeletalMesh() override;
	void Render(UINT InstanceCount) const override;
	void CreateBuffers() override;
	
	int GetBoneIndex() const;
	void SetBoneIndex( int InBoneIndex );

#pragma region Bone
private:
	struct BoneIndexDesc
	{
		UINT BaseBoneIndex;
		float Padding[3];
	};
	BoneIndexDesc BoneIndexData;
	ConstantBuffer * BoneIndexBuffer;
	string CBufferName = "CB_BoneIndex";
#pragma endregion Bone

};
