#pragma once


class ModelBone
{
	using ThisClass = ModelBone;
	using ThisClassPtr = ThisClass*;
	friend class Model;

	ModelBone();
	~ModelBone();

	static void ReadFile( const BinaryReader * InReader, vector<ThisClassPtr> & OutBones);
	
	int Index = -1;
	string Name = "";
	int ParentIndex = -1;
	ThisClassPtr Parent = nullptr;
	Matrix Transform;
	vector<UINT> MeshIndices;
	
	vector<ThisClassPtr> Children;
};
