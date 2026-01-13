#include <PlayerObject/PlayerObject.h>
#include <PlayerObject/PlayerController.h>

// Example 1: Basic initialization and key binding
void Example_BasicSetup(SE_G::DeferredRenderer* renderer, PlayerObject* player)
{

    // Initialize Lua system
    eastl::string scriptPath = "Projects/Templates/player_controller.lua";
    bool success = player->m_luaActionMapping.Initialize(scriptPath);
    
    if (!success) {
        // Handle error - check console for Lua errors
        return;
    }

    // Set player reference so Lua can access it
    player->m_luaActionMapping.SetPlayerObject(player);

    // Bind keys to Lua functions
    player->m_luaActionMapping.BindKey(Keys::W, "onMoveForward");
    player->m_luaActionMapping.BindKey(Keys::S, "onMoveBackward");
    player->m_luaActionMapping.BindKey(Keys::A, "onStrafeLeft");
    player->m_luaActionMapping.BindKey(Keys::D, "onStrafeRight");
    player->m_luaActionMapping.BindKey(Keys::Space, "onJump");
    player->m_luaActionMapping.BindKey(Keys::E, "onInteract");
    // player->m_luaActionMapping.BindKey(Keys::ControlKey, "onCrouch");
    // player->m_luaActionMapping.BindKey(Keys::ShiftKey, "onToggleSprint");

    // Enable Lua callback mode
    player->m_playerController.SetLuaCallbackMode(true);
}

// Example 2: Integration with your existing input system
void Example_InputIntegration(PlayerObject* player)
{
    // In your main loop or input handler:
    // When a key is pressed:
    Keys pressedKey = Keys::W; // From your input system
    player->m_playerController.HandleKeyDown(pressedKey);
    
    // The PlayerController will automatically:
    // 1. Check if Lua callback mode is enabled
    // 2. Look up the bound Lua function
    // 3. Execute it with "down" parameter
    // 4. Fall back to C++ handling if no Lua binding exists

    // When a key is released:
    player->m_playerController.HandleKeyUp(pressedKey);
}

// Example 3: Manual Lua function execution with custom parameters
void Example_ManualLuaCall(PlayerObject* player)
{
    // Execute Lua function by name with custom arguments
    player->m_luaActionMapping.ExecuteLuaFunction("onLookAround", 10.0f, 5.0f);
    
    // Call debug function
    player->m_luaActionMapping.ExecuteLuaFunction("debugPlayerState");

    // Pass delta time for update functions
    float deltaTime = 0.016f; // 60 FPS
    player->m_luaActionMapping.ExecuteLuaFunction("onUpdate", deltaTime);
}

// Example 4: Advanced - Direct Lua state access
void Example_AdvancedLuaAccess(PlayerObject* player)
{
    // Get direct access to Lua state for advanced operations
    sol::state* lua = player->m_luaActionMapping.GetLuaState();
    
    if (lua) {
        // Set global variables
        (*lua)["gameTimeSeconds"] = 120.5f;
        (*lua)["playerHealth"] = 100;

        // Call Lua functions with return values
        sol::protected_function func = (*lua)["getPlayerInfo"];
        if (func.valid()) {
            auto result = func();
            if (result.valid()) {
                std::string info = result;
                // Use the returned string
            }
        }

        // Create Lua tables from C++
        sol::table settings = lua->create_table();
        settings["volume"] = 0.8f;
        settings["difficulty"] = "hard";
        (*lua)["gameSettings"] = settings;
    }
}

// Example 5: Error handling and debugging
void Example_ErrorHandling(PlayerObject* player)
{
    // Check if a key is bound before using it
    if (player->m_luaActionMapping.IsKeyBound(Keys::W)) {
        auto funcName = player->m_luaActionMapping.GetBoundFunction(Keys::W);
        if (funcName.has_value()) {
            // Execute the bound function
            player->m_luaActionMapping.ExecuteKeyAction(Keys::W, "down");
        }
    }

    // Reload script at runtime (useful for hot-reloading during development)
    if (/* some condition, like F5 pressed */ false) {
        bool reloaded = player->m_luaActionMapping.ReloadScript();
        if (reloaded) {
            // Re-bind keys after reload
            player->m_luaActionMapping.BindKey(Keys::W, "onMoveForward");
            // ... etc
        }
    }

    // ExecuteKeyAction returns false if function doesn't exist or errors occur
    // Errors are logged to console automatically
    bool success = player->m_luaActionMapping.ExecuteKeyAction(Keys::G, "nonExistentFunction");
    if (!success) {
        // Fallback behavior
    }
}

// Example 6: Using string-based key binding (easier for config files)
void Example_StringBasedBinding(PlayerObject* player)
{
    // Bind using key names instead of Keys enum
    player->m_luaActionMapping.BindKeyByString("W", "onMoveForward");
    player->m_luaActionMapping.BindKeyByString("Space", "onJump");
    player->m_luaActionMapping.BindKeyByString("E", "onInteract");

    // This is useful when loading bindings from JSON/config files:
    /*
    json config = LoadConfig("keybindings.json");
    for (auto& [keyName, funcName] : config.items()) {
        player->m_luaActionMapping.BindKeyByString(
            keyName.get<eastl::string>(),
            funcName.get<eastl::string>()
        );
    }
    */
}

// Example 7: Complete initialization in PlayerObject constructor
class MyPlayerObject : public PlayerObject
{
public:
    MyPlayerObject(const json& j, SE_G::DeferredRenderer* renderSystem)
        : PlayerObject(j, renderSystem)
    {
        // Initialize Lua
        eastl::string scriptPath = "Projects/Templates/player_controller.lua";
        m_luaActionMapping.Initialize(scriptPath);
        m_luaActionMapping.SetPlayerObject(this);

        // Set up bindings
        SetupDefaultKeyBindings();

        // Enable Lua mode
        m_playerController.SetLuaCallbackMode(true);
    }

    void SetupDefaultKeyBindings()
    {
        m_luaActionMapping.BindKey(Keys::W, "onMoveForward");
        m_luaActionMapping.BindKey(Keys::S, "onMoveBackward");
        m_luaActionMapping.BindKey(Keys::A, "onStrafeLeft");
        m_luaActionMapping.BindKey(Keys::D, "onStrafeRight");
        m_luaActionMapping.BindKey(Keys::Space, "onJump");
        m_luaActionMapping.BindKey(Keys::E, "onInteract");
        // m_luaActionMapping.BindKey(Keys::ControlKey, "onCrouch");
        // m_luaActionMapping.BindKey(Keys::ShiftKey, "onToggleSprint");
    }
};

// Example 8: Hybrid approach - C++ and Lua together
void Example_HybridApproach(PlayerObject* player)
{

    // You can toggle between C++ and Lua at runtime
    bool useLua = true; // Could come from settings

    player->m_playerController.SetLuaCallbackMode(useLua);

    // Now input handling automatically switches:
    // - If useLua = true: tries Lua first, falls back to C++
    // - If useLua = false: uses only C++ handling

    // This gives you flexibility:
    // - Use Lua for rapid prototyping and tweaking
    // - Use C++ for performance-critical code
    // - Players can mod gameplay via Lua scripts
}
