#pragma once

#include "Systems/IExecutable.h"

class Main final : public IExecutable
{
public:
	void Initialize() override;
	void Destroy() override;

	void Tick() override;
	void Render() override;

private:
	void Push(IExecutable* Executable);

private:
	vector<IExecutable*> Executables;
};