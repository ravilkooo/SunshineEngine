#include <d3d11.h>

#include "Graphics/Renderer/Technique/IconTechnique.h"

#include <Graphics/Bindable/ConstantBuffer.h>
#include "Graphics/Bindable/VertexBuffer.h"

#include <Graphics/GraphicsResources/VertexShader.h>

namespace SE_G {
	IconTechnique::IconTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
		eastl::string technique,
		IconData iconData, float iconSizeX, float iconSizeY)
		: RenderTechnique(device, assignedTransform, technique)
	{
		m_iconData = iconData;
		m_iconBuffer = eastl::make_shared<Bind::PixelConstantBuffer<IconData>>(device, iconData, 1u);

		CreateVertexBuffer(device, iconSizeX, iconSizeY);
	}

	void IconTechnique::Pass(ID3D11DeviceContext* context)
	{
		BindAll(context);
		DrawTechnique(context);
	}

	void IconTechnique::BindAll(ID3D11DeviceContext* context)
	{
		m_vertexBuffer->Bind(context);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		m_iconBuffer->Bind(context);
	}

	void IconTechnique::DrawTechnique(ID3D11DeviceContext* context)
	{
		context->Draw(1u, 0);
	}

	void IconTechnique::CreateVertexBuffer(ID3D11Device* device, float iconSizeX, float iconSizeY) {
		IconVertex iconVertex =
		{ { 0.0f, 0.0f, 0.0f }, { iconSizeX, iconSizeY } };

		// qwerty
		m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(
			device, &iconVertex, 1, sizeof(IconVertex));
	}
}