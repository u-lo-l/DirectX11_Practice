﻿#pragma once

#include "Systems/IExecutable.h"
#include "Renders/Shader/HlslShader.hpp"

namespace Sdt
{
	class TerrainDemo final : public IExecutable
	{
	public:
		using VertexType = VertexColor;
	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;
	private:
		Terrain * Landscape = nullptr;
		Vector Position = Vector::Zero;

		HlslShader<VertexType> * TerrainNormalDrawer = nullptr;

	// // for drawing normal vectors
	// 	Shader * Drawer = nullptr;
	// 	VertexBuffer * VBuffer = nullptr;
	// 	vector<VertexType> Vertice;
	//
	// // for drawing Y
	// 	Shader * Drawer2 = nullptr;
	// 	VertexBuffer * VBuffer2 = nullptr;
	// 	vector<VertexType> Vertice2;
	};
}
