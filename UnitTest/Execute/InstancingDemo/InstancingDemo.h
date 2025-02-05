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
		Model * Airplane = nullptr;
		Model * Sphere = nullptr;

		vector<Model *> Models;
		vector<Vector> Scales;
		vector<float> PosZ;
	};
}
