#pragma once

#include "Systems/IExecutable.h"

namespace sdt
{
	class CrossQuadDemo final : public IExecutable
	{
	private:
		struct ModelInstanceData
		{
			Model* Object;
			Vector Scale;
			float Height;
		};
	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;
	private:
		void LoadModel();
		void SetPlane();
		void SetModelsPosition(int MaxInstanceCount = 10, float Stride = 20);

		void LoadCrossQuadGrass();
		void MakeRandomGrasses(const Vector & Center, const Vector2D & Range, UINT Count, UINT MapIndex);
		
	private:
		Model * Plane = nullptr;
		vector<ModelInstanceData> ModelInstances;

		CrossQuad * Grasses = nullptr;
	};
}
