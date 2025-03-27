#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class NonInstancing_Vertex final : public IExecutable
	{
		static constexpr UINT MaxInstanceCount = 100000;
		using VertexType = VertexTextureNormal;
	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

		void CreateCube(float InSize = 1.0f);
	private:
		Shader * Drawer = nullptr;
	
		vector<VertexType> Vertices;
		vector<UINT> Indices;

		VertexBuffer * VBuffer = nullptr;
		IndexBuffer * IBuffer = nullptr;
		ConstantDataBinder * PerFrame = nullptr;

		Texture * DiffuseMap = nullptr;
		IESRV_t * sDiffuseMap = nullptr;

		Transform* Transforms[MaxInstanceCount] = {};
	};
}
