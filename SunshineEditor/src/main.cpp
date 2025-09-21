#include "DeferredGame.h"
#include <Jolt/Jolt.h>
#include <VGJS.h>
#include <assimp/Importer.hpp>
#include <EASTL/allocator.h>
#include <imgui.h>
#include <lua.hpp>

#include "fmod.hpp"
#include "common.h"

int imgui_test();
int fmod_test();
int lua_test();
int game_test();

int main() {
    game_test();
}

int game_test() {
    //imgui_test();
    DeferredGame game = DeferredGame();
    game.Run();

    return 0;
}

int fmod_test() {
    FMOD_Main();

    return 0;
}

// Main code
int imgui_test()
{
    // 1. Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProcImGui, 0, 0,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"ImGuiTestWindow", NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, L"ImGui Test", WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);

    // 2. Initialize DirectX 11
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    DXGI_SWAP_CHAIN_DESC scDesc = {};
    scDesc.BufferCount = 1;
    scDesc.BufferDesc.Width = 800;
    scDesc.BufferDesc.Height = 600;
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.OutputWindow = hwnd;
    scDesc.SampleDesc.Count = 1;
    scDesc.Windowed = TRUE;
    IDXGISwapChain* swapChain = nullptr;

    D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        0, nullptr, 0, D3D11_SDK_VERSION,
        &scDesc, &swapChain, &device, &featureLevel, &context);

    // 3. Create render target
    ID3D11RenderTargetView* rtv = nullptr;
    ID3D11Texture2D* backBuffer = nullptr;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    device->CreateRenderTargetView(backBuffer, nullptr, &rtv);
    backBuffer->Release();

    // 4. Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);

    // Show window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // 5. Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Example window
        ImGui::Begin("Hello, ImGui!");
        ImGui::Text("This is a simple test.");
        ImGui::SliderFloat("Float value", &io.DeltaTime, 0.0f, 1.0f);
        if (ImGui::Button("Close"))
            PostQuitMessage(0);
        ImGui::End();

        // Rendering
        ImGui::Render();
        context->OMSetRenderTargets(1, &rtv, nullptr);
        const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        context->ClearRenderTargetView(rtv, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swapChain->Present(1, 0);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    rtv->Release();
    swapChain->Release();
    context->Release();
    device->Release();

    return 0;
}

int lua_test() {
    lua_State* L = luaL_newstate();    // create new Lua state
    luaL_openlibs(L);                  // open standard libraries

    // run a Lua script file
    std::wstring script = ENGINE_ASSETS_DIR;
    script = script + L"Scripts/test.lua";

    if (luaL_dofile(L, std::string(script.begin(), script.end()).c_str()) != LUA_OK) {
        std::cerr << "Error running Lua script: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1); // remove error message
    }

    // call a Lua function from C++
    lua_getglobal(L, "add");  // push function onto stack
    lua_pushnumber(L, 5);     // push argument 1
    lua_pushnumber(L, 7);     // push argument 2

    if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
        std::cerr << "Error calling add(): " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
    else {
        double sum = lua_tonumber(L, -1);
        std::cout << "5 + 7 = " << sum << std::endl;
        lua_pop(L, 1); // remove return value
    }

    lua_close(L); // cleanup

	DeferredGame game = DeferredGame();
	game.Run();

	return 0;
}
