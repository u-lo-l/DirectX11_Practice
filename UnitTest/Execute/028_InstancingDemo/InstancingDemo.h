#pragma once

#include "Systems/IExecutable.h"

namespace sdt
{
	class InstancingDemo final : public IExecutable
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
		void SetPlane();
		void SetModelsPosition(int MaxInstanceCount = 10, float Stride = 20);
		
	private:
		Model * Plane = nullptr;
		vector<ModelInstanceData> ModelInstances;

	};
}
