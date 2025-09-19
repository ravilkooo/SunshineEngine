#pragma once

#include "Bindable.h"

#include "BlendState.h"
#include "Topology.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "TextureB.h"
#include "VertexShader.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "TransformCBuffer.h"
#include "ConstantBuffer.h"
#include "Rasterizer.h"
#include "Sampler.h"
#include "DepthStencilState.h"

/*
Bindables tips

1. InputLayout needs Vertex shader's Bytecode to Init

2.  Possible Order
{
	1) Topology
	2) VertexBuffer
	3) IndexBuffer
	4) TextureB
	5) VertexShader
	6) InputLayout
	7) PixelShader
	8) Vertex_CBuffer
	9) Index_CBuffer
	10) Transform_CBuffer
	11) Rasterizer
}

*/