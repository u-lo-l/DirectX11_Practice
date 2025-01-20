#pragma once
#include "Pch.h"
#include "Model/ModelAnimation.h"


struct MaterialData
{
	string Name;
	string ShaderName;

	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	vector<string> DiffuseFiles;
	vector<string> SpecularFiles;
	vector<string> NormalFiles;

	void SetTextureFilesFromAiMaterialByTextureType(const aiMaterial * Material, aiTextureType InTextureType);
};

struct BoneData
{
	UINT Index;
	string Name;
	int Parent;
	Matrix Transform;

	vector<UINT> MeshIndices;
};

struct MeshData
{
	using VertexType = ModelVertex;
	
	string Name;

	string MaterialName;

	vector<VertexType> Vertices;
	vector<UINT> Indices;
};

/**
 *  @breif
 *  KeyFrameAnimation 내에서 특정 Bone에 대한 KeyFrameData를 저장하는 구조체.
 *  @property 
 *  BoneName  : 어떤 Bone에 적용되는 Animation인지
 *  PosKeys   : 위치 정보
 *  ScaleKeys : 크기 정보
 *  RotKeys   : 회전 정보
 *  @details
 *  애니메이션 은 모든 Frame에 대해 데이터를 저장하지 않는다.<br/>
 *  그래서 ClipData의 Duration보다 Pos, Scale, Rot데이터의 vector의 크기가 작을 수 있다.<br/>
 *  대신 중간중간 빈 부분은 보간해주어 애니메이션을 표현한다.
 */
struct ClipNodeData
{
	string BoneName;
	vector<FrameDataVec> PosKeys;
	vector<FrameDataVec> ScaleKeys;
	vector<FrameDataQuat> RotKeys;
};

/**
 *  하나의 KeyFrameAnimation Clip에 대한 정보.
 *  Name : Animation 이름
 *  Duration : 전체 Frame수 - 1 : 마지막 프레임의 번호가 저장된다.
 *  TicksPerSecond : 애니메이션의 FPS
 *  NodeDatas : 
 */
struct ClipData
{
	string Name;
	float Duration;
	float TicksPerSecond;
	vector<ClipNodeData *> NodeDatas;
};
