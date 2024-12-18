#pragma once

#include "Systems/IExecutable.h"

class Line : public IExecutable
{
public:
	void Initialize();
	void Destroy();

	void Tick();
	void Render();
};