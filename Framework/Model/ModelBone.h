#pragma once


class ModelBone
{
	using ThisClass = ModelBone;
	using ThisClassPtr = ThisClass*;
	friend class Model;
	friend class ModelAnimation;

	ModelBone();
	~ModelBone();
	bool IsRootBone() const {return ParentIndex < 0;}
	static void ReadModelFile( const BinaryReader * InReader, vector<ThisClassPtr> & OutBones);
	
	int Index = -1;
	string Name;
	
	int ParentIndex = -1;
	ThisClassPtr Parent = nullptr;

	Matrix Transform;
	vector<UINT> MeshIndices;
	
	vector<ThisClassPtr> Children;
};
