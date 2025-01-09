#include "framework.h"
#include "Model.h"

Model::Model()
 : RootBone(nullptr)
{
	
}

Model::~Model()
{
	for (ModelBone * bone : Bones)
		SAFE_DELETE(bone);
	for (ModelMesh * mesh : Meshes)
		SAFE_DELETE(mesh);
}

void Model::Tick() const
{
	for (ModelMesh * mesh : Meshes)
		mesh->Tick();
}

void Model::Render() const
{
	for (ModelMesh * mesh : Meshes)
		mesh->Render();
}

void Model::ReadMesh( const wstring & InFileName )
{
	wstring FullPath = W_MODEL_PATH + InFileName + L".mesh";

	BinaryReader * BinReader = new BinaryReader();
	BinReader->Open(FullPath);

	ModelBone::ReadFile(BinReader, this->Bones);
	ModelMesh::ReadFile(BinReader, this->Meshes);
	
	BinReader->Close();
	SAFE_DELETE(BinReader);
}

void Model::BindData(Shader * InShader)
{
	for (ModelMesh * Mesh : this->Meshes)
	{
		Mesh->BindData(InShader);
	}
}
