#pragma once

#include "Environment/Weather/Precipitation.h"
#include "Systems/IExecutable.h"
#include "Renders/Shader/Hlsl2DTextureShader.h"

namespace sdt
{
	class ShadowDemo final : public IExecutable
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
		void PreRender() override;
		void Render() override;
		void PostRender() override;
		
	private:
		void LoadSky();
		void LoadWeather();
		void LoadModel();
		void LoadCrossQuadGrass();

		void LoadLightingDemo();
		
		void SetPlane();
		void SetModelsPosition_Square(const vector<ModelInstanceData> & Models, int MaxInstanceCount = 10, float Stride = 20, const Vector& Center = {0,0,0}) const;
		void SetModelsPosition_Line(const vector<ModelInstanceData> & Models, int MaxInstanceCount = 10, float Stride = 20, const Vector& Center = {0,0,0}) const;
		void MakeRandomGrasses(const Vector & Center, const Vector2D & Range, UINT Count, UINT MapIndex);
		void CreatePointLights();
		void CreateSpotLights();
		Model * Plane = nullptr;
		vector<ModelInstanceData> ModelInstances;
		vector<ModelInstanceData> ModelInstances_ForLighting;
		CrossQuad * Grasses = nullptr;
		SkySphere * Sky = nullptr;
		Precipitation * Rain = nullptr;
		
		ParticleSystem * Particle_Fire = nullptr;

		RenderTarget * Diffuse_RT = nullptr;
		DepthStencil * DS = nullptr;

		Hlsl2DTextureShader * TextureShader = nullptr;

		Projector * Decal = nullptr;
	};
}
