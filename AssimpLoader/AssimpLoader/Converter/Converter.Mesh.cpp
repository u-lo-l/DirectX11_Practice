#include "Pch.h"
#include "Converter.h"
#include <fstream>

namespace Sdt
{
	void Converter::ExportMesh( const wstring & InSaveFileName )
	{
		wstring FullFileName = W_MODEL_PATH + InSaveFileName + L".mesh";
		ReadBoneData(Scene->mRootNode, 0, -1);
		ReadMeshData();
		ReadSkinData();
		
		WriteMesh(FullFileName);
	}
	
	void Converter::ReadBoneData( const aiNode * InNode, int InIndex, int InParent )
	{
		BoneData * Bone = new BoneData();
		Bone->Index = InIndex;
		Bone->Parent = InParent;
		Bone->Name = InNode->mName.C_Str();
		
		Bone->Transform = InNode->mTransformation;
		// row-major(Assimp) to colum-major(DX)
		Bone->Transform.Transpose();

		if (InParent >= 0)
		{
			// local-coord to world-coord
			Bone->Transform = Bone->Transform * Bones[InParent]->Transform;
		}
		Bones.push_back(Bone);


		const UINT MeshCount = InNode->mNumMeshes;
		Bone->MeshIndices.reserve(MeshCount);
		for (UINT i	= 0 ; i < MeshCount ; i++)
		{
			Bone->MeshIndices.push_back(InNode->mMeshes[i]);
		}

		const UINT ChildBoneCount = InNode->mNumChildren;
		for (UINT i = 0; i < ChildBoneCount ; i++)
		{
			ReadBoneData(InNode->mChildren[i], Bones.size(), InIndex);
		}
	}

	void Converter::ReadMeshData()
	{
		const UINT MeshCount = Scene->mNumMeshes;
		Meshes.resize(MeshCount);
		for (UINT i = 0; i < MeshCount; i++)
		{
			Meshes[i] = new MeshData();
			const aiMesh * const AiMesh = Scene->mMeshes[i];

			// Read Material Data
			const UINT MatIndex = AiMesh->mMaterialIndex;
			Meshes[i]->MaterialName = Scene->mMaterials[MatIndex]->GetName().C_Str();

			// Read Vertices Data
			const UINT VerticesCount = AiMesh->mNumVertices;
			Meshes[i]->Vertices.reserve(VerticesCount);
			for (UINT v = 0; v < VerticesCount; v++)
			{
				Meshes[i]->Vertices.emplace_back(ThisClass::ReadSingleVertexDataFromAiMesh(AiMesh, v));
			}

			// Read Indices Data
			const UINT FacesCount = AiMesh->mNumFaces;
			for (UINT f = 0; f < FacesCount; f++)
			{
				const aiFace & Face = AiMesh->mFaces[f];
				const UINT IndicesCount = Face.mNumIndices;
				for (UINT k = 0; k < IndicesCount; k++)
				{
					Meshes[i]->Indices.push_back(Face.mIndices[k]);
				}
			}
		}
	}

	MeshData::VertexType Converter::ReadSingleVertexDataFromAiMesh( const aiMesh * Mesh, UINT VertexIndex )
	{
		MeshData::VertexType Vertex;
		memcpy_s(&Vertex.Position, sizeof(Vector), Mesh->mVertices + VertexIndex, sizeof(Vector));

		if (Mesh->HasTextureCoords(0) == true)
		{
			memcpy_s(&Vertex.UV, sizeof(Vector2D), Mesh->mTextureCoords[0] + VertexIndex, sizeof(Vector2D));
		}
		if (Mesh->HasVertexColors(0) == true)
		{
			memcpy_s(&Vertex.Color, sizeof(Color), Mesh->mColors[0] + VertexIndex, sizeof(Color));
		}
		if (Mesh->HasNormals() == true)
		{
			memcpy_s(&Vertex.Normal, sizeof(Vector), Mesh->mNormals + VertexIndex, sizeof(Vector));
		}
		if (Mesh->HasTangentsAndBitangents() == true)
		{
			memcpy_s(&Vertex.Tangent, sizeof(Vector), Mesh->mTangents + VertexIndex, sizeof(Vector));
		}
		return Vertex;
	}

	void Converter::WriteMesh( const wstring & InSaveFileName ) const
	{
		Path::CreateFolders(Path::GetDirectoryName(InSaveFileName));
		BinaryWriter * BinWriter = new BinaryWriter();
		BinWriter->Open(InSaveFileName);

		BinWriter->WriteUint(Bones.size());
		for (const BoneData * BoneData : Bones)
		{
			BinWriter->WriteUint(BoneData->Index);
			BinWriter->WriteString(BoneData->Name);
			BinWriter->WriteInt(BoneData->Parent);
			BinWriter->WriteMatrix(BoneData->Transform);

			const UINT MeshIndexCount = BoneData->MeshIndices.size();
			BinWriter->WriteUint(MeshIndexCount);
			if (MeshIndexCount > 0)
				BinWriter->WriteByte(&(BoneData->MeshIndices[0]), MeshIndexCount * sizeof(UINT));
			SAFE_DELETE(BoneData);
		}

		BinWriter->WriteUint(Meshes.size());
		for (const MeshData * MeshData : Meshes)
		{
			BinWriter->WriteString(MeshData->Name);
			BinWriter->WriteString(MeshData->MaterialName);

			const UINT VerticesCount = MeshData->Vertices.size();
			BinWriter->WriteUint(VerticesCount);
			if (VerticesCount > 0)
				BinWriter->WriteByte(&(MeshData->Vertices[0]), VerticesCount * sizeof(MeshData::VertexType));

			const UINT IndicesCount = MeshData->Indices.size();
			BinWriter->WriteUint(IndicesCount);
			if (IndicesCount > 0)
				BinWriter->WriteByte(&(MeshData->Indices[0]), IndicesCount * sizeof(UINT));	
			SAFE_DELETE(MeshData);
		}
		
		BinWriter->Close();
		SAFE_DELETE(BinWriter);
	}

	void Converter::ReadSkinData()
	{
		const UINT MeshCount = Scene->mNumMeshes;
		for (UINT MeshIndex = 0; MeshIndex < MeshCount; MeshIndex++)
		{
			const aiMesh * const TargetMesh = Scene->mMeshes[MeshIndex];
			// Bone이아니다 -> 스키닝이 될 아이가 아니다.
			if (TargetMesh->HasBones() == false) 
			{
				continue;
			}

			const UINT BoneCount = TargetMesh->mNumBones;
			for (UINT boneIndex = 0; boneIndex < BoneCount; boneIndex++)
			{
				aiBone * Bone = TargetMesh->mBones[boneIndex];
				const char * boneName = Bone->mName.C_Str();

				UINT TargetBoneIndex = 0;
				// Bones : fbx 모델의 모든 Bone정보.
				for (UINT j = 0; j < Bones.size(); j++)
				{
					if (Bones[j]->Name == boneName)
					{
						TargetBoneIndex = j;
						break;
					}
				}

				for (UINT w = 0; w < Bone->mNumWeights; w++)
				{
					const UINT VertexId = Bone->mWeights[w].mVertexId; // ex) VertexId : 20978
					const float Weight = Bone->mWeights[w].mWeight; // ex) 20978번 Vertex에 이 Bone이 미치는 영향력이 0.025f다.

					MeshData * const meshdata =  this->Meshes[MeshIndex];
					Vector4 & Indices = meshdata->Vertices[VertexId].Indices;
					Vector4 & Weights = meshdata->Vertices[VertexId].Weights;

					// 최대 영향을 받을거 4개다.
					for (UINT v = 0; v < 4; v++)
					{
						if (Indices.V[v] <= 0)
						{
							Indices.V[v] = static_cast<float>(TargetBoneIndex);
							Weights.V[v] = Weight;
							break;
						}
					}// for(V)
				}//for(w)
			} //for(boneIndex)
		}//for(MeshIndex)

		// for (UINT MeshIndex = 0; MeshIndex < MeshCount; MeshIndex++)
		// {
		// 	const aiMesh * const TargetMesh = Scene->mMeshes[MeshIndex];
		// 	const UINT BoneCount = TargetMesh->mNumBones;
		// 	for (UINT BoneIndex = 0; BoneIndex <BoneCount; BoneIndex++)
		// 	{
		// 		aiBone * Bone = TargetMesh->mBones[BoneIndex];
		// 		for (UINT w = 0; w < Bone->mNumWeights; w++)
		// 		{
		// 			MeshData * const meshdata =  this->Meshes[MeshIndex];
		// 			Vector4 & Weights = meshdata->Vertices[VertexId].Weights;
		// 		}
		// 	}
		// }
	}
}