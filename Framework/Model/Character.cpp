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

const CSkeletal * Character::GetSkeleton() const
{
	return this->SkeletalMesh->GetSkeletal();
}

void Character::AddAnimationClip(const string& InName, AnimationClip* InAnimation)
{
	ASSERT(!!this->SkeletalMesh, "Character has no SkeletalMesh")
	
	if (AnimController == nullptr)
		AnimController = new AnimationController(this->SkeletalMesh->GetSkeletal());
	Animations[InName] = InAnimation;
	AnimController->SetCurrentAnimation(InAnimation);
}

void Character::Tick()
{
	this->AnimController->Tick();
	this->SkeletalMesh->Tick();
}

void Character::Render()
{
	this->SkeletalMesh->Render();
}
