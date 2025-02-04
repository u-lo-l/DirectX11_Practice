#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class InstancingDemo final : public IExecutable
	{
	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;
	private:
		Shader * Drawer = nullptr;

		Model * Plane = nullptr;
		Model * Cube = nullptr;
		vector<Vector> Positions = {};
		vector<Vector> Rotations = {};

		Model * Adam = nullptr;
	};
}
