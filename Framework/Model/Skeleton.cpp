#include "framework.h"
#include "Skeleton.h"

Skeleton::Skeleton()
	: BoneDescBuffer(nullptr)
{
}

Skeleton::~Skeleton()
{
	for (const ModelBone * Bone : Bones)
		SAFE_DELETE(Bone);
}

void Skeleton::SetUpBoneTable(const vector<ModelMesh *> & Meshes)
{
	this->CachedBoneTable = new CachedBoneTableType(); // 애니메이션 다 읽으면 지워진다.
	for (int i = 0; i < GetBoneCount(); i++)
	{
		ModelBone * TargetBone = this->Bones[i];

		/*
		 * 기본적으로 mVertex들은 fbx모델의 Root를 기준으로 한다.
		 * 하지만 mMesh가 어떤 aiNode의 Transform을 참조하는경우 해당 Transform(이하 aiTF)을 기준으로한다.
		 * 여기서 문제는 하나의 mMesh가 여러 aiNode로부터 참조될 수 있기에 그냥 초기에 변환해버리면 안된다는 것이다.
		 * 최종적인 Root기준의 mVertex는 
		 */
		this->BoneData.OffsetMatrix[i] = Matrix::Invert(TargetBone->Transform);
		this->BoneData.BoneTransform[i] = TargetBone->Transform;
		for (const UINT number : TargetBone->MeshIndices)
		{
			SkeletalMesh * SkMesh = dynamic_cast<SkeletalMesh*>(Meshes[number]);
			if (SkMesh == nullptr)
				continue;
			SkMesh->SetBoneIndex(TargetBone->Index);
		}
		(*CachedBoneTable)[TargetBone->Name] = TargetBone;
	}
	// 이것 이후에 CreateBuffer 해야함.
}

void Skeleton::ClearBoneTable()
{
	SAFE_DELETE(CachedBoneTable);
}

void Skeleton::CreateBuffer( const map<string, Material *> & MaterialsInModel)
{
	const string CBufferInfo = "Bone Transform Data";
	BoneDescBuffer = new ConstantBuffer(&BoneData, CBufferInfo, sizeof(BoneDesc));
	for (const auto & Pair : MaterialsInModel)
	{
		ECB_BoneDescBuffers.emplace_back(Pair.second->GetShader()->AsConstantBuffer(CBufferName));
	}
}

// TODO : Render마다 말고 최초 한 번만 Bind해도 되지 않나?
void Skeleton::BindToGPU() const
{
	BoneDescBuffer->BindToGPU();
	for (IECB_t * CBufferHandle : ECB_BoneDescBuffers)
	{
		CHECK(CBufferHandle->SetConstantBuffer(*BoneDescBuffer) >= 0);
	}
}
