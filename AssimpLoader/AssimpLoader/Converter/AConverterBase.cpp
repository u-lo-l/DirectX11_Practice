#include "Pch.h"
#include "AConverterBase.h"

const unsigned int AConverterBase::ConvertFlag =
	aiProcess_ConvertToLeftHanded
	| aiProcess_Triangulate
	| aiProcess_GenNormals
	| aiProcess_CalcTangentSpace
	| aiProcess_TransformUVCoords
	| aiProcess_GenUVCoords
	| aiProcess_OptimizeMeshes;

AConverterBase::AConverterBase()
	: Importer(new Assimp::Importer()) 
{ }

AConverterBase::~AConverterBase()
{
	SAFE_DELETE(Importer);
}

void AConverterBase::ParseMetaData(const wstring & InFileName, const aiScene * InScene)
{
	const aiMetadata * const & Meta = InScene->mMetaData;
	const UINT Props = Meta->mNumProperties;
	for (UINT i = 0; i < Props; i++)
	{
		const aiMetadataEntry & Entry = Meta->mValues[i];
		const string & Key = Meta->mKeys[i].C_Str();
		switch (Entry.mType)
		{
		case AI_BOOL:  // NOLINT(bugprone-branch-clone)
			MetaData_Bool[Key] = *static_cast<bool*>(Entry.mData);
			break;
		case AI_INT32: case AI_UINT32: case AI_INT64: case AI_UINT64:
			MetaData_Int[Key] = *static_cast<int*>(Entry.mData);
			break;
		case AI_FLOAT: case AI_DOUBLE:
			MetaData_Float[Key] = *static_cast<float*>(Entry.mData);
			break;
		case AI_AIVECTOR3D:
			MetaData_Vector[Key] = *static_cast<aiVector3D*>(Entry.mData);
			break;
		case AI_AISTRING:
			MetaData_String[Key] = static_cast<aiString *>(Entry.mData)->C_Str();
			break;
		default:
			break;
		}
	}
	int Front = (MetaData_Int["FrontAxis"] + 1) * MetaData_Int["FrontAxisSign"];
	int Up = (MetaData_Int["UpAxis"] + 1) * MetaData_Int["UpAxisSign"];
	CreateConvertMatrix(Front, Up, InFileName);
}

/*
 * 
 */
void AConverterBase::CreateConvertMatrix(const int Forward, const int Up, const wstring & InFileName)
{
	int Abs = abs(Forward);
	float Sign = Forward > 0 ? 1.0f : -1.0f;
	Vector ForwardAxis = Vector(Abs == 1 ? 1.f : 0.f, Abs == 2 ? 1.f : 0.f, Abs == 3 ? 1.f : 0.f ) * Sign;
	ASSERT(ForwardAxis.Length() > 0, String::Format("%s | %ls  Front Vector Not Valid", __FUNCTION__, InFileName.c_str()).c_str())
	Abs = abs(Up);
	Sign = Up > 0 ? 1.0f : -1.0f;
	Vector UpAxis = Vector(Abs == 1 ? 1.f : 0.f, Abs == 2 ? 1.f : 0.f, Abs == 3 ? 1.f : 0.f ) * Sign;;
	ASSERT(UpAxis.Length() > 0, String::Format("%s | %ls  Up Vector Not Valid", __FUNCTION__, InFileName.c_str()).c_str())
	Vector RightAxis = Vector::Cross(UpAxis, ForwardAxis);

	ASSERT(RightAxis.Length() > 0, String::Format("%s | %ls Coordinate Rank Not Valid", __FUNCTION__, InFileName.c_str()).c_str())

	Matrix ModelBaseMatrix = Matrix(
		RightAxis.X, RightAxis.Y, RightAxis.Z, 0,
		UpAxis.X, UpAxis.Y, UpAxis.Z, 0,
		ForwardAxis.X, ForwardAxis.Y, ForwardAxis.Z, 0,
		0.f, 0.f, 0.f, 1.f
	);
	CoordinateConvertMatrix = Matrix::Identity;
}
