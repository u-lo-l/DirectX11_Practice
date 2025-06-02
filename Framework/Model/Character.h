#pragma once

class AnimationClip;
class AnimationController;
class CSkeletalMesh;

class Character
{
public:
	Character();
	~Character();
	
	void SetSkeletalMesh(const wstring & InMeshName);
	void AddAnimationClip(const string & InName, AnimationClip * InAnimation);
	void Tick();
	void Render();
private:
	Transform * Tf = nullptr;
	// TODO : Collider * BoundingCapsule = nullptr;
	CSkeletalMesh * SkeletalMesh = nullptr;
	AnimationController * AnimController = nullptr;
	map<string, AnimationClip *> Animations;
	int CurrentAnimationID = 0;
	float AnimationPlayRate = 1.0f;
};
