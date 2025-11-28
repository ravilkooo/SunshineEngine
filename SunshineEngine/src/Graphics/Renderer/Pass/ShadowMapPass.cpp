#include <Graphics/Renderer/Pass/ShadowMapPass.h>
#include <Utils/StringUtils.h>

namespace SE_G {

	ShadowMapPass::ShadowMapPass(ID3D11Device* device, ID3D11DeviceContext* context,
		GPass* gPass,
		eastl::shared_ptr<SE_G::DirectionalLightData> lightData,
		UINT smSizeX, UINT smSizeY)
		:
		RenderPass("ShadowMapPass", device, context),
		m_shadowMap(ShadowMap(device, smSizeX, smSizeY)),
		m_gPass(gPass),
		m_lightData(lightData)
	{
		m_passType = PassType::Shadow;

		m_playerCamera = gPass->camera.get();

		m_lightViewCamera = eastl::make_unique<SE_G::Camera>(device,
			m_shadowMap.m_mapWidth / m_shadowMap.m_mapHeight);
		m_lightViewCamera->SetPosition(lightData->Position);
		m_lightViewCamera->SetTarget(lightData->Position + lightData->Direction);
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


		vertexShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/ShadowMapPass/ShadowMapVS.hlsl").c_str());
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
		rastDesc.DepthBias = 1000000;
		rastDesc.DepthBiasClamp = 0.0f;
		rastDesc.SlopeScaledDepthBias = 2.0f;

		AddPerFrameBind(new Bind::Rasterizer(device, rastDesc));
		

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
	}

	ShadowMapPass::~ShadowMapPass()
	{
		
	}

	void ShadowMapPass::StartFrame()
	{
	}

	void ShadowMapPass::Pass()
	{
		vertexShader->Bind(GetDeviceContext());
		context->PSSetShader(nullptr, nullptr, 0u);
		context->PSSetShaderResources(0u, 0u, nullptr);
		BindAllPerFrame();

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

			for (auto& tech : m_gPass->m_techniques) {
				tech->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
				// tech->BindAll(GetDeviceContext());

				if (tech->m_mesh)
					tech->m_mesh->Bind(context.Get());


				tech->DrawTechnique(GetDeviceContext());
			}

		}
	}

	void ShadowMapPass::EndFrame()
	{
		m_cascadesConstantBuffer->Update(context.Get(), m_cascadesData);

		ID3D11RenderTargetView* nullRTVs[] = { nullptr };
		ID3D11DepthStencilView* nullDSVs[] = { nullptr };
		context->OMSetRenderTargets(1, nullRTVs, *nullDSVs);
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
		// SetPosition - выбираем на уровне вершины фрустума, котора€ располагаетс€ "выше" (в противоположную сторону -) всех остальных вдоль оси направлени€ света, затем отмер€ем назад значение nearZ
		// и выбираем ровно посередине исход€ из значений исполььзованных при расчЄте SetViewWidth и SetViewHeight
		// farestZ - макс рассто€ние от SetPosition до вершин фрустума вдоль направление света

		float playerNearZ = m_playerCamera->GetNearZ();
		float playerFarZ = m_playerCamera->GetFarZ();

		// ѕолучаем крайние точки подфрустумов игрока
		m_playerCamera->SetNearZ(cascadeBounds[cascadeNum] - (cascadeNum > 0 ? frustumBias : 0));
		m_playerCamera->SetFarZ(cascadeBounds[cascadeNum + 1] + frustumBias);

		DXSM::Matrix playerViewMat = m_playerCamera->GetViewMatrix();
		DXSM::Matrix playerProjMat = m_playerCamera->GetProjectionMatrix();

		m_playerCamera->SetNearZ(playerNearZ);
		m_playerCamera->SetFarZ(playerFarZ);

		DXSM::Matrix viewInverse = playerViewMat.Invert();
		DXSM::Matrix projInverse = playerProjMat.Invert();

		DXSM::Vector3 farLeftPoint, farRightPoint, highestPoint, lowestPoint, nearZPoint, farZPoint;
		float farLeft, farRight, highest, lowest, nearestZ, farestZ;
		{
			DXSM::Vector4 _v = { 0, 0, 0, 1 };
			_v = DXSM::Vector4::Transform(_v, projInverse);
			_v = DXSM::Vector4::Transform(_v, viewInverse);
			_v = _v / _v.w;
			DXSM::Vector3 _w = DXSM::Vector3(_v);
			farLeftPoint = _w; farRightPoint = _w; highestPoint = _w; lowestPoint = _w; nearZPoint = _w; farZPoint = _w;
		}

		DXSM::Vector3 zDir = DXSM::Vector3(m_lightData->Direction);
		zDir.Normalize();
		DXSM::Vector3 camUpDir = m_lightViewCamera->GetUp();
		camUpDir.Normalize();
		DXSM::Vector3 xDir = camUpDir.Cross(zDir);
		xDir.Normalize();

		float epsilon = 0.0001;
		if (xDir.LengthSquared() < epsilon)
		{
			m_lightViewCamera->SetUp({ 0.0f, 0.0f, 1.0f });
			camUpDir = m_lightViewCamera->GetUp();
			camUpDir.Normalize();
			xDir = camUpDir.Cross(zDir);
			xDir.Normalize();
		}

		DXSM::Vector3 yDir = zDir.Cross(xDir);
		yDir.Normalize();

		for (float i = -1; i < 2; i += 2)
		{
			for (float j = -1; j < 2; j += 2)
			{
				for (float k = 0; k < 2; k++)
				{
					DXSM::Vector4 _v = { i, j, k, 1 };
					_v = DXSM::Vector4::Transform(_v, projInverse);
					_v = DXSM::Vector4::Transform(_v, viewInverse);
					_v = _v / _v.w;
					if (farRightPoint.Dot(xDir) < DXSM::Vector3(_v).Dot(xDir)) { farRightPoint = DXSM::Vector3(_v); }
					else
						if (farLeftPoint.Dot(xDir) > DXSM::Vector3(_v).Dot(xDir)) { farLeftPoint = DXSM::Vector3(_v); }

					if (highestPoint.Dot(yDir) < DXSM::Vector3(_v).Dot(yDir)) { highestPoint = DXSM::Vector3(_v); }
					else
						if (lowestPoint.Dot(yDir) > DXSM::Vector3(_v).Dot(yDir)) { lowestPoint = DXSM::Vector3(_v); }

					if (farZPoint.Dot(zDir) < DXSM::Vector3(_v).Dot(zDir)) { farZPoint = DXSM::Vector3(_v); }
					else
						if (nearZPoint.Dot(zDir) > DXSM::Vector3(_v).Dot(zDir)) { nearZPoint = DXSM::Vector3(_v); }


					//std::cout << _v.x << ",\t" << _v.y << ",\t" << _v.z << "\n";
				}
			}
		}
		farRight = farRightPoint.Dot(xDir);
		farLeft = farLeftPoint.Dot(xDir);
		highest = highestPoint.Dot(yDir);
		lowest = lowestPoint.Dot(yDir);
		nearestZ = nearZPoint.Dot(zDir);
		farestZ = farZPoint.Dot(zDir);

		printf("\n");
		printf("      x----- (%6.2f, %6.2f, %6.2f) [%6.2f] -----x\n", highestPoint.x, highestPoint.y, highestPoint.z, highest);
		printf("      |                                             |\n");
		printf("(%6.2f, %6.2f, %6.2f) [%6.2f]              (%6.2f, %6.2f, %6.2f) [%6.2f]\n", farLeftPoint.x, farLeftPoint.y, farLeftPoint.z, farLeft, farRightPoint.x, farRightPoint.y, farRightPoint.z, farRight);
		printf("      |                                             |\n");
		printf("      x----- (%6.2f, %6.2f, %6.2f) [%6.2f] -----x\n", lowestPoint.x, lowestPoint.y, lowestPoint.z, lowest);
		printf("\n");

		float _nearZ = 0.01;

		DXSM::Vector3 newCamPos =
			(farRight + farLeft) * 0.5 * xDir
			+ (highest + lowest) * 0.5 * yDir
			+ (nearestZ - _nearZ) * zDir; // -0.01 так как камера немного отдалена от nearZ


		m_lightViewCamera->SetPosition(newCamPos);
		m_lightViewCamera->SetTarget(newCamPos + m_lightData->Direction);
		m_lightViewCamera->SetNearZ(_nearZ);
		m_lightViewCamera->SetFarZ(farestZ - nearestZ + _nearZ);
		m_lightViewCamera->SetViewWidth(farRight - farLeft);
		m_lightViewCamera->SetViewHeight(highest - lowest);

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
	}
}