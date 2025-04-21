#pragma once

#include "Systems/IExecutable.h"
#include "Renders/Shader/Hlsl2DTextureShader.h"

namespace sdt
{
	class PNTriangleDemo final : public IExecutable
	{
	private:
		using VertexType = VertexNormal;		
	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

		vector<VertexType> Vertices;
		VertexBuffer * VBuffer;
		vector<UINT> Indices;
		IndexBuffer * IBuffer;

		HlslShader<VertexType> * Shader;

		struct TessellationFactorDesc
		{
			int Edge[4] = {1,1,1,1};
			int Inner[2] = {0,0};
			float Padding[2];
		} TessellationFactor;
		struct WVPDesc
		{
			Matrix World;
			Matrix View;
			Matrix Projection;
		} WVP;
		ConstantBuffer * WVPBuffer_DS;
		ConstantBuffer * TessellationFactorBuffer_HS;
	};
}
