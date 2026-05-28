#include <Graphics/Renderer/Pass/ColliderPass.h>
#include <Graphics/Renderer/Technique/ColliderTechnique.h>

#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/GBuffer.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>

#include <Graphics/Bindable/DepthStencilState.h>
#include <Graphics/Bindable/IndexBuffer.h>
#include <Graphics/Bindable/Sampler.h>
#include <Graphics/Bindable/Topology.h>

#include <Graphics/GraphicsResources/PixelShader.h>

#include <Graphics/Utils/Camera.h>

#include <Component/TransformComponent.h>

#include <ResourceManager/ResourceManagerFacade.h>

#include <Utils/StringUtils.h>
#include <Utils/MathUtils.h>

namespace SE_G {
	eastl::unique_ptr<Bind::Topology> ColliderPass::s_topology;
	eastl::unique_ptr<Bind::IndexBuffer> ColliderPass::s_shapesIndexBuffer;
	eastl::unique_ptr<Bind::VertexBuffer> ColliderPass::s_shapesVertexBuffer;
	eastl::unordered_map<SE::ColliderShapeType, SE::ColliderBufferOffset> ColliderPass::s_shapeBufferOffsets;

	bool ColliderPass::s_staticDataInitializated = false;

	ColliderPass::ColliderPass(ID3D11Device* device, ID3D11DeviceContext* context,
		eastl::shared_ptr<GBuffer> pGBuffer,
		eastl::shared_ptr<Camera> camera)
		:
		RenderPass("ColliderPass", device, context)
	{
		if (!s_staticDataInitializated)
		{
			ColliderPass::InitStaticData(device);
		}

		this->m_GBuffer = pGBuffer;
		this->m_camera = camera;
		this->m_screenWidth = pGBuffer->m_screenWidth;
		this->m_screenHeight = pGBuffer->m_screenHeight;
		m_passType = PassType::Collider;

		// Set RTVs
		m_bufferRTVs[0] = pGBuffer->pLightRTV.Get();

		// Viewport
		m_viewport = {};
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1.0f;

		D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		m_depthStencilState = eastl::make_unique<Bind::DepthStencilState>(device, dsDesc);

		// Usual sampler for all SRV
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_GBufferSampler = eastl::make_unique<Bind::Sampler>(device, samplerDesc, 0u);
		AddPerFrameBind(m_GBufferSampler.get());

		// m_pixelShader = eastl::make_unique<Bind::PixelShader>(device,
		// 	MakeEngineAssetPath_Wstring(L"Shaders/ColliderPass/ColliderShaderPS.hlsl").c_str());
		AssetPath shaderPath = AssetPath(L"Shaders/ColliderPass/ColliderShaderPS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::PIXEL_SHADER;
		auto& rm = ResourceManagerFacade::Instance();
		ResourceHandle pshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::PixelShader* pshaderRes = rm.Get<SE_G::Bind::PixelShader>(pshaderHandle);
		m_pixelShader = eastl::shared_ptr<SE_G::Bind::PixelShader>(
			pshaderRes,
			[](SE_G::Bind::PixelShader*) {}
		);
	}

	ColliderPass::~ColliderPass() {
		//delete[] m_bufferRTVs;
	}

	void ColliderPass::StartFrame()
	{
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"Collider Pass");

		context->OMSetRenderTargets(2, m_bufferRTVs, m_GBuffer->pDepthDSV.Get());

		context->RSSetViewports(1, &m_viewport);

		m_camera->UpdateBuffer(context.Get());
		m_camera->BindBuffer(context.Get());
	}

	void ColliderPass::Pass()
	{
		BindAllPerFrame();

		// VertexBuffer with all default shapes
		s_topology->Bind(context.Get());
		m_depthStencilState->Bind(context.Get());
		m_pixelShader->Bind(context.Get());

		// Default shapes
		s_shapesVertexBuffer->Bind(context.Get());
		s_shapesIndexBuffer->Bind(context.Get());
		for (auto& tech : m_techniques)
		{
			DXSM::Vector3 old_localScaleFactor = tech.second->m_assignedTransform->m_localScaleFactor;
			DXSM::Vector3 old_localRotation = tech.second->m_assignedTransform->m_localRotation;
			DXSM::Vector3 old_localPosition = tech.second->m_assignedTransform->m_localPosition;

			tech.second->m_assignedTransform->EnableMeshTransformMode();
			DXSM::Matrix fullTransform = tech.second->m_assignedTransform->GetWorldMatrix_noLocal();
			DXSM::Vector3 scale;
			DXSM::Vector3 rotate;
			DXSM::Vector3 translation;
			DecomposeTransform(fullTransform, scale, rotate, translation);
			
			tech.second->m_assignedTransform->m_localPosition = DXSM::Vector3::Zero;
			tech.second->m_assignedTransform->m_localRotation = DXSM::Vector3::Zero;
			tech.second->m_assignedTransform->m_localScaleFactor = DXSM::Vector3(
				1.0f / scale.x,
				1.0f / scale.y,
				1.0f / scale.z
			);
			tech.second->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
			tech.second->Pass(GetDeviceContext());
			tech.second->m_assignedTransform->DisableMeshTransformMode();

			tech.second->m_assignedTransform->m_localPosition = old_localPosition;
			tech.second->m_assignedTransform->m_localRotation = old_localRotation;
			tech.second->m_assignedTransform->m_localScaleFactor = old_localScaleFactor;
		}

		// Custom shapes
		for (auto& tech : m_customTechniques)
		{
			DXSM::Matrix fullTransform = tech->m_assignedTransform->GetWorldMatrix_noLocal();
			DXSM::Vector3 scale;
			DXSM::Vector3 rotate;
			DXSM::Vector3 translation;
			DecomposeTransform(fullTransform, scale, rotate, translation);

			DXSM::Vector3 old_scaleFactor = tech->m_assignedTransform->m_scaleFactor;
			tech->m_assignedTransform->m_scaleFactor = DXSM::Vector3(
				old_scaleFactor.x / scale.x,
				old_scaleFactor.y / scale.y,
				old_scaleFactor.z / scale.z
			);

			tech->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
			tech->Pass(GetDeviceContext());

			tech->m_assignedTransform->m_scaleFactor = old_scaleFactor;
		}
	}

	void ColliderPass::EndFrame()
	{
		ID3D11RenderTargetView* nullRTVs[] = { nullptr };
		ID3D11DepthStencilView* nullDSVs[] = { nullptr };
		context->OMSetRenderTargets(1, nullRTVs, *nullDSVs);

		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
	}

	void ColliderPass::OnResize(UINT resizeWidth, UINT resizeHeight)
	{
		m_screenWidth = resizeWidth;
		m_screenHeight = resizeHeight;

		// Set RTVs
		m_bufferRTVs[0] = m_GBuffer->pLightRTV.Get();

		// Viewport
		m_viewport = {};
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1.0f;
	}

	void ColliderPass::InitStaticData(ID3D11Device* device)
	{
		// Circle segments
		constexpr UINT segments = 32u;
		const UINT arcSegments = segments / 2u;
		constexpr float step = DX::XM_2PI / float(segments);
		constexpr float halfHeight = 0.5f;

		UINT vertexOffset = 0u;
		UINT indexOffset = 0u;

		s_shapeBufferOffsets[SE::ColliderShapeType::Box] =
			SE::ColliderBufferOffset{ 
				vertexOffset, 8u,
				indexOffset, 24u };
		vertexOffset += 8u;
		indexOffset += 24u;

		s_shapeBufferOffsets[SE::ColliderShapeType::Sphere] =
			SE::ColliderBufferOffset{
				vertexOffset, 3u * segments,
				indexOffset, 3u * segments * 2u };
		vertexOffset += 3u * segments;
		indexOffset += 3u * segments * 2u;

		s_shapeBufferOffsets[SE::ColliderShapeType::Capsule] =
			SE::ColliderBufferOffset{
				vertexOffset, 2u * segments + 4u * (arcSegments + 1u),
				indexOffset, 2u * 2u * segments + 4u * 2u * arcSegments + 4u * 2u };
		s_shapeBufferOffsets[SE::ColliderShapeType::TaperedCapsule] =
			SE::ColliderBufferOffset(s_shapeBufferOffsets[SE::ColliderShapeType::Capsule]);
		vertexOffset += 2u * segments + 4u * (arcSegments + 1u);
		indexOffset += 2u * 2u * segments + 4u * 2u * arcSegments + 4u * 2u;

		eastl::vector<SE::ColliderVertex> vertices;
		eastl::vector<UINT> indices;

		//vertices.reserve(vertexOffset);
		//indices.reserve(indexOffset);

		vertices.reserve(0u);
		indices.reserve(0u);

		// Box
		{
			// 8 corners of the box
			SE::ColliderVertex v[8] =
			{
				SE::ColliderVertex{{-0.5f, -0.5f, -0.5f}}, // 0
				SE::ColliderVertex{{+0.5f, -0.5f, -0.5f}}, // 1
				SE::ColliderVertex{{+0.5f, +0.5f, -0.5f}}, // 2
				SE::ColliderVertex{{-0.5f, +0.5f, -0.5f}}, // 3
				SE::ColliderVertex{{-0.5f, -0.5f, +0.5f}}, // 4
				SE::ColliderVertex{{+0.5f, -0.5f, +0.5f}}, // 5
				SE::ColliderVertex{{+0.5f, +0.5f, +0.5f}}, // 6
				SE::ColliderVertex{{-0.5f, +0.5f, +0.5f}}, // 7
			};

			for (size_t i = 0; i < 8; i++)
			{
				vertices.push_back(v[i]);
			}
			// 12 edges as 24 indices (line list)
			const UINT idx[] =
			{
				0u,1u, 1u,2u, 2u,3u, 3u,0u,    // bottom rectangle
				4u,5u, 5u,6u, 6u,7u, 7u,4u,    // top rectangle
				0u,4u, 1u,5u, 2u,6u, 3u,7u     // vertical edges
			};
			for (size_t i = 0; i < 24; i++)
			{
				indices.push_back(idx[i]);
			}
		}

		// Sphere
		{
			// Helper lambda: add one circle in a given plane
			auto addCircle = [&](int plane) // 0 = XY, 1 = YZ, 2 = XZ
				{
					const UINT baseIndex = static_cast<UINT>(vertices.size());

					for (UINT i = 0u; i < segments; ++i)
					{
						float a = step * float(i);
						float c = cosf(a);
						float s = sinf(a);

						SE::ColliderVertex v{};
						switch (plane)
						{
						case 0: v.position = DXSM::Vector3(c, s, 0.0f);      break; // XY
						case 1: v.position = DXSM::Vector3(0.0f, c, s);      break; // YZ
						case 2: v.position = DXSM::Vector3(c, 0.0f, s);      break; // XZ
						}
						vertices.push_back(v);
					}

					// indices: connect i -> i+1, last -> first
					for (UINT i = 0u; i < segments; ++i)
					{
						UINT aIdx = baseIndex + UINT(i);
						UINT bIdx = baseIndex + UINT((i + 1u) % segments);
						indices.push_back(aIdx);
						indices.push_back(bIdx);
					}
				};

			addCircle(0); // XY
			addCircle(1); // YZ
			addCircle(2); // XZ
		}

		// Capsule/TaperedCapsule
		{
			auto addCircleXZ = [&](float yCenter) -> UINT
				{
					UINT base = static_cast<UINT>(vertices.size());
					for (UINT i = 0u; i < segments; ++i)
					{
						float a = step * float(i);
						float c = cosf(a);
						float s = sinf(a);
						vertices.push_back({ DXSM::Vector3(c, yCenter, s) });
					}
					for (UINT i = 0u; i < segments; ++i)
					{
						UINT aIdx = base + UINT(i);
						UINT bIdx = base + UINT((i + 1u) % segments);
						indices.push_back(aIdx);
						indices.push_back(bIdx);
					}
					return base;
				};

			auto addVerticalSemiCircle = [&](bool top, bool inXZPlane) // top/bottom, and which vertical plane
				{
					const float yCenter = top ? +halfHeight : -halfHeight;
					const float start = top ? 0.0f : DX::XM_PI;        // upper or lower half
					const float end = top ? DX::XM_PI : DX::XM_2PI;

					UINT base = static_cast<UINT>(vertices.size());

					for (UINT i = 0u; i <= arcSegments; ++i)
					{
						float t = float(i) / float(arcSegments);
						float a = start + (end - start) * t;
						float c = cosf(a);
						float s = sinf(a);

						SE::ColliderVertex v{};
						if (inXZPlane)          // X‑Y plane (z = 0)
							v.position = DXSM::Vector3(c, yCenter + s, 0.0f);
						else                    // Z‑Y plane (x = 0)
							v.position = DXSM::Vector3(0.0f, yCenter + s, c);

						vertices.push_back(v);
					}

					for (UINT i = 0u; i < arcSegments; ++i)
					{
						UINT aIdx = base + UINT(i);
						UINT bIdx = base + UINT(i + 1u);
						indices.push_back(aIdx);
						indices.push_back(bIdx);
					}
				};

			// Upper and lower equator circles (rings where caps meet cylinder)
			UINT baseTop = addCircleXZ(+halfHeight);
			UINT baseBottom = addCircleXZ(-halfHeight);

			// Four lines connecting the two rings at 0, 90, 180, 270 degrees
			for (UINT k = 0u; k < 4u; ++k)
			{
				UINT idxOnCircle = (k * segments) / 4u;     // quarter points
				UINT topIdx = baseTop + UINT(idxOnCircle);
				UINT bottomIdx = baseBottom + UINT(idxOnCircle);
				indices.push_back(topIdx);
				indices.push_back(bottomIdx);
			}

			// Vertical semi‑circles forming the hemispherical caps
			addVerticalSemiCircle(true, true);   // top,   X‑Y plane
			addVerticalSemiCircle(true, false);  // top,   Y‑Z plane
			addVerticalSemiCircle(false, true);   // bottom X‑Y plane
			addVerticalSemiCircle(false, false);  // bottom Y‑Z plane
		}

		s_shapesVertexBuffer = eastl::make_unique<Bind::VertexBuffer>(
			device, vertices.data(), vertices.size(), sizeof(SE::ColliderVertex));
		s_shapesIndexBuffer = eastl::make_unique<Bind::IndexBuffer>(
			device, indices.data(), indices.size());
		s_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		s_staticDataInitializated = true;
	}

	TriggerPass::TriggerPass(ID3D11Device* device, ID3D11DeviceContext* context,
		eastl::shared_ptr<GBuffer> pGBuffer,
		eastl::shared_ptr<Camera> camera)
		: ColliderPass(device, context, pGBuffer, camera)
	{
		techniqueTag = "TriggerPass";
		m_passType = PassType::Trigger;
	}
}