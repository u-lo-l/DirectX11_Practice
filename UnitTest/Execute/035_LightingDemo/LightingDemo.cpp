#include "Pch.h"
#include "LightingDemo.h"

namespace sdt
{
	void LightingDemo::Initialize()
	{
		Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 150, 150, 20 );
		MainCamera->SetRotation( 225, 0, 180);

		// Particle_Fire = new ParticleSystem(L"Fire");
		// Particle_Fire->SetScale(35.f);
		// LoadWeather();
		// LoadSky();
		// LoadModel();
		// LoadCrossQuadGrass();
		LoadLightingDemo();
	}

	void LightingDemo::Destroy()
	{
		SAFE_DELETE(Plane);
		for (ModelInstanceData ModelAndScale : ModelInstances)
		{
			SAFE_DELETE(ModelAndScale.Object);
		}
	}

	void LightingDemo::Tick()
	{
		if (!!Sky)
			Sky->Tick();
		if (!!Plane)
			Plane->Tick();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->Tick();
		for (const ModelInstanceData & P : ModelInstances_ForLighting)
			P.Object->Tick();
		if (!!Grasses)
			Grasses->Tick();
		if (!!Rain)
			Rain->Tick();
		if (!!Particle_Fire)
		{
			Particle_Fire->Add({0,5,0});
			Particle_Fire->Tick();
		}
	}

	void LightingDemo::Render()
	{
		if (!!Sky)
			Sky->Render();
		if (!!Plane)
			Plane->Render();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->Render();
		for (const ModelInstanceData & P : ModelInstances_ForLighting)
			P.Object->Render();
		if (!!Grasses)
			Grasses->Render();
		if (!!Rain)
			Rain->Render();
		if (!!Particle_Fire)
			Particle_Fire->Render();
	}

	void LightingDemo::LoadWeather()
	{
		Rain = new Precipitation(
			Vector(300, 100, 500),
			static_cast<UINT>(1e+4f),
			L"Environments/Rain.png",
			WeatherType::Rain
		);
	}
	void LightingDemo::LoadSky()
	{
		// Sky = new SkySphere(L"Environments/GrassCube1024.dds", 0.5f);
		Sky = new SkySphere(L"Environments/SunSetCube1024.dds", 0.5f);
	}

	void LightingDemo::LoadModel()
	{
		constexpr UINT InstanceCount = 36;
		constexpr float Stride = 40.f;
		if (!Plane)
			SetPlane();
		
		ModelInstances.insert(ModelInstances.end(), {
			{new Model(L"Adam"),{0.2f,0.2f,0.2f}, 15.f},
			{new Model(L"Sphere"),{0.05f,0.05f,0.05f}, 15.f},
		});
		SetModelsPosition_Square(ModelInstances, InstanceCount, Stride);
	}
	void LightingDemo::LoadLightingDemo()
	{
		if (!Plane)
			SetPlane();
		if (!!Plane)
		{
			Transform * tf = Plane->AddTransforms();
			tf->SetPosition({300,0,0});
			tf->SetScale({2.5,1,2.5});
			tf->SetRotation({0,0,90});
			tf->UpdateWorldMatrix();
			ModelInstances_ForLighting.insert(ModelInstances_ForLighting.end(), {
				{new Model(L"Cone"),{0.1f,0.1f,0.1f}, 15.f},
				{new Model(L"Cylinder"),{0.1f,0.1f,0.1f}, 15.f},
			});
			constexpr UINT InstanceCount = 5;
			constexpr float Stride = 50.f;
			SetModelsPosition_Line(ModelInstances_ForLighting, InstanceCount, Stride, {300,5,0});
		}
	}

	void LightingDemo::LoadCrossQuadGrass()
	{
		Grasses = new CrossQuad();
		Grasses->AddTexture(L"Terrain/Grass/grass_07.tga");
		Grasses->AddTexture(L"Terrain/Grass/grass_11.tga");
		Grasses->AddTexture(L"Terrain/Grass/grass_14.tga");
		MakeRandomGrasses({0,0,0}, {-125, 125}, 720, 0);
		MakeRandomGrasses({0,0,0}, {-125, 125}, 450, 1);
		MakeRandomGrasses({0,0,0}, {-125, 125}, 310, 2);
	}

	void LightingDemo::SetPlane()
	{
		Plane = new Model(L"Plane");
		Transform * tf = Plane->AddTransforms();
		tf->SetPosition({0,0,0});
		tf->SetScale({2.5,1,2.5});
		tf->SetRotation({0,0,90});
		tf->UpdateWorldMatrix();;
	}
	void LightingDemo::SetModelsPosition_Square(const vector<ModelInstanceData> & Models, int MaxInstanceCount, float Stride, const Vector& Center ) const
	{
		const int Width = static_cast<int>(sqrt(MaxInstanceCount));
		const int Height = (MaxInstanceCount + Width - 1) / Width;
		const int Depth = ModelInstances.size();
		Vector Offet = Center - 0.5f * Vector(static_cast<float>(Width - 1) * Stride ,0, static_cast<float>(Height - 1) * Stride);

		for (int ModelIndex = 0 ; ModelIndex < Depth ; ModelIndex++)
		{
			Vector InstancePosition;
			InstancePosition.Y = static_cast<float>(ModelIndex) * Stride;
			for (int H = 0 ; H < Height ; H++)
			{
				InstancePosition.Z = static_cast<float>(H) * Stride;// - static_cast<float>(Height) * Stride / 2;
				for (int W  = 0 ; W < Width ; W++)
				{
					const int InstanceIndex = H * Width + W;
					if (InstanceIndex >= MaxInstanceCount)
						break;
					InstancePosition.X = static_cast<float>(W) * Stride;// - static_cast<float>(Width) * Stride / 2;
					Transform * TF = Models[ModelIndex].Object->AddTransforms();
					TF->SetScale(Models[ModelIndex].Scale);
					TF->SetPosition(Offet + InstancePosition);
					TF->SetRotation({0, 0, Math::Random(-180.f, 180.f)});
					TF->UpdateWorldMatrix();

					const int AnimationNum = Models[ModelIndex].Object->GetClipCount();
					if (AnimationNum > 0)
					{
						Models[ModelIndex].Object->SetClipIndex(InstanceIndex, Math::Random(0, AnimationNum));
					}
				}
			}
		}
	}

	void LightingDemo::SetModelsPosition_Line
	(
		const vector<ModelInstanceData>& Models,
		int MaxInstanceCount,
		float Stride,
		const Vector& Center
	) const
	{
		const int Width = Models.size();
		const int Depth = (MaxInstanceCount);
		Vector Offet = Center - 0.5f * Vector(static_cast<float>(Width - 1) * Stride ,0, static_cast<float>(Depth - 1) * Stride);
		for (int ModelIndex = 0 ; ModelIndex < Width ; ModelIndex++)
		{
			Vector InstancePosition;
			InstancePosition.X = Stride * static_cast<float>(ModelIndex);
			for (int InstanceIndex = 0 ; InstanceIndex < Depth ; InstanceIndex++)
			{
				InstancePosition.Z = Stride * static_cast<float>(InstanceIndex);
				Transform * TF = Models[ModelIndex].Object->AddTransforms();
				TF->SetScale(Models[ModelIndex].Scale);
				TF->SetPosition(Offet + InstancePosition);
				TF->SetRotation({0, 0, Math::Random(-180.f, 180.f)});
				TF->UpdateWorldMatrix();

				const int AnimationNum = Models[ModelIndex].Object->GetClipCount();
				if (AnimationNum > 0)
				{
					Models[ModelIndex].Object->SetClipIndex(InstanceIndex, Math::Random(0, AnimationNum));
				}
			}
		}
	}

	void LightingDemo::MakeRandomGrasses(const Vector& Center, const Vector2D& Range, UINT Count, UINT MapIndex)
	{
		for (UINT i = 0; i < Count; i++)
		{
			Vector2D scale;
			scale.X = Math::Random(5.f, 8.f);
			scale.Y = Math::Random(5.f, 8.f);

			Vector position;
			position.X = Math::Random(Center.X + Range.X, Center.X + Range.Y);
			position.Z = Math::Random(Center.Z + Range.X, Center.Z + Range.Y);
			position.Y = scale.Y * 0.5f + Center.Y;

			Grasses->Add(position, scale, MapIndex);
		}
	}
}
