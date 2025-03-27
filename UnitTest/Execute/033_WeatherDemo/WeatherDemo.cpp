#include "Pch.h"
#include "WeatherDemo.h"

namespace sdt
{
	void WeatherDemo::Initialize()
	{
		Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( -262, 175, -247 );
		MainCamera->SetRotation( 210, 135, 180);

		LoadWeather();
		LoadSky();
		LoadModel();
		LoadCrossQuadGrass();
	}

	void WeatherDemo::Destroy()
	{
		SAFE_DELETE(Plane);
		for (ModelInstanceData ModelAndScale : ModelInstances)
		{
			SAFE_DELETE(ModelAndScale.Object);
		}
	}

	void WeatherDemo::Tick()
	{
		if (!!Sky)
			Sky->Tick();
		if (!!Plane)
			Plane->Tick();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->Tick();
		if (!!Grasses)
			Grasses->Tick();
		if (!!Rain)
			Rain->Tick();
	}

	void WeatherDemo::Render()
	{
		if (!!Sky)
			Sky->Render();
		if (!!Plane)
			Plane->Render();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->Render();
		if (!!Grasses)
			Grasses->Render();
		if (!!Rain)
			Rain->Render();
	}

	void WeatherDemo::LoadWeather()
	{
		Rain = new Precipitation(
			Vector(300, 100, 500),
			static_cast<UINT>(1e+4f),
			L"Environments/Rain.png",
			WeatherType::Rain
		);
	}
	void WeatherDemo::LoadSky()
	{
		// Sky = new SkySphere(L"Environments/GrassCube1024.dds", 0.5f);
		Sky = new SkySphere(L"Environments/SunSetCube1024.dds", 0.5f);
	}

	void WeatherDemo::LoadModel()
	{
		constexpr UINT InstanceCount = 36;
		constexpr float Stride = 40.f;
		SetPlane();
		
		ModelInstances.insert(ModelInstances.end(), {
			{new Model(L"Adam"),{0.2f,0.2f,0.2f}, 15.f},
			{new Model(L"Cube"),{0.05f,0.05f,0.05f}, 15.f},
		});
		SetModelsPosition(InstanceCount, Stride);
	}

	void WeatherDemo::LoadCrossQuadGrass()
	{
		Grasses = new CrossQuad();
		Grasses->AddTexture(L"Terrain/Grass/grass_07.tga");
		Grasses->AddTexture(L"Terrain/Grass/grass_11.tga");
		Grasses->AddTexture(L"Terrain/Grass/grass_14.tga");
		MakeRandomGrasses({0,0,0}, {-125, 125}, 720, 0);
		MakeRandomGrasses({0,0,0}, {-125, 125}, 450, 1);
		MakeRandomGrasses({0,0,0}, {-125, 125}, 310, 2);
	}
	void WeatherDemo::SetPlane()
	{
		Plane = new Model(L"Plane");
		Transform * tf = Plane->AddTransforms();
		tf->SetPosition({0,0,0});
		tf->SetScale({2.5,1,2.5});
		tf->SetRotation({0,0,90});
		tf->UpdateWorldMatrix();;
	}
	void WeatherDemo::SetModelsPosition( int MaxInstanceCount, float Stride )
	{
		const int Width = static_cast<int>(sqrt(MaxInstanceCount));
		const int Height = (MaxInstanceCount + Width - 1) / Width;
		const int Depth = ModelInstances.size();
		
		for (int ModelIndex = 0 ; ModelIndex < Depth ; ModelIndex++)
		{
			Vector InstancePosition;
			InstancePosition.Y = static_cast<float>(ModelIndex) * Stride;
			for (int H = 0 ; H < Height ; H++)
			{
				InstancePosition.Z = static_cast<float>(H) * Stride - static_cast<float>(Height) * Stride / 2;
				for (int W  = 0 ; W < Width ; W++)
				{
					const int InstanceIndex = H * Width + W;
					if (InstanceIndex >= MaxInstanceCount)
						break;
					InstancePosition.X = static_cast<float>(W) * Stride - static_cast<float>(Width) * Stride / 2;
					Transform * TF = ModelInstances[ModelIndex].Object->AddTransforms();
					TF->SetScale(ModelInstances[ModelIndex].Scale);
					TF->SetPosition(InstancePosition);
					TF->SetRotation({0, 0, Math::Random(-180.f, 180.f)});
					TF->UpdateWorldMatrix();

					const int AnimationNum = ModelInstances[ModelIndex].Object->GetClipCount();
					if (AnimationNum > 0)
					{
						ModelInstances[ModelIndex].Object->SetClipIndex(InstanceIndex, Math::Random(0, AnimationNum));
					}
				}
			}
		}
	}
	void WeatherDemo::MakeRandomGrasses(const Vector& Center, const Vector2D& Range, UINT Count, UINT MapIndex)
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
