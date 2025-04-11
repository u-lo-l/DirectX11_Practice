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
		const map<string, Material<VertexType>*> & InMaterialTable,
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
	virtual void RenderShadow(UINT InstanceCount) const;
	virtual void Render(UINT InstanceCount) const;
	virtual void CreateBuffers();
	
	void SetWorldTransform( const Transform * InTransform ) const;
	
protected :
	string MeshName;

	Transform * ref_ModelWorldTransform = nullptr;

public:
	void SetMaterialData(Material<VertexType> * InMaterial);
protected:
	Material<VertexType> * MaterialData = nullptr;

	vector<VertexType> Vertices;
	
	vector<UINT> Indices;

	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;
	ConstantBuffer * ViewInv_CBuffer_PS = nullptr;

	struct ViewInvDesc
	{
		Matrix ViewInv;
		Vector2D Tiling = {1,1};
		Vector2D Padding;
	};
	ViewInvDesc PS_ViewInv;
	Transform * WorldTF = nullptr;
};
