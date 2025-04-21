#pragma once

#include "Systems/IExecutable.h"
#include "Renders/Shader/Hlsl2DTextureShader.h"

namespace sdt
{
	class DisplacementMappingDemo final : public IExecutable
	{
	private:
		using VertexType = VertexTextureNormal;		
	public:
		void Initialize() override;
		void Destroy() override;
		
		void Tick() override;
		void Render() override;

		HlslShader<VertexType> * DisplacementMappingShader;
		Texture * HeightMap;

		struct DisplacementFactorDesc
		{
			float Displacement = 0;
			float Padding[3];
		} DisplacementFactor;
		struct WVPDesc
		{
			Matrix World;
			Matrix View;
			Matrix Projection;
		} WVP;

		vector<VertexType> Vertices;
		vector<UINT> Indices;
		VertexBuffer * VBuffer = nullptr;
		IndexBuffer * IBuffer = nullptr;
		
		ConstantBuffer * WVPBuffer_DS;
		ConstantBuffer * DisplacementBuffer_DS;
	};
}
