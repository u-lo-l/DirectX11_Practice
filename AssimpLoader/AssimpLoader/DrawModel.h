#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class DrawModel final : public IExecutable
	{
	public:
		~DrawModel() override;
		void Initialize() override;
		void Destroy() override;
		void Tick() override;
		void Render() override;
	private:
		vector<Model *> Models;
		int ModelIndex = 0;


	/*=========================================================================*/

	private:
		vector<Vector> Positions;
		Model * Sphere = nullptr;
		void SetBoneIndicators();
	};
}
