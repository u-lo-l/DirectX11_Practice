#pragma once
#include "Systems/IExecutable.h"

class Character;
class CSkeletalMesh;

class DrawCSkeletalMesh : public IExecutable
{
public:
	void Initialize() override;
	void Destroy() override;
	void Tick() override;
	void Render() override;

	Character * Adam; 
};
