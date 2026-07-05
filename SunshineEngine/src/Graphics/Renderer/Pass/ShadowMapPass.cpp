#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/RenderingSystem.h>

#include <Graphics/Renderer/Pass/ShadowMapPass.h>
#include <Graphics/Renderer/Pass/GPass.h>
#include <Graphics/Renderer/Technique/GPassTechnique.h>

#include <Graphics/Bindable/Rasterizer.h>
#include <Graphics/Bindable/Sampler.h>
#include <Graphics/Bindable/ConstantBuffer.h>

#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/GraphicsResources/Texture.h>

#include <Graphics/GraphicsResources/Mesh.h>

#include <Graphics/Utils/Camera.h>

#include <Component/TransformComponent.h>
#include <Component/MeshComponent.h>

#include <ResourceManager/ResourceManagerFacade.h>

#include <Utils/StringUtils.h>

namespace SE_G {

	ShadowMapPass::ShadowMapPass(DeferredRenderer* renderer,
		GPass* gPass,
		eastl::shared_ptr<SE_G::DirectionalLightData> lightData,
		UINT smSizeX, UINT smSizeY)
		:
		RenderPass("ShadowMapPass", renderer),
		m_shadowMap(ShadowMap(m_renderer->GetDevice(), smSizeX, smSizeY)),
		m_gPass(gPass),
		m_lightData(lightData)
	{
		m_passType = PassType::Shadow;

		m_playerCamera = gPass->m_renderer->GetMainCamera().get();

		auto device = m_renderer->GetDevice();
		m_lightViewCamera = eastl::make_unique<SE_G::Camera>(device,
			m_shadowMap.m_mapWidth / m_shadowMap.m_mapHeight);
		m_lightViewCamera->SetPosition(lightData->Position);
		DXSM::Vector3 direction = DXSM::Vector3(
			cos(lightData->Direction.y) * cos(lightData->Direction.x),
			sin(lightData->Direction.y),
			cos(lightData->Direction.y) * sin(lightData->Direction.x)
		);
		m_lightViewCamera->SetTarget(lightData->Position + direction);
		DXSM::Vector3 cameraUp = { 0, 1, 0 };
		cameraUp.Normalize();
		m_lightViewCamera->SetUp(cameraUp);
		m_lightViewCamera->SwitchProjection();
		m_lightViewCamera->SetViewWidth(100.0f);
		m_lightViewCamera->SetViewHeight(100.0f);

		m_lightViewCamera->SetNearZ(cascadeBounds[2]);
		m_lightViewCamera->SetFarZ(cascadeBounds[3]);

		// Viewport for rendering z-buffer from light
		m_smViewport.TopLeftX = 0.0f;
		m_smViewport.TopLeftY = 0.0f;
		m_smViewport.Width = static_cast<float>(m_shadowMap.m_mapWidth);
		m_smViewport.Height = static_cast<float>(m_shadowMap.m_mapHeight);
		m_smViewport.MinDepth = 0.0f;
		m_smViewport.MaxDepth = 1.0f;

		AssetPath shaderPath = AssetPath(L"Shaders/ShadowMapPass/ShadowMapVS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::VERTEX_SHADER;
		SE_G::Bind::VertexShader::FillStandartInputLayout(shaderPath.m_params.asShader.numInputElements,
			shaderPath.m_params.asShader.IALayoutInputElements);
		auto& rm = ResourceManagerFacade::Instance();
		ResourceHandle vshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::VertexShader* vshaderRes = rm.Get<SE_G::Bind::VertexShader>(vshaderHandle);
		vertexShader = eastl::shared_ptr<SE_G::Bind::VertexShader>(
			vshaderRes,
			[](SE_G::Bind::VertexShader*) {}
		);
		delete[] shaderPath.m_params.asShader.IALayoutInputElements;
		// vertexShader = eastl::make_unique<Bind::VertexShader>(device,
		// 	MakeEngineAssetPath_Wstring(L"Shaders/ShadowMapPass/ShadowMapVS.hlsl").c_str());
		//AddPerFrameBind(vertexShader);

		// Buffer for transformation respect to light camera

		// Transform NDC space [-1,+1]^2 to texture space [0,1]^2 (for XY; Z stays the same)
		DirectX::XMMATRIX T = {
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f
		};

		for (size_t i = 0; i < 4; i++)
		{
			m_lightViewCamera->SetNearZ(cascadeBounds[i] - (i > 0 ? frustumBias : 0));
			m_lightViewCamera->SetFarZ(cascadeBounds[i + 1]);

			m_cascadesData.cascades[i] = {
				m_lightViewCamera->GetViewMatrix(),
				m_lightViewCamera->GetProjectionMatrix(),
				DirectX::XMMatrixIdentity()
			};

			m_cascadesData.cascades[i].shadowTransformFull =
				m_cascadesData.cascades[i].lightView
				* m_cascadesData.cascades[i].lightProjection
				* T;

		}
		m_cascadesData.distances.x = cascadeBounds[1];
		m_cascadesData.distances.y = cascadeBounds[2];
		m_cascadesData.distances.z = cascadeBounds[3];
		m_cascadesData.distances.w = cascadeBounds[4];

		m_shadowTransformsConstantBuffer = eastl::make_unique<Bind::VertexConstantBuffer<ShadowTransformData>>(
			device, m_cascadesData.cascades[0], 1u);

		// Rasterizer for depth bias to get rid of self-shadowing
		D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.DepthBias = DEPTH_BIAS_D24_UNORM(0.001f);
		rastDesc.DepthBiasClamp = 0.0f;
		rastDesc.SlopeScaledDepthBias = 3.0f;

		m_shadowMapRasterizer = eastl::make_unique<Bind::Rasterizer>(device, rastDesc);
		

		// Directional Stuff bindables (textures, samplers)

		// --- Shadow stuff ---

		// View texture as Shader resource while using it for shadowing in pixel shader
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = 1;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.ArraySize = 4;

		m_shadowMapTexture = eastl::make_unique<Bind::Texture>(device, m_shadowMap.GetTexture(), srvDesc, 4u);

		// Sampler of texture. It samples values from texture
		D3D11_SAMPLER_DESC shadowSamplerDesc;
		shadowSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		shadowSamplerDesc.MipLODBias = 0.0f;
		shadowSamplerDesc.MaxAnisotropy = 1;
		shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		shadowSamplerDesc.BorderColor[0] = 1.0;
		shadowSamplerDesc.BorderColor[1] = 1.0;
		shadowSamplerDesc.BorderColor[2] = 1.0;
		shadowSamplerDesc.BorderColor[3] = 1.0;
		shadowSamplerDesc.MinLOD = 0;
		shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_shadowSampler_1 = eastl::make_unique<Bind::Sampler>(device, shadowSamplerDesc, 1u);

		shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.MipLODBias = 0.0f;
		shadowSamplerDesc.MaxAnisotropy = 1;
		shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		shadowSamplerDesc.BorderColor[0] = 1.0;
		shadowSamplerDesc.BorderColor[1] = 1.0;
		shadowSamplerDesc.BorderColor[2] = 1.0;
		shadowSamplerDesc.BorderColor[3] = 1.0;
		shadowSamplerDesc.MinLOD = 0;
		shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_shadowSampler_2 = eastl::make_unique<Bind::Sampler>(device, shadowSamplerDesc, 2u);

		m_cascadesConstantBuffer = eastl::make_unique<Bind::PixelConstantBuffer<ShadowMapPass::CascadesData>>(
			device, m_cascadesData, 3u);

		m_mapSizePCB = eastl::make_unique<Bind::PixelConstantBuffer<MapSizePCB>>(
			device, MapSizePCB{ DXSM::Vector2{ m_shadowMap.m_mapWidth * 1.0f, m_shadowMap.m_mapHeight * 1.0f } }, 4u);

		//InitFrustumStuff(device, context);
	}

	ShadowMapPass::~ShadowMapPass()
	{
		
	}

	void ShadowMapPass::StartFrame()
	{
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"ShadowMap Pass");
	}

	void ShadowMapPass::Pass()
	{
		auto context = m_renderer->GetDeviceContext();
		vertexShader->Bind(GetDeviceContext());
		context->PSSetShader(nullptr, nullptr, 0u);
		context->PSSetShaderResources(0u, 0u, nullptr);
		BindAllPerFrame();


		DXSM::Vector3 direction = DXSM::Vector3(
			cos(m_lightData->Direction.y) * cos(m_lightData->Direction.x),
			sin(m_lightData->Direction.y),
			cos(m_lightData->Direction.y) * sin(m_lightData->Direction.x)
		);
		float eps = 0.01f;
		if (abs(direction.z) < 1.0f - eps)
		{
			DXSM::Vector3 _help = direction.Cross(DXSM::Vector3::Up);
			m_lightViewCamera->SetUp(_help.Cross(direction));
		}
		else
		{
			DXSM::Vector3 _help = direction.Cross(DXSM::Vector3::Backward);
			m_lightViewCamera->SetUp(_help.Cross(direction));
		}

		for (currCascade = 0; currCascade < 4; currCascade++)
		{
			context->OMSetRenderTargets(0, NULL, NULL);
			context->RSSetViewports(1, &m_smViewport);

			// Set null render target because we are only going to draw
			// to depth buffer. Setting a null render target will disable
			// color writes.
			ID3D11RenderTargetView* rtvDepth[1] = { 0 };

			context->OMSetRenderTargets(1, rtvDepth, m_shadowMap.m_depthDSV[currCascade].Get());
			context->ClearDepthStencilView(m_shadowMap.m_depthDSV[currCascade].Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			// Get data for current cascade
			GenerateBoundingFrustum(currCascade);
			MapCurrentCascadeData();
			m_shadowTransformsConstantBuffer->Bind(GetDeviceContext());
			m_shadowMapRasterizer->Bind(GetDeviceContext());

			for (auto& tech : m_gPass->m_techniques) {
				if (!tech.second->IsEnabled())
					continue;

				if (tech.second->GetTechniqueTag() == "GPass")
					tech.second->m_assignedTransform->EnableMeshTransformMode();

				tech.second->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
				// tech.second->BindAll(GetDeviceContext());

				if (tech.second->GetTechniqueTag() == "GPass")
				{
					auto gTech = static_cast<GPassTechnique*>(tech.second.get());
					gTech->m_meshData->m_mesh->Bind(context);
				}

				tech.second->DrawTechnique(GetDeviceContext());

				if (tech.second->GetTechniqueTag() == "GPass")
					tech.second->m_assignedTransform->DisableMeshTransformMode();
			}

		}
	}

	void ShadowMapPass::EndFrame()
	{
		auto context = m_renderer->GetDeviceContext();
		m_cascadesConstantBuffer->Update(context, m_cascadesData);

		ID3D11RenderTargetView* nullRTVs[] = { nullptr };
		ID3D11DepthStencilView* nullDSVs[] = { nullptr };
		context->OMSetRenderTargets(1, nullRTVs, *nullDSVs);

		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
	}

	SE_G::Camera* ShadowMapPass::GetFrustumCamera()
	{
		return m_lightViewCamera.get();
	}

	void ShadowMapPass::GetFrustumBoundsZ(int i, float* nearZ, float* farZ)
	{
		*nearZ = cascadeBounds[i];
		*farZ = cascadeBounds[i + 1];
	}

	ShadowMapPass::ShadowTransformData ShadowMapPass::GenerateBoundingFrustum(UINT cascadeNum)
	{
		// дл€ каждого фруструма у камеры мен€ютс€ следующие настройи:
		// nearestZ - у всех одинаковый (например 0.01)
		// SetViewWidth - вычисл€етс€ по значени€м точек (находим right вектор как вект произв Up X Direction)
		// SetViewHeight - вычисл€етс€ по значени€м точек
		// SetPosition - выбираем на уровне вершины фрустума, котора€ располагаетс€ "выше" (в противоположную сторону -)
		// всех остальных вдоль оси направлени€ света, затем отмер€ем назад значение nearZ
		// и выбираем ровно посередине исход€ из значений исполььзованных при расчЄте SetViewWidth и SetViewHeight
		// farestZ - макс рассто€ние от SetPosition до вершин фрустума вдоль направление света
		m_playerCamera = m_gPass->GetCamera();

		float playerNearZ = m_playerCamera->GetNearZ();
		float playerFarZ = m_playerCamera->GetFarZ();

		// ѕолучаем крайние точки подфрустумов игрока
		m_playerCamera->SetNearZ(cascadeBounds[cascadeNum] - (cascadeNum > 0 ? frustumBias : 0));
		m_playerCamera->SetFarZ(cascadeBounds[cascadeNum + 1]);

		auto _fpsWorld = FillFrustumPoints(m_playerCamera);

		m_playerCamera->SetNearZ(playerNearZ);
		m_playerCamera->SetFarZ(playerFarZ);

		ShadowMapPass::FrustumPoints _fpsLightView;
		const DXSM::Matrix lightViewMat = m_lightViewCamera->GetViewMatrix();
		const DXSM::Matrix lightViewMatInv = lightViewMat.Invert();

		for (size_t i = 0; i < 8; i++)
		{
			_fpsLightView.corners[i] = DXSM::Vector4::Transform(_fpsWorld.corners[i], lightViewMat);
			_fpsLightView.corners[i] = _fpsLightView.corners[i] / _fpsLightView.corners[i].w;
		}
		float x_min = _fpsLightView.corners[0].x; float x_max = _fpsLightView.corners[0].x;
		float y_min = _fpsLightView.corners[0].y; float y_max = _fpsLightView.corners[0].y;
		float z_min = _fpsLightView.corners[0].z; float z_max = _fpsLightView.corners[0].z;
		for (size_t i = 1; i < 8; i++)
		{
			x_min = eastl::min(_fpsLightView.corners[i].x, x_min);
			x_max = eastl::max(_fpsLightView.corners[i].x, x_max);
			y_min = eastl::min(_fpsLightView.corners[i].y, y_min);
			y_max = eastl::max(_fpsLightView.corners[i].y, y_max);
			z_min = eastl::min(_fpsLightView.corners[i].z, z_min);
			z_max = eastl::max(_fpsLightView.corners[i].z, z_max);
		}
		float newWidth = x_max - x_min;
		float newHeight = y_max - y_min;
		float newDeltaZ = z_max - z_min;
		float _nearZ = 0.01;
		
		DXSM::Vector3 newCamPos_oldViewSpace((x_min + x_max) * 0.5f, (y_min + y_max) * 0.5f, z_min - _nearZ);
		DXSM::Vector3 newCamPos_worldSpace = DXSM::Vector3::Transform(newCamPos_oldViewSpace, lightViewMatInv);

		m_lightViewCamera->SetPosition(newCamPos_worldSpace);

		DXSM::Vector3 direction = DXSM::Vector3(
			cos(m_lightData->Direction.y) * cos(m_lightData->Direction.x),
			sin(m_lightData->Direction.y),
			cos(m_lightData->Direction.y) * sin(m_lightData->Direction.x)
		);

		m_lightViewCamera->SetTarget(newCamPos_worldSpace + direction);
		m_lightViewCamera->SetNearZ(_nearZ);
		m_lightViewCamera->SetFarZ(newDeltaZ + _nearZ);
		m_lightViewCamera->SetViewWidth(newWidth);
		m_lightViewCamera->SetViewHeight(newHeight);

		/*
		printf("\n");
		printf("      x----- (%6.2f, %6.2f, %6.2f) [%6.2f] -----x\n", highestPoint.x, highestPoint.y, highestPoint.z, highest);
		printf("      |                                             |\n");
		printf("(%6.2f, %6.2f, %6.2f) [%6.2f]              (%6.2f, %6.2f, %6.2f) [%6.2f]\n", farLeftPoint.x, farLeftPoint.y, farLeftPoint.z, farLeft, farRightPoint.x, farRightPoint.y, farRightPoint.z, farRight);
		printf("      |                                             |\n");
		printf("      x----- (%6.2f, %6.2f, %6.2f) [%6.2f] -----x\n", lowestPoint.x, lowestPoint.y, lowestPoint.z, lowest);
		printf("\n");
		*/

		{
			DirectX::XMMATRIX T = {
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f
			};

			m_cascadesData.cascades[cascadeNum] = {
				m_lightViewCamera->GetViewMatrix(),
				m_lightViewCamera->GetProjectionMatrix(),
				DirectX::XMMatrixIdentity()
			};

			m_cascadesData.cascades[cascadeNum].shadowTransformFull =
				m_cascadesData.cascades[cascadeNum].lightView
				* m_cascadesData.cascades[cascadeNum].lightProjection
				* T;
		}
		// fps[cascadeNum] = FillFrustumPoints(m_lightViewCamera.get());
		// fps[cascadeNum].idx = currCascade;
		return m_cascadesData.cascades[cascadeNum];
	}

	void ShadowMapPass::MapCurrentCascadeData()
	{
		m_shadowTransformsConstantBuffer->Update(GetDeviceContext(), m_cascadesData.cascades[currCascade]);
	}

	void ShadowMapPass::BindForLightingPass()
	{
		m_shadowMapTexture->Bind(GetDeviceContext());
		m_shadowSampler_1->Bind(GetDeviceContext());
		m_shadowSampler_2->Bind(GetDeviceContext());
		m_cascadesConstantBuffer->Bind(GetDeviceContext());
		m_mapSizePCB->Bind(GetDeviceContext());
	}

	ShadowMapPass::FrustumPoints ShadowMapPass::FillFrustumPoints(Camera* camera)
	{
		FrustumPoints frustPoints;
		const DXSM::Matrix viewProj = camera->GetViewMatrix() * camera->GetProjectionMatrix();
		const DXSM::Matrix inv = viewProj.Invert();
		UINT currCorner = 0;
		for (unsigned int x = 0; x < 2; ++x) {
			for (unsigned int y = 0; y < 2; ++y) {
				for (unsigned int z = 0; z < 2; ++z) {
					DXSM::Vector4 pt = DXSM::Vector4::Transform(
						DXSM::Vector4(
							2.0f * x - 1.0f, // -1, 1
							2.0f * y - 1.0f, // -1, 1
							z,               //  0, 1
							1.0f), inv);
					pt = pt / pt.w;
					frustPoints.corners[currCorner++] = pt;
				}
			}
		}
		return frustPoints;
	}
}