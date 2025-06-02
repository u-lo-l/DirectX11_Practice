#include "framework.h"
#include "CBone.h"

CBone::CBone(const BoneDesc& Desc)
{
	Index = Desc.Index;
	BoneName = Desc.BoneName;
	ParentIndex = Desc.ParentIndex;
	MatrixData = { Desc.RootTransform, Desc.OffsetMatrix };
	// CB_BoneMatrix = new ConstantBuffer(
	// 	(UINT)ShaderType::VertexShader,
	// 	0, // TODO
	// 	&MatrixData,
	// 	"Bone Matrix Data",
	// 	sizeof(MatrixDesc),
	// 	true
	// );
}

CBone::~CBone()
{
	SAFE_DELETE(CB_BoneMatrix);
}

bool CBone::IsRootBone() const
{
	return ParentIndex < 0;
}

void CBone::SetBoneIndex(int Index)
{
	this->Index = Index;
}

void CBone::SetName(const string& InName)
{
	this->BoneName = InName;
}

void CBone::SetParentIndex(int Index)
{
	this->ParentIndex = Index;
}

void CBone::SetParentBone(CBone* InParent, int InBoneIndex)
{
	Parent = InParent;
	ParentIndex = InBoneIndex;
}

void CBone::SetRootTransform(const Matrix& InMatrix)
{
	MatrixData.RootTransform = InMatrix;
}

void CBone::SetOffsetMatrix(const Matrix& InMatrix)
{
	MatrixData.OffsetMatrix = InMatrix;
}

void CBone::AddChildBone(CBone* InChild)
{
	Children.push_back(InChild);
}

const CBone* CBone::GetParentBone() const
{
	return Parent;
}

CBone* CBone::GetParentBone()
{
	return Parent;
}

const vector<CBone*>& CBone::GetChildrenBones() const
{
	return Children;
}

const vector<UINT>& CBone::GetAttachedMeshIndices() const
{
	return AttachedMeshIndices;
}

int CBone::GetBoneIndex() const
{
	return Index;
}

int CBone::GetParentIndex() const
{
	return ParentIndex;
}

const Matrix& CBone::GetRootTransform() const
{
	return MatrixData.RootTransform;
}

const Matrix& CBone::GetRootOffsetMatrix() const
{
	return MatrixData.OffsetMatrix;
}

const string & CBone::GetName() const
{
	return BoneName;
}

void CBone::SetAttachedMeshIndices(const vector<UINT> & InAttachedMeshIndices)
{
	AttachedMeshIndices = InAttachedMeshIndices;
}
