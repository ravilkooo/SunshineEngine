#include "Graphics/Renderer/Technique/IconTechnique.h"


namespace SE_G {
	IconTechnique::IconTechnique(ID3D11Device* device,
		eastl::string technique,
		IconData iconData, float iconSizeX, float iconSizeY)
		: RenderTechnique(device, technique)
	{
		m_iconData = iconData;
		m_iconBuffer = eastl::make_shared<Bind::PixelConstantBuffer<IconData>>(device, iconData, 1u);

		CreateVertexBuffer(device, iconSizeX, iconSizeY);

		//m_geometryCB = eastl::make_shared<Bind::GeometryConstantBuffer<DXSM::Vector3>>(device, 1u);
	}

	void IconTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		BindAll(context);
		DrawTechnique(context);
	}

	void IconTechnique::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		m_vertexBuffer->Bind(context.Get());

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		m_iconBuffer->Bind(context.Get());

		//m_geometryCB->Update(context.Get(), )
		//m_geometryCB->Bind(context.Get());
	}

	void IconTechnique::DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
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