#include "Pch.h"
#include "LightingDemo.h"

namespace sdt
{
	void LightingDemo::Initialize()
	{
		RT = new RenderTarget(static_cast<UINT>(D3D::GetDesc().Width),static_cast<UINT>(D3D::GetDesc().Height));
		DS = new DepthStencil(static_cast<UINT>(D3D::GetDesc().Width),static_cast<UINT>(D3D::GetDesc().Height), false);
		TextureShader = new Hlsl2DTextureShader(*RT);
		float TextureWidth = 0.2f * D3D::GetDesc().Width;
		float TextureHeight = 0.2f * D3D::GetDesc().Height;
		TextureShader->GetTransform()->SetScale({TextureWidth, TextureHeight, 1});
		// TextureShader->GetTransform()->SetPosition({-1 + TextureWidth / 2,-1 + TextureHeight / 2, 0});
		TextureShader->GetTransform()->SetPosition({TextureWidth / 2,TextureHeight / 2, 0});
		
		Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 150, 150, 20 );
		MainCamera->SetRotation( 225, 0, 180);

		PEffect = new PostEffect(L"PostEffect/PostEffect.hlsl", *RT);
		// Particle_Fire = new ParticleSystem(L"Fire");
		// Particle_Fire->SetScale(35.f);
		// LoadWeather();
		LoadSky();
		LoadModel();
		LoadCrossQuadGrass();
		LoadLightingDemo();
	}

	void LightingDemo::Destroy()
	{
		SAFE_DELETE(Plane);
		for (ModelInstanceData ModelAndScale : ModelInstances)
		{
			SAFE_DELETE(ModelAndScale.Object);
		}
		for (ModelInstanceData ModelAndScale : ModelInstances_ForLighting)
		{
			SAFE_DELETE(ModelAndScale.Object);
		}
		SAFE_DELETE(Grasses);
		SAFE_DELETE(Sky);
		SAFE_DELETE(Rain);
		SAFE_DELETE(Particle_Fire);
		SAFE_DELETE(RT);
		SAFE_DELETE(DS);
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
		if (!!RT && Keyboard::Get()->IsPressed(VK_SPACE))
		{
			RT->SaveTexture(L"TEST");
		}
		
		if (!!PEffect)
			PEffect->Tick();
		if (!!TextureShader)
			TextureShader->Tick();
	}

	void LightingDemo::Render()
	{
	}

	void LightingDemo::PostRender()
	{
		if (!!PEffect)
			PEffect->Render();

		
		if (!!TextureShader) // 원본 축소
			TextureShader->Render();
	}

	void LightingDemo::PreRender()
	{
		RT->SetRenderTarget(DS);
		RT->ClearRenderTarget(); // Render 하기 전에 Clear
		DS->ClearDepthStencil(); // Render 하기 전에 Clear
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
			CreatePointLights();
			CreateSpotLights();
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
		Plane->SetTiling({5,5});
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

	void LightingDemo::CreatePointLights()
	{
		PointLight light;

		light =
		{
			Color(0, 0, 0, 1), //Ambient
			Color(0, 0, 1, 1), //Diffuse
			Color(0, 0, 0.7f, 1), //Specular
			Color(0, 0, 0.7f, 1), //Emissive
			Vector(-1.0f, 0.5f, -4.0f)+ Vector(300,0,0), //Position
			2, //Range
			0.9f //Intensity
		};
		LightingManager::Get()->AddPointLight(light);

		light =
		{
			Color(0.0f, 0.0f, 0.0f, 1.0f),
			Color(1.0f, 0.0f, 0.0f, 1.0f),
			Color(0.6f, 0.2f, 0.0f, 1.0f),
			Color(0.6f, 0.3f, 0.0f, 1.0f),
			Vector(+1.0f, 0.5f, -4.0f)+ Vector(300,0,0), 
			2.0f, 
			0.3f
		};
		LightingManager::Get()->AddPointLight(light);
	}

	void LightingDemo::CreateSpotLights()
	{
		SpotLight light;
		light =
		{
			Color(0.3f, 1.0f, 0.0f, 1.0f),
			Color(0.7f, 1.0f, 0.0f, 1.0f),
			Color(0.3f, 1.0f, 0.0f, 1.0f),
			Color(0.3f, 1.0f, 0.0f, 1.0f),
			Vector(-2.5f, 5.0f, +3.0f) + Vector(300,0,0), 
			25.0f,
			Vector(0, -1, 0), 
			30.0f, 
			45.f,
			90.f
		 };
		LightingManager::Get()->AddSpotLight(light);

		light =
		{
			Color(1.0f, 0.2f, 0.9f, 1.0f),
			Color(1.0f, 0.2f, 0.9f, 1.0f),
			Color(1.0f, 0.2f, 0.9f, 1.0f),
			Color(1.0f, 0.2f, 0.9f, 1.0f),
			Vector(0.0f, 5.0f, +3.0f)+ Vector(300,0,0), 
			30.0f,
			Vector(0, -1, 0), 
			40.0f,
			20,
			45
			// 20 * Math::DegToRadian,
			// 45 * Math::DegToRadian
		};
		LightingManager::Get()->AddSpotLight(light);

		light =
		{
			Color(0.0f, 0.2f, 0.9f, 1.0f),
			Color(0.0f, 0.2f, 0.9f, 1.0f),
			Color(0.0f, 0.2f, 0.9f, 1.0f),
			Color(0.0f, 0.2f, 0.9f, 1.0f),
			Vector(2.5f, 5.0f, +3.0f)+ Vector(300,0,0),
			35.0f,
			Vector(0, -1, 0),
			20.0f,
			60 * Math::DegToRadian,
			90 * Math::DegToRadian
		};
		LightingManager::Get()->AddSpotLight(light);
	}

}
