#pragma once

class ModelAnimation;
class ModelBone;
struct MeshData;

class ModelMesh
{
public:
	using ThisClass = ModelMesh;
	using ThisClassPtr = ThisClass*;
	friend class Model;
protected:
	using VertexType = ModelVertex;
	
	static void ReadMeshFile(
		const BinaryReader * InReader,
		vector<ThisClassPtr> & OutMeshes,
		const map<string, Material*> & InMaterialTable,
		bool bIsSkeletal = false
	);

#ifdef DO_DEBUG
	explicit ModelMesh(const string & MetaData = "ModelMesh");
	string MetaData;
#else
	ModelMesh();
#endif
	virtual ~ModelMesh() = 0;
	void Tick();
	virtual void Render(UINT InstanceCount) const;
	virtual void CreateBuffers();
	
	void SetWorldTransform( const Transform * InTransform ) const;
	
protected :
	string MeshName;

	Transform * ref_ModelWorldTransform = nullptr;

public:
	void SetMaterialData(Material * InMaterial);
protected:
	Material * MaterialData = nullptr;

	vector<VertexType> Vertices;
	
	vector<UINT> Indices;

	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;

	///
	struct W_Desc
	{
		Matrix World;
	} W_Data;
	ConstantBuffer * WBuffer = nullptr;
	///
};
