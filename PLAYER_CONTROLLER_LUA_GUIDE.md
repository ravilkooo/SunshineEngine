# PlayerController Lua Integration Guide

## Overview
This system allows you to bind keyboard/mouse input to Lua functions, enabling flexible, script-driven player behavior without recompiling C++ code.

## Architecture

### Components
1. **PlayerLuaKeyActionsMapping**: Manages Lua state, key bindings, and function execution
2. **PlayerController**: Handles input and routes to Lua or C++ handlers
3. **Lua Script**: Contains callback functions for player actions

### Flow
```
Input Event → PlayerController → Lua Binding Lookup → Lua Function Execution
                     ↓ (if no Lua binding)
              C++ Fallback Handler
```

## Quick Start

### 1. Initialize Lua System
```cpp
PlayerObject player;

// Load Lua script
eastl::string scriptPath = "Projects/Templates/player_controller.lua";
player.m_luaActionMapping.Initialize(scriptPath);
player.m_luaActionMapping.SetPlayerObject(&player);
```

### 2. Bind Keys to Lua Functions
```cpp
player.m_luaActionMapping.BindKey(Keys::W, "onMoveForward");
player.m_luaActionMapping.BindKey(Keys::Space, "onJump");
player.m_luaActionMapping.BindKey(Keys::E, "onInteract");
```

### 3. Enable Lua Mode
```cpp
player.m_playerController.SetLuaCallbackMode(true);
```

### 4. Input Handling (Already Implemented)
```cpp
// Your existing input code works automatically:
player.m_playerController.HandleKeyDown(Keys::W);
player.m_playerController.HandleKeyUp(Keys::W);
```

## Lua Script Structure

### Basic Function Signature
```lua
function onMoveForward(action)
    -- action is "down" or "up"
    if not player then return end
    
    local transform = player:GetTransform()
    -- Modify player state...
end
```

### Available C++ Objects in Lua

#### PlayerObject
```lua
player:GetTransform()  -- Returns TransformComponent
player:GetCamera()     -- Returns Camera
player:GetName()       -- Returns string
```

#### TransformComponent
```lua
transform.position.x, .y, .z  -- Read/Write
transform.rotation.x, .y, .z  -- Read/Write
transform.scale.x, .y, .z     -- Read/Write
```

#### Camera
```lua
camera.forward.x, .y, .z  -- Read-only
camera.up.x, .y, .z       -- Read-only
camera.right.x, .y, .z    -- Read-only
camera.position.x, .y, .z -- Read-only
```

#### Vector3
```lua
local v = Vector3(1.0, 2.0, 3.0)
v:Normalize()
local len = v:Length()
```

## Integration Examples

### Example 1: Basic Setup in PlayerObject Constructor
```cpp
PlayerObject::PlayerObject(const json& j, SE_G::DeferredRenderer* renderSystem)
{
    // ... existing initialization ...

    // Initialize Lua
    m_luaActionMapping.Initialize("Projects/Templates/player_controller.lua");
    m_luaActionMapping.SetPlayerObject(this);

    // Bind keys
    m_luaActionMapping.BindKey(Keys::W, "onMoveForward");
    m_luaActionMapping.BindKey(Keys::S, "onMoveBackward");
    m_luaActionMapping.BindKey(Keys::A, "onStrafeLeft");
    m_luaActionMapping.BindKey(Keys::D, "onStrafeRight");
    m_luaActionMapping.BindKey(Keys::Space, "onJump");

    // Enable Lua callbacks
    m_playerController.SetLuaCallbackMode(true);
}
```

### Example 2: Input Polling Loop
```cpp
void Game::Update(float deltaTime)
{
    // Your existing input system
    if (InputDevice::instance->IsKeyDown(Keys::W)) {
        m_player->m_playerController.HandleKeyDown(Keys::W);
    }
    
    if (InputDevice::instance->IsKeyUp(Keys::W)) {
        m_player->m_playerController.HandleKeyUp(Keys::W);
    }

    // Update player (calls Lua if enabled)
    m_player->m_playerController.UpdatePlayer(deltaTime);
}
```

### Example 3: Manual Lua Function Calls
```cpp
// Call Lua function directly with custom parameters
m_player->m_luaActionMapping.ExecuteLuaFunction("onUpdate", deltaTime);
m_player->m_luaActionMapping.ExecuteLuaFunction("onLookAround", mouseX, mouseY);
```

## Error Handling

### Automatic Error Logging
All Lua errors are automatically logged to console:
```
[PlayerLuaKeyActionsMapping Error] Lua function not found: onMissingFunction
[PlayerLuaKeyActionsMapping Error] Lua execution error in onJump: attempt to index nil value
```

### Check Before Execution
```cpp
if (m_luaActionMapping.IsKeyBound(Keys::W)) {
    auto funcName = m_luaActionMapping.GetBoundFunction(Keys::W);
    if (funcName.has_value()) {
        // Safe to execute
        m_luaActionMapping.ExecuteKeyAction(Keys::W, "down");
    }
}
```

### Protected Function Calls
All Lua calls use `sol::protected_function` internally, so crashes are prevented.

## Advanced Features

### Hot-Reloading Scripts
```cpp
// During development, reload script without restarting
if (InputDevice::instance->IsKeyPressed(Keys::F5)) {
    m_player->m_luaActionMapping.ReloadScript();
    // Re-bind keys...
}
```

### Direct Lua State Access
```cpp
sol::state* lua = m_player->m_luaActionMapping.GetLuaState();
if (lua) {
    // Set global variables
    (*lua)["gameTime"] = currentTime;
    
    // Access Lua tables
    sol::table config = (*lua)["gameSettings"];
    float volume = config["volume"];
}
```

### String-Based Key Binding (Config Files)
```cpp
// Easier for JSON/config loading
m_luaActionMapping.BindKeyByString("W", "onMoveForward");
m_luaActionMapping.BindKeyByString("Space", "onJump");
```

### Hybrid Mode (C++ + Lua)
```cpp
// Toggle between Lua and C++ at runtime
m_playerController.SetLuaCallbackMode(useLuaScripts);

// When enabled:  Tries Lua first, falls back to C++
// When disabled: Uses only C++ handling
```

## Passing Custom Data to Lua

### Register New C++ Types
In `PlayerLuaKeyActionsMapping::RegisterLuaBindings()`:
```cpp
// Add your custom component
m_luaState->new_usertype<MyCustomComponent>("MyComponent",
    sol::no_constructor,
    "myProperty", &MyCustomComponent::myProperty,
    "MyMethod", &MyCustomComponent::MyMethod
);

// Add to PlayerObject binding
m_luaState->new_usertype<PlayerObject>("PlayerObject",
    // ... existing bindings ...
    "GetCustom", [](PlayerObject* player) {
        return player->GetComponent<MyCustomComponent>();
    }
);
```

### Use in Lua
```lua
function onCustomAction(action)
    local custom = player:GetCustom()
    custom.myProperty = 42
    custom:MyMethod()
end
```

## Performance Considerations

1. **Lua Overhead**: ~1-5 microseconds per function call (negligible for input)
2. **Use C++ for Hot Paths**: Keep per-frame physics/rendering in C++
3. **Cache Lua Functions**: Don't look up functions every frame
4. **Batch Operations**: Pass multiple values in one call vs many calls

## Common Patterns

### Movement Input
```lua
function onMoveForward(action)
    if action == "down" then
        local transform = player:GetTransform()
        local camera = player:GetCamera()
        local speed = 0.1
        
        transform.position.x = transform.position.x + camera.forward.x * speed
        transform.position.z = transform.position.z + camera.forward.z * speed
    end
end
```

### Toggle State
```lua
local isCrouching = false

function onCrouch(action)
    if action == "down" then
        isCrouching = not isCrouching
        local transform = player:GetTransform()
        transform.scale.y = isCrouching and 0.5 or 1.0
    end
end
```

### Cooldown System
```lua
local lastJumpTime = 0
local jumpCooldown = 0.5

function onJump(action)
    if action ~= "down" then return end
    
    local currentTime = os.clock()
    if currentTime - lastJumpTime < jumpCooldown then
        print("Jump on cooldown!")
        return
    end
    
    lastJumpTime = currentTime
    -- Perform jump...
end
```

## Debugging Tips

1. **Use print()**: Available in Lua, outputs to console
2. **Debug Function**: Call `debugPlayerState()` from C++
3. **Check Console**: All Lua errors logged automatically
4. **Test in Isolation**: Call Lua functions directly from C++ for testing

## File Locations

- Header: `SunshineEngine/include/PlayerObject/PlayerLuaKeyActionsMapping.h`
- Implementation: `SunshineEngine/src/PlayerObject/PlayerLuaKeyActionsMapping.cpp`
- Controller Header: `SunshineEngine/include/PlayerObject/PlayerController.h`
- Controller Impl: `SunshineEngine/src/PlayerObject/PlayerController.cpp`
- Example Script: `Projects/Templates/player_controller.lua`
- Integration Examples: `SunshineEngine/src/PlayerObject/PlayerLuaKeyActionsMapping_Examples.cpp`

## Troubleshooting

### "Lua function not found"
- Check function name matches exactly (case-sensitive)
- Ensure script loaded successfully
- Verify function is defined before calling

### "attempt to index nil value"
- Check if `player` object is set: `m_luaActionMapping.SetPlayerObject(&player)`
- Ensure components exist before accessing

### Script not loading
- Verify file path is correct (relative to executable)
- Check for Lua syntax errors in console
- Ensure sol2 libraries are linked properly

## Best Practices

1. **Initialize Once**: Load Lua scripts during startup, not every frame
2. **Validate Bindings**: Check if keys are bound before executing
3. **Use Hybrid Mode**: Lua for gameplay, C++ for performance
4. **Error Handling**: Always check `ExecuteLuaFunction()` return value
5. **Hot-Reload in Debug**: Enable script reloading for faster iteration
6. **Keep Scripts Simple**: Complex logic should be in C++
7. **Document Functions**: Add comments to Lua functions for team members

## Next Steps

1. Test basic movement with Lua callbacks
2. Add more component bindings (Health, Inventory, etc.)
3. Implement config file for key bindings
4. Add Lua debugging tools
5. Create more complex gameplay scripts

---

**Questions?** Check `PlayerLuaKeyActionsMapping_Examples.cpp` for complete working examples.
