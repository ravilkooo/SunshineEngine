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
void PlayerObject::SetupLuaActionMapping_test()
{
	// In your PlayerObject constructor or initialization:
	AssetPath scriptPath(L"player_controller.lua", AssetPath::AssetSource::Project);

	m_luaActionMapping.Initialize(WStringToUtf8(scriptPath.GetFullPath()));
	m_luaActionMapping.SetPlayerObject(this);

	// Bind keys
	m_luaActionMapping.BindKey(Keys::Q, "onMoveForward");
	m_luaActionMapping.BindKey(Keys::E, "onJump");

	// Enable Lua mode
	m_playerController.SetLuaCallbackMode(true);
}
```

### 2. Bind Keys to Lua Functions
```cpp
	// Bind keys
	m_luaActionMapping.BindKey(Keys::Q, "onMoveForward");
	m_luaActionMapping.BindKey(Keys::E, "onJump");
```

### 3. Enable Lua Mode
```cpp
    // Enable Lua mode
	m_playerController.SetLuaCallbackMode(true);
```

### 4. Input Handling
```cpp
void Game::HandleKeyDown(Keys key)
{
	m_playerObject->m_playerController.HandleKeyDown(key);
}

void Game::HandleKeyUp(Keys key)
{
	m_playerObject->m_playerController.HandleKeyUp(key);
}
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
void PlayerObject::SetupLuaActionMapping_test()
{
	// In your PlayerObject constructor or initialization:
	AssetPath scriptPath(L"player_controller.lua", AssetPath::AssetSource::Project);

	m_luaActionMapping.Initialize(WStringToUtf8(scriptPath.GetFullPath()));
	m_luaActionMapping.SetPlayerObject(this);

	// Bind keys
	m_luaActionMapping.BindKey(Keys::Q, "onMoveForward");
	m_luaActionMapping.BindKey(Keys::E, "onJump");

	// Enable Lua mode
	m_playerController.SetLuaCallbackMode(true);
}
```

### Example 2: Input Polling Loop
```cpp
void Game::Update(float deltaTime) {

	 m_luaManager.Update(m_scene.get(), deltaTime);

	 m_physicsSystem->Step(deltaTime);

	 m_physicsSystem->SyncronizeTransforms(m_scene.get());

	 if (m_particleSystem)
		 m_particleSystem->Update(deltaTime);

	 m_playerObject->m_playerController.UpdatePlayer(deltaTime);
}

void PlayerController::UpdatePlayer(float deltaTime)
{
	// Update input state for this frame - computes edge events
	m_inputManager.Update();

	// Handle camera rotation
	m_player->m_playerCamera->RotateStickYawPitch(deltaTime * m_stickYawMoveDir, deltaTime * m_stickPitchMoveDir);
	ExecuteAllOnKeyDown();

	// Handle movement using InputManager (supports key held)
	if (m_inputManager.IsKeyDown(Keys::W) ||
		m_inputManager.IsKeyDown(Keys::A) ||
		m_inputManager.IsKeyDown(Keys::S) ||
		m_inputManager.IsKeyDown(Keys::D))
	{
		m_moveDir =
		{
			(m_inputManager.IsKeyDown(Keys::D) - m_inputManager.IsKeyDown(Keys::A)) * 1.0f,
			0.0f,
			(m_inputManager.IsKeyDown(Keys::W) - m_inputManager.IsKeyDown(Keys::S)) * 1.0f
		};
		m_moveDir.Normalize();
		m_moveDir = DXSM::Vector3::Transform(m_moveDir, m_player->m_playerCamera->rotateCamToForward);
		m_player->GetComponent<TransformComponent>()->m_position += m_moveDir * m_moveSpeed;
	}

	m_stickYawMoveDir = 0.0f;
	m_stickPitchMoveDir = 0.0f;
	m_moveDir = DXSM::Vector3::Zero;
}
```

### Example 3: Manual Lua Function Calls
```cpp
// Call Lua function directly with custom parameters
m_player->m_luaActionMapping.ExecuteLuaFunction("onUpdate", deltaTime);
m_player->m_luaActionMapping.ExecuteLuaFunction("onLookAround", mouseX, mouseY);

void PlayerController::HandleKeyDown(Keys key)
{
	// Feed input to InputManager for proper edge detection
	m_inputManager.ProcessKeyDown(key);

	// Try Lua callback for key press (edge event)
	if (m_useLuaCallbacks && m_player) {
		// Only call Lua on the press event (not every frame while held)
		if (m_inputManager.IsKeyPressed(key))
		{
			m_player->m_luaActionMapping.ExecuteKeyAction(key, "pressed");
		}
		else if (m_inputManager.IsKeyDown(key))
		{
			m_player->m_luaActionMapping.ExecuteKeyAction(key, "down");
		}
	}
}
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

1. **Lua Overhead**: per function call (negligible for input)
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
- Example Script: `Projects/$ProjectName$/player_controller.lua`

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
