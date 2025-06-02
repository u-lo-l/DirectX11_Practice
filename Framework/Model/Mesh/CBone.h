#pragma once

class CBone
{
public:
	struct BoneDesc
	{
		int Index = -1;
		int ParentIndex = -1;
		string BoneName;
		Matrix RootTransform;
		Matrix OffsetMatrix;
	};
	explicit CBone(const BoneDesc & Desc);
	~CBone();
	bool IsRootBone() const;

	void SetBoneIndex(int Index);
	void SetName(const string & InName);
	void SetParentIndex(int Index);
	void SetParentBone(CBone* InParent, int InBoneIndex);
	void SetRootTransform(const Matrix & InMatrix);
	void SetOffsetMatrix(const Matrix & InMatrix);
	void SetAttachedMeshIndices(const vector<UINT> & InAttachedMeshIndices);
	void AddChildBone(CBone* InChild);

	int GetBoneIndex() const;
	const string& GetName() const;
	int GetParentIndex() const;
	const CBone* GetParentBone() const;
	CBone* GetParentBone();
	const Matrix & GetRootTransform() const;
	const Matrix & GetRootOffsetMatrix() const;
	const vector<CBone*> & GetChildrenBones() const;
	const vector<UINT>& GetAttachedMeshIndices() const;
	
private:
	int Index = 0;
	int ParentIndex = -1;
	string BoneName;

	struct MatrixDesc
	{
		Matrix RootTransform;
		Matrix OffsetMatrix;
	} MatrixData;
	ConstantBuffer * CB_BoneMatrix = nullptr;

	vector<UINT> AttachedMeshIndices;

	CBone * Parent = nullptr;
	vector<CBone *> Children;
};

