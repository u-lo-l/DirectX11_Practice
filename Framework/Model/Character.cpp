#include "framework.h"
#include "Character.h"

#include "Animation/AnimationController.h"

Character::Character()
{
	SkeletalMesh = nullptr;
	AnimController = nullptr;
	Tf = new Transform();
}

Character::~Character()
{
	SAFE_DELETE(SkeletalMesh);
	SAFE_DELETE(AnimController);
}

void Character::SetSkeletalMesh(const wstring& InMeshName)
{
	SAFE_DELETE(SkeletalMesh);
	SAFE_DELETE(AnimController);
	this->SkeletalMesh = new CSkeletalMesh(InMeshName);
	this->SkeletalMesh->GetTransform()->SetParent(this->Tf);
	this->AnimController = new AnimationController(this->SkeletalMesh->GetSkeletal());
}

const CSkeletal * Character::GetSkeleton() const
{
	return this->SkeletalMesh->GetSkeletal();
}

void Character::AddAnimationClip(const string& InName, AnimationClip* InAnimation)
{
	ASSERT(!!this->SkeletalMesh, "Character has no SkeletalMesh")
	
	Animations[InName] = InAnimation;
	AnimController->SetCurrentAnimation(InAnimation);
}

AnimationController* Character::GetAnimationController() const
{
	return AnimController;
}

void Character::Tick()
{
	this->AnimController->Tick();
	// this->SkeletalMesh->Tick();
}

void Character::Render()
{
	// this->SkeletalMesh->Render();
}
