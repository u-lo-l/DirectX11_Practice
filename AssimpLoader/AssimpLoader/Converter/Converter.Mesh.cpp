﻿#include "Pch.h"
#include "Converter.h"
#include <fstream>
#ifdef DO_DEBUG
static void CheckWeight(UINT MeshCount, const vector<MeshData *> & Meshes)
{
	UINT TotalWeightCount = 0;
	UINT CompleteWeightCount = 0;
	UINT NotCompleteWeightCount = 0;
	UINT ZeroWeightCount = 0;
	UINT ZeroIndex = 0;
	UINT TotalMeshIndexCount = 0;

	for (UINT MeshIndex = 0; MeshIndex < MeshCount; MeshIndex++)
	{
		const MeshData * const TargetMesh = Meshes[MeshIndex];
		const UINT VCount = TargetMesh->Vertices.size();
		for (UINT v  = 0 ; v < VCount ; v++)
		{
			float sum = 0;
			for (int i = 0 ; i < 4 ; i++)
			{
				sum += TargetMesh->Vertices[v].Weights[i];
			}
			if (sum < 0.999f)
			{
				NotCompleteWeightCount++;
				// printf("MeshIndex : %d, VertexIndex : %d, weight sum : %f [%f, %f, %f, %f]\n", MeshIndex, v, sum, TargetMesh->Vertices[v].Weights[0], TargetMesh->Vertices[v].Weights[1], TargetMesh->Vertices[v].Weights[2], TargetMesh->Vertices[v].Weights[3]);
			}
			else if (sum < 0.0001f)
			{
				ZeroWeightCount++;
			}
			else
			{
				CompleteWeightCount++;
			}
			TotalWeightCount++;
			for (int i = 0 ; i < 4 ; i++)
			{
				if (TargetMesh->Vertices[v].Indices[i] < 0)
				{
					if (i == 0)
						ZeroIndex++;
					break;
				}
			}
			TotalMeshIndexCount++;
		}
	}
	printf("=== === AnimNode Weight Info === ===\n");
	printf("=== TotalWeightCount : %d\n", TotalWeightCount);
	printf("=== IncompleteWeightCount : %d\n", NotCompleteWeightCount);
	printf("=== CompleteWeightCount : %d\n", CompleteWeightCount);
	printf("=== ZeroWeightCount : %d\n", ZeroWeightCount);
	printf("=== ZeroIndex: %d / %d\n", ZeroIndex, TotalMeshIndexCount);
	printf("=== === === === === === === === === === \n");
}
#endif
namespace sdt
{
	void Converter::ExportMesh( const wstring & InSaveFileName )
	{
		wstring FullFileName = W_MODEL_PATH + InSaveFileName + L".mesh";
		ReadBoneData(Scene->mRootNode, 0, -1);
		ReadMeshData();
		ReadSkinData();
		
		WriteAndClearBonesAndMeshes(FullFileName);
	}

	// 모든 Bone의 Root기준 Transform을 계산한다. 재귀적으로.
	void Converter::ReadBoneData( const aiNode * InNode, int InIndex, int InParent )
	{
		BoneData * Bone = new BoneData();
		Bone->Index = InIndex;
		Bone->Parent = InParent;
		Bone->Name = InNode->mName.C_Str();
		Bone->Transform = InNode->mTransformation;
		Bone->Transform.Transpose(); // row-major(Assimp) to colum-major(DX)
		if (Bone->IsRootBone() == false)
		{
			Bone->Transform = Bone->Transform * Bones[Bone->Parent]->Transform;
		}
		Bones.push_back(Bone);

		const UINT MeshCount = InNode->mNumMeshes;
		Bone->MeshIndices.reserve(MeshCount);
		for (UINT i	= 0 ; i < MeshCount ; i++)
		{
			// 이 Bone이 몇 번 째 Mesh에 영향을 주는가?
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
		this->Meshes.resize(MeshCount);
		for (UINT i = 0; i < MeshCount; i++)
		{
			Meshes[i] = new MeshData();
			const aiMesh * const AiMesh = Scene->mMeshes[i];
			const aiNode * MeshNode = Scene->mRootNode->FindNode(AiMesh->mName);
			aiMatrix4x4 MeshTransform = aiMatrix4x4();

			// MeshNode의 ParentNode가 Bone임을 가정. 아니면 UB
			// 실제론 ParentNode가 Bone임을 보장하지 않음.
			if (MeshNode != nullptr)
				MeshTransform = MeshNode->mTransformation;
			
			// Read Material Data
			const UINT MatIndex = AiMesh->mMaterialIndex;
			Meshes[i]->MaterialName = Scene->mMaterials[MatIndex]->GetName().C_Str();

			// Read Vertices Data
			const UINT VerticesCount = AiMesh->mNumVertices;
			Meshes[i]->Vertices.reserve(VerticesCount);
			for (UINT v = 0; v < VerticesCount; v++)
			{
				Meshes[i]->Vertices.emplace_back(ThisClass::ReadSingleVertexDataFromAiMesh(AiMesh, v, MeshTransform));
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

	MeshData::VertexType Converter::ReadSingleVertexDataFromAiMesh( const aiMesh * Mesh, UINT VertexIndex, const aiMatrix4x4 & InMeshTransform )
	{
		MeshData::VertexType Vertex;
		aiVector3D transformedVertex = InMeshTransform * Mesh->mVertices[VertexIndex];
		memcpy_s(&Vertex.Position, sizeof(Vector), &transformedVertex, sizeof(Vector));

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
		else
		{
			Vertex.Tangent = {1,0,0};
		}
		return Vertex;
	}

	void Converter::WriteAndClearBonesAndMeshes( const wstring & InSaveFileName )
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
			BinWriter->WriteSTDVector<UINT>(BoneData->MeshIndices);
			SAFE_DELETE(BoneData);
		}
		Bones.clear();
		
		BinWriter->WriteUint(Meshes.size());
		for (const MeshData * MeshData : Meshes)
		{
			BinWriter->WriteString(MeshData->Name);
			BinWriter->WriteString(MeshData->MaterialName);
			BinWriter->WriteSTDVector<MeshData::VertexType>(MeshData->Vertices);
			BinWriter->WriteSTDVector<UINT>(MeshData->Indices);

			SAFE_DELETE(MeshData);
		}
		Meshes.clear();
		
		BinWriter->Close();
		SAFE_DELETE(BinWriter);
	}

	/**
	 *  Mesh는 여러 Node들에 의해서 참조될 수 있다.
	 *  Skinning은 Bone에 의존하기 때문에, aiMesh가 가진 mBones만큼 데이터를 처리한다.
	 */
	void Converter::ReadSkinData()
	{
		map<string, BoneData*> TempBoneTable;
		for (BoneData * const Bone : this->Bones)
		{
			TempBoneTable.insert(make_pair(Bone->Name, Bone));
		}
		
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
				const aiBone * Bone = TargetMesh->mBones[boneIndex];
				const char * BoneName = Bone->mName.C_Str();
				const auto It = TempBoneTable.find(BoneName);
				if (It == TempBoneTable.cend())
					break; // Bone NotFound
				const UINT TargetBoneIndex = It->second->Index;

				// aiBone은 어떤 Vertex에 대해 얼마나 weight를 줄 지 가지고 있다.
				// 이 데이터를 MeshData를 저장한다.
				for (UINT w = 0; w < Bone->mNumWeights; w++)
				{
					const UINT VertexId = Bone->mWeights[w].mVertexId; // ex) VertexId : 20978
					const float Weight = Bone->mWeights[w].mWeight; // ex) 20978번 Vertex에 이 Bone이 미치는 영향력이 0.025f다.
					constexpr float WeightThreshold = 0.015f; 
					if (Weight < WeightThreshold)
						continue;
					MeshData::VertexType & TargetVertex = this->Meshes[MeshIndex]->Vertices[VertexId];
					Vector4 & Indices = TargetVertex.Indices;
					Vector4 & Weights = TargetVertex.Weights;

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

#ifdef DO_DEBUG
		CheckWeight(MeshCount, this->Meshes);
#endif
		
	}
}