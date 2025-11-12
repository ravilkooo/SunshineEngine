#pragma once

// ThirdPartyLibs
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include <Graphics/Renderer/DeferredRenderer.h>

#include <GameObject.h>
#include <GameTimer.h>

#include <Graphics/Renderer/Pass/GPass.h>
#include <Graphics/Renderer/Pass/LightPass.h>
#include <Graphics/Renderer/Pass/SelectionPass.h>
#include <Graphics/Renderer/Pass/IconPass.h>

#include <EditorObjectFactory.h>
#include <Scripting/LuaManager.h>

#include <Physics/PhysicsSystem.h>


class WorldEditor
{
public:

    class PixelUUIDHandler {
    public:

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_clickMouseBuffer;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_UUIDOnMouseClickShader;

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_outputUUIDBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_outputUUIDBufferStaged;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_outputUUIDUAV;

        PixelUUIDHandler() {
        }

        void Init(ID3D11Device* device) {
            D3D11_BUFFER_DESC outputUUIDBufferDesc;
            ZeroMemory(&outputUUIDBufferDesc, sizeof(outputUUIDBufferDesc));
            outputUUIDBufferDesc.Usage = D3D11_USAGE_DEFAULT;
            outputUUIDBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
            outputUUIDBufferDesc.ByteWidth = 4 * sizeof(UINT);
            //outputUUIDBufferDesc.MiscFlags = D3D1_RESOURCE_MISC_;
            device->CreateBuffer(&outputUUIDBufferDesc, nullptr,
                m_outputUUIDBuffer.GetAddressOf());

            outputUUIDBufferDesc.Usage = D3D11_USAGE_STAGING;
            outputUUIDBufferDesc.BindFlags = 0;
            outputUUIDBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            device->CreateBuffer(&outputUUIDBufferDesc, 0,
                m_outputUUIDBufferStaged.GetAddressOf());

            D3D11_UNORDERED_ACCESS_VIEW_DESC outputUUIDUAVDesc;
            outputUUIDUAVDesc.Format = DXGI_FORMAT_R32_UINT;
            outputUUIDUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            outputUUIDUAVDesc.Buffer.FirstElement = 0;
            outputUUIDUAVDesc.Buffer.NumElements = 4;
            outputUUIDUAVDesc.Buffer.Flags = 0;
            device->CreateUnorderedAccessView(m_outputUUIDBuffer.Get(), &outputUUIDUAVDesc, m_outputUUIDUAV.GetAddressOf());

            CD3D11_BUFFER_DESC clickBufferDesc(2*sizeof(uint64_t), D3D11_BIND_CONSTANT_BUFFER);
            HRESULT hr = device->CreateBuffer(&clickBufferDesc, nullptr, &m_clickMouseBuffer);
            if (FAILED(hr)) {
                printf("error!!!\n");
            }

            Microsoft::WRL::ComPtr<ID3DBlob> cs_blob;
            D3DCompileFromFile(JoinWchar_Wchar(EDITOR_ASSETS_DIR, L"Shaders/UUIDOnMouseClickCS.hlsl"),
                nullptr,
                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                "main", "cs_5_0",
                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                &cs_blob, nullptr);
            device->CreateComputeShader(
                cs_blob->GetBufferPointer(),
                cs_blob->GetBufferSize(),
                nullptr,
                m_UUIDOnMouseClickShader.GetAddressOf()
            );
            return;
        }

        SE::UUID GetUUID(ID3D11DeviceContext* context,
            ID3D11ShaderResourceView* UUIDTextureView,
            UINT mouseClickX, UINT mouseClickY) {
            
            context->CSSetShaderResources(0u, 1u, &UUIDTextureView);

            UINT clickPos[2] = { mouseClickX, mouseClickY };
            context->UpdateSubresource(m_clickMouseBuffer.Get(), 0, nullptr, &clickPos, 0, 0);
            context->CSSetConstantBuffers(0, 1, m_clickMouseBuffer.GetAddressOf());

            UINT initCount[] = { (UINT)-1 };
            context->CSSetUnorderedAccessViews(0, 1, m_outputUUIDUAV.GetAddressOf(), initCount);

            context->CSSetShader(m_UUIDOnMouseClickShader.Get(), nullptr, 0);
            context->Dispatch(1, 1, 1);
            context->CSSetShader(nullptr, nullptr, 0);
            ID3D11UnorderedAccessView* uavs[] = { nullptr };
            context->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);
            ID3D11ShaderResourceView* nullSRVs[] = { nullptr };
            context->CSSetShaderResources(0, 1, nullSRVs);

            context->CopyResource(m_outputUUIDBufferStaged.Get(), m_outputUUIDBuffer.Get());
            D3D11_MAPPED_SUBRESOURCE mappedData;
            context->Map(m_outputUUIDBufferStaged.Get(), 0, D3D11_MAP_READ, 0, &mappedData);

            uint32_t* hilo = reinterpret_cast<uint32_t*>(mappedData.pData);
            uint64_t uuid = (uint64_t)hilo[0] << 32 | hilo[1];

            context->Unmap(m_outputUUIDBufferStaged.Get(), 0);

            return SE::UUID(uuid);
            
            //return SE::UUID(0u);
        }

    };

    PixelUUIDHandler* m_pixelUUIDHandler;

    WorldEditor();
    ~WorldEditor();

    void InitWorldEditor(
        eastl::shared_ptr<SE_G::DeferredRenderer> renderer,
        UINT screenWidth = 800u,
        UINT screenHeight = 600u);
    void Run();

    void Update(float deltaTime);
    //void SyncronizeTransforms();
    void Render();
    void ClearScene();

    GameTimer m_timer;
    eastl::shared_ptr<Scene_Info> m_scene;
    eastl::shared_ptr<SE_G::DeferredRenderer> m_renderer;
    LuaManager m_luaManager;

    void OnResize(UINT resizeWidth, UINT resizeHeight);

    // Change to (Index + generation handle (robust for inserts/erases))
    // eastl::shared_ptr<GameObject> m_acticeGameObject;

    UINT m_screenWidth = 800u;
    UINT m_screenHeight = 800u;

    eastl::shared_ptr<SE_G::GPass> m_gPass;
    eastl::shared_ptr<SE_G::LightPass> m_lightPass;
    eastl::shared_ptr<SE_G::SelectionPass> m_selectionPass;
    eastl::shared_ptr<SE_G::IconPass> m_iconPass;

    float m_deltaTime = 0.0f;


    // track ray from mouse click
    DXSM::Vector4 rayDirection;

    void DeprojectScreenToWorld(DXSM::Vector2 mouseScreenCoords, DXSM::Vector2 lastGameViewportSize);

    SE::UUID ChooseObjectByClick(UINT x, UINT y);

private:
    //eastl::shared_ptr<PhysicsSystem> m_physicsSystem;
    // testing
    // SE::UUID floorId;
    // SE::UUID ballId;
};
