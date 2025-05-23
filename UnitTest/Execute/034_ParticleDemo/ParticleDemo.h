﻿#pragma once

#include "Environment/Weather/Precipitation.h"
#include "Systems/IExecutable.h"

namespace sdt
{
	class ParticleDemo final : public IExecutable
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
		void LoadSky();
		void LoadWeather();
		void LoadModel();
		void LoadCrossQuadGrass();
		
		void SetPlane();
		void SetModelsPosition(int MaxInstanceCount = 10, float Stride = 20);
		void MakeRandomGrasses(const Vector & Center, const Vector2D & Range, UINT Count, UINT MapIndex);
		Model * Plane = nullptr;
		vector<ModelInstanceData> ModelInstances;
		CrossQuad * Grasses = nullptr;
		SkySphere * Sky = nullptr;
		Precipitation * Rain = nullptr;
		
		ParticleSystem * Particle_Fire = nullptr;
	};
}
