#pragma once
#include "Systems/IExecutable.h"

class Character;
class CSkeletalMesh;

class DrawCSkeletalMesh : public IExecutable
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Tick() override;
	virtual void Render() override;

	Character * Adam; 
};
