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

const float AConverterBase::PRE_Y_ROTATION = Math::PI;

AConverterBase::AConverterBase()
	: Importer(new Assimp::Importer()) 
{ }

AConverterBase::~AConverterBase()
{
	SAFE_DELETE(Importer);
}
