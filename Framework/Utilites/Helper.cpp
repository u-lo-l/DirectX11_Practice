#include "framework.h"
#include "Helper.h"

string Helper::ColorToJson( const Color & InColor )
{
	return String::Format("%0.6f,%0.6f,%0.6f,%0.6f", InColor.R, InColor.G, InColor.B, InColor.A);
}

Color Helper::JsonToColor(const string & InJsonString)
{
	vector<Json::String> v;
	String::SplitString(&v, InJsonString, ",");

	CHECK(v.size() == 4);

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}

Vector Helper::JsonToVector3(const string & InJsonString)
{
	vector<Json::String> v;
	String::SplitString(&v, InJsonString, ",");

	CHECK(v.size() == 3);
	
	return Vector(stof(v[0]), stof(v[1]), stof(v[2]));
}

Quaternion Helper::JsonToQuaternion(const string & InJsonString)
{
	vector<Json::String> v;
	String::SplitString(&v, InJsonString, ",");

	CHECK(v.size() == 4);
	
	return Quaternion(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}
