#pragma once

#include "Systems/IExecutable.h"

class Main final : public IExecutable
{
public:
	void Initialize() override;
	void Destroy() override;

	void Tick() override;
	void Render() override;

	unsigned int GetExecutableCount() const { return Executables.size(); }
private:
	void Push(IExecutable* Executable);

private:
	vector<IExecutable*> Executables;
};