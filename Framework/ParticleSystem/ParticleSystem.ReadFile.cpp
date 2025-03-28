#include "framework.h"
#include "ParticleSystem.h"
#include <fstream>

void ParticleSystem::ReadFile(const wstring & InFileName)
{
	ifstream stream;
	stream.open(InFileName);

	Json::Value root;
	stream >> root;

	Json::Value::Members members = root.getMemberNames();
	for (UINT i = 0; i < members.size(); i++)
	{
		const Json::Value& Value = root[members[i]];
		switch (i)
		{
			case 0: ReadColor(Value); break;
			case 1: ReadSettings(Value); break;
			case 2: ReadSize(Value); break;
			case 3: ReadSpeed(Value); break;
			case 4: ReadVelocity(Value); break;
		}
	}
}

void ParticleSystem::ReadColor(const Json::Value& Value)
{
	Data.ColorAmount = stof(Value["ColorAmount"].asString());

	vector<string> color;

	String::SplitString(&color, Value["MaxColor"].asString(), ",");
	Data.MaxColor = Color(stof(color[0]), stof(color[1]), stof(color[2]), stof(color[3]));

	String::SplitString(&color, Value["MinColor"].asString(), ",");
	Data.MinColor = Color(stof(color[0]), stof(color[1]), stof(color[2]), stof(color[3]));
}

void ParticleSystem::ReadSettings(const Json::Value& Value)
{
	Data.BlendType = static_cast<EBlendType>(stoi(Value["BlendType"].asString()));
	Data.MaxParticle = (UINT)stoi(Value["MaxParticle"].asString());
	Data.DurationModifier = stof(Value["ReadyRandomTime"].asString());
	Data.Duration = stof(Value["ReadyTime"].asString());
	Data.bLoop = static_cast<bool>(stoi(Value["bLoop"].asString()));

	vector<string> v;
	String::SplitString(&v, Value["Gravity"].asString(), ",");
	Data.Gravity = Vector(stof(v[0]), stof(v[1]), stof(v[2]));

	Data.TextureFileName = String::ToWString(Value["TextureFile"].asString());
}

void ParticleSystem::ReadSize(const Json::Value& Value)
{
	Data.MaxEndSize = stof(Value["MaxEndSize"].asString());
	Data.MaxStartSize = stof(Value["MaxStartSize"].asString());
	Data.MinEndSize = stof(Value["MinEndSize"].asString());
	Data.MinStartSize = stof(Value["MinStartSize"].asString());
}

void ParticleSystem::ReadSpeed(const Json::Value& Value)
{
	Data.MaxRotateSpeed = stof(Value["MaxRotateSpeed"].asString());
	Data.MinRotateSpeed = stof(Value["MinRotateSpeed"].asString());
}

void ParticleSystem::ReadVelocity(const Json::Value& Value)
{
	Data.EndVelocity = stof(Value["EndVelocity"].asString());
	Data.MaxHorizontalVelocity = stof(Value["MaxHorizontalVelocity"].asString());
	Data.MaxVerticalVelocity = stof(Value["MaxVerticalVelocity"].asString());
	Data.MinHorizontalVelocity = stof(Value["MinHorizontalVelocity"].asString());
	Data.MinVerticalVelocity = stof(Value["MinVerticalVelocity"].asString());
	Data.StartVelocity = stof(Value["StartVelocity"].asString());
}

void ParticleSystem::CreateSingleParticleVertex
(
	const ParticleData & InData,
	const Vector & InPosition,
	float InElapsedTime,
	VertexType & OutParticleVertex
)
{

	const float HorizontalAngle = Math::Pi * 2.0f * Math::Random(0.0f, 1.0f);
	Vector4 random;
	random.X = Math::Random(0.0f, 1.0f);
	random.Y = Math::Random(0.0f, 1.0f);
	random.Z = Math::Random(0.0f, 1.0f);
	random.W = Math::Random(0.0f, 1.0f);


	OutParticleVertex.Position = InPosition; // SpawnedPosition

	OutParticleVertex.StartVelocity.Y = InData.MinVerticalVelocity;
	OutParticleVertex.StartVelocity.X = InData.MinHorizontalVelocity * cosf(HorizontalAngle);
	OutParticleVertex.StartVelocity.Z = InData.MinHorizontalVelocity * sinf(HorizontalAngle);

	OutParticleVertex.EndVelocity.Y = InData.MaxVerticalVelocity;
	OutParticleVertex.EndVelocity.X = InData.MaxHorizontalVelocity * cosf(HorizontalAngle);
	OutParticleVertex.EndVelocity.Z = InData.MaxHorizontalVelocity * sinf(HorizontalAngle);
	
	OutParticleVertex.Random = random;
	OutParticleVertex.SpawnedTime = InElapsedTime;
}
