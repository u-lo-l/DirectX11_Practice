#include "framework.h"
#include "Character.h"

#include "Animation/AnimationController.h"

Character::Character()
{
	SkeletalMesh = nullptr;
	AnimController = nullptr;
}

Character::~Character()
{
	SAFE_DELETE(SkeletalMesh);
	SAFE_DELETE(AnimController);
}

void Character::SetSkeletalMesh(const wstring& InMeshName)
{
	this->SkeletalMesh = new CSkeletalMesh(InMeshName);
}

void Character::AddAnimationClip(const string& InName, AnimationClip* InAnimation)
{
	if (AnimController == nullptr)
		AnimController = new AnimationController();
	Animations[InName] = InAnimation;
}

void Character::Tick()
{
	this->SkeletalMesh->Tick();
}

void Character::Render()
{
	this->SkeletalMesh->Render();
}
