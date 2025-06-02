#pragma once

class Skeleton;

class ModelBone
{
public:
	static void ReadModelFile( const BinaryReader * InReader, Skeleton* &OutSkeleton);
private:
	using ThisClass = ModelBone;
	using ThisClassPtr = ThisClass*;
	friend class Model;
	friend class ModelAnimation;
	friend class Skeleton;
	friend class CSkeletalMesh;

	ModelBone();
	~ModelBone();
	bool IsRootBone() const {return ParentIndex < 0;}
	
	int Index = -1;
	string Name;
	
	int ParentIndex = -1;
	ThisClassPtr Parent = nullptr;

	Matrix Transform;
	vector<UINT> MeshIndices;
	
	vector<ThisClassPtr> Children;
};
