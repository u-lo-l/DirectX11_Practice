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
	Shader * CachedShader = nullptr;
	int Pass = 0;

	string MeshName;

	Transform * ref_ModelWorldTransform = nullptr;

public:
	void SetMaterialData(Material * InMaterial);
protected:
	Material * MaterialData = nullptr;

	UINT VerticesCount = 0;
	VertexType * Vertices = nullptr;
	
	UINT IndicesCount = 0;
	UINT* Indices = nullptr;

	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;
	
	// View-Projection Matrix정보임. 굳이 왜 여기에 또... 이거도 Shader마다인데...
	// 이게 Global에 생성해놓고 가져다 써야겠다.
	ConstantDataBinder * GlobalMatrixCBBinder = nullptr;
};
