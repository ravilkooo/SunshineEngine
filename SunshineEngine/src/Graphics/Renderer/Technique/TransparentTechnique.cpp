#include <d3d11.h>

#include <Graphics/Renderer/Technique/TransparentTechnique.h>
#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <Graphics/Bindable/Sampler.h>
#include <Graphics/Bindable/DepthStencilState.h>
#include <Graphics/Bindable/Rasterizer.h>
#include <Graphics/Bindable/ConstantBuffer.h>

#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/GraphicsResources/Mesh.h>

#include <Component/MeshComponent.h>

#include <ResourceManager/ResourceManagerFacade.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <Utils/StringUtils.h>


namespace SE_G {
	eastl::shared_ptr<Bind::PixelShader> TransparentTechnique::s_defaultShader;
	eastl::shared_ptr<Bind::PixelShader> TransparentTechnique::s_hiddenEditorShader;
	bool TransparentTechnique::s_staticDataInitializated = false;

	TransparentTechnique::TransparentTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
		SE::UUID uuid)
		: RenderTechnique(device, assignedTransform, technique)
	{
		if (!s_staticDataInitializated) {
			TransparentTechnique::InitStaticData(device);
		}

		m_device = device;
		m_objectUUID = uuid;
		
		D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rastDesc.CullMode = D3D11_CULL_BACK;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		m_rasterizer = eastl::make_unique<Bind::Rasterizer>(device, rastDesc);

		m_uuidBuffer = eastl::make_unique<Bind::PixelConstantBuffer<SE::UUIDhilo>>(
			device,
			uuid.GetHilo(),
			0u
		);

		AssetPath shaderPath = AssetPath(L"Shaders/TransparentPass/TransparentShaderVS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::VERTEX_SHADER;
		SE_G::Bind::VertexShader::FillStandartInputLayout(shaderPath.m_params.asShader.numInputElements,
			shaderPath.m_params.asShader.IALayoutInputElements);
		auto& rm = ResourceManagerFacade::Instance();
		ResourceHandle vshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::VertexShader* vshaderRes = rm.Get<SE_G::Bind::VertexShader>(vshaderHandle);
		m_vertexShader = eastl::shared_ptr<SE_G::Bind::VertexShader>(
			vshaderRes,
			[](SE_G::Bind::VertexShader*) {}
		);
		delete[] shaderPath.m_params.asShader.IALayoutInputElements;

		auto ap = AssetPath(
			SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
			AssetPath::AssetSource::Engine);
		ResourceHandle texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
		SE_G::Bind::Texture* texture =
			ResourceManagerFacade::Instance().Get<SE_G::Bind::Texture>(texHandle);

		m_texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texture,
			[](SE_G::Bind::Texture*) {}
		);
		m_texture->m_texturePath = texture->m_texturePath;

		m_textureSampler = eastl::make_unique<SE_G::Bind::Sampler>(
			device,
			SE_G::Bind::SamplerPreset::Wrap);
	}

	TransparentTechnique::~TransparentTechnique()
	{
	}

	TransparentTechnique::TransparentTechnique(GPassTechnique* gPassTech)
		: TransparentTechnique(gPassTech->m_device, gPassTech->m_assignedTransform, "Transparent", gPassTech->m_objectUUID)
	{
		// "Cast" GPassTechnique to TransparentTechnique
		InitByMeshData(gPassTech->m_meshData);
	}

	TransparentTechnique::TransparentTechnique(TransparentTechnique&& other) noexcept
		: RenderTechnique(eastl::move(other)),
		m_uuidBuffer(eastl::move(other.m_uuidBuffer)),
		m_device(other.m_device)
	{
		other.m_device = nullptr;
	}

	TransparentTechnique& TransparentTechnique::operator=(TransparentTechnique&& other) noexcept
	{
		if (this != &other) {
			RenderTechnique::operator=(eastl::move(other));
			m_uuidBuffer = eastl::move(other.m_uuidBuffer);
			m_device = other.m_device;
			other.m_device = nullptr;
		}
		return *this;
	}

	void TransparentTechnique::SetRasterizer(D3D11_RASTERIZER_DESC rastDesc)
	{
		m_rasterizer = eastl::make_unique<Bind::Rasterizer>(m_device, rastDesc);
	}

	void TransparentTechnique::BindAll(ID3D11DeviceContext* context)
	{
		RenderTechnique::BindAll(context);
		if (m_isHiddenInEditor)
			s_hiddenEditorShader->Bind(context);
		else
			s_defaultShader->Bind(context);

		m_uuidBuffer->Bind(context);
		m_meshData->m_mesh->Bind(context);
		m_meshData->m_texture->Bind(context, 0u);
		m_meshData->m_textureSampler->Bind(context);
	}

	void TransparentTechnique::DrawTechnique(ID3D11DeviceContext* context)
	{
		m_meshData->m_mesh->Draw(context);
	}

	void TransparentTechnique::InitByMeshData(eastl::shared_ptr<MeshData> meshData)
	{
		m_meshData = meshData;
	}

	void TransparentTechnique::InitStaticData(ID3D11Device* device)
	{
		AssetPath shaderPath = AssetPath(L"Shaders/TransparentPass/TransparentTextureShaderPS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::PIXEL_SHADER;
		auto& rm = ResourceManagerFacade::Instance();
		ResourceHandle pshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::PixelShader* pshaderRes = rm.Get<SE_G::Bind::PixelShader>(pshaderHandle);
		s_defaultShader = eastl::shared_ptr<SE_G::Bind::PixelShader>(
			pshaderRes,
			[](SE_G::Bind::PixelShader*) {}
		);

		shaderPath.m_assetRelativePath = L"Shaders/TransparentPass/TransparentTextureShaderHiddenEditorPS.hlsl";
		ResourceHandle hiddenPshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::PixelShader* hiddenPshaderRes = rm.Get<SE_G::Bind::PixelShader>(hiddenPshaderHandle);
		s_hiddenEditorShader = eastl::shared_ptr<SE_G::Bind::PixelShader>(
			hiddenPshaderRes,
			[](SE_G::Bind::PixelShader*) {}
		);

		s_staticDataInitializated = true;
	}
}