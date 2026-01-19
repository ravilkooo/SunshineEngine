# PlayerController Lua Programming Guide

A comprehensive guide to scripting player behavior in Lua for the Sunshine Engine. This guide covers all available types, functions, patterns, and best practices for writing player controller scripts.

## Table of Contents

1. [Overview](#overview)
2. [Getting Started](#getting-started)
3. [Core Types & API Reference](#core-types--api-reference)
4. [Common Patterns](#common-patterns)
5. [Input Handling](#input-handling)
6. [Physics & Movement](#physics--movement)
7. [Debugging & Best Practices](#debugging--best-practices)

## Overview

The PlayerController Lua system allows you to write player behavior scripts without modifying C++ code. Input events are routed to Lua callback functions, giving you direct access to the player object, camera, transform, and physics systems.

### System Architecture

```
User Input (Keyboard/Mouse)
    ↓
PlayerController (Input Processing)
    ↓
PlayerLuaKeyActionsMapping (Lua Binding)
    ↓
Lua Script Functions (Your Code)
```

## Getting Started

### C++ Setup (Engine Side) (ONLY FOR DEVELOPERS TEAM)

```cpp
// In PlayerObject initialization
AssetPath scriptPath(L"player_controller.lua", AssetPath::AssetSource::Project);

m_luaActionMapping.Initialize(WStringToUtf8(scriptPath.GetFullPath()));
m_luaActionMapping.SetPlayerObject(this);

// Bind input keys to Lua functions
m_luaActionMapping.BindKey(Keys::W, "onMoveForward");
m_luaActionMapping.BindKey(Keys::S, "onMoveBackward");
m_luaActionMapping.BindKey(Keys::A, "onStrafeLeft");
m_luaActionMapping.BindKey(Keys::D, "onStrafeRight");
m_luaActionMapping.BindKey(Keys::Space, "onJump");
m_luaActionMapping.InitMouseActionHandler("onLookAround");

m_playerController.SetLuaCallbackMode(true);
```

### Lua Script Skeleton

```lua
-- Global configuration
local moveSpeed = 0.1
local mouseSensitivity = 1.0

-- Input callback
function onMoveForward(action)
    if action == "down" then
        if not player then return end
        -- Your code here
    end
end

-- Initialization (called once when script loads)
function onInit()
    print("Player controller initialized!")
end

onInit()
```

## Core Types & API Reference

### PlayerObject

The main global object representing the player. Access it via the `player` global variable.

**Methods:**
```lua
player:getTransform()    -- Returns TransformComponent
player:getCamera()       -- Returns Camera
player:getPhysics()      -- Returns PhysicsComponent
player:getName()         -- Returns string (player name)
```

**Example:**
```lua
function onMoveForward(action)
    if not player then return end
    
    local transform = player:getTransform()
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local physics = player:getPhysics()
end
```

---

### TransformComponent

Represents position, rotation, and scale of the player object.

**Properties:**
- `position` (Vector3) - World position, read/write
- `rotation` (Vector3) - Rotation in Euler angles (x, y, z), read/write  
- `scale` (Vector3) - Scale factors, read/write

**Example:**
```lua
function onMoveForward(action)
    if action == "down" then
        local transform = player:getTransform()
        
        -- Modify position directly
        transform.position.x = transform.position.x + 0.1
        transform.position.y = transform.position.y + 0.1
        transform.position.z = transform.position.z + 0.1
        
        -- Rotate player
        transform.rotation.y = transform.rotation.y + 0.1
        
        -- Scale player
        transform.scale.x = 2.0
    end
end
```

---

### Vector3

3D vector type for position, direction, and forces.

**Construction:**
```lua
local v1 = Vector3(1.0, 2.0, 3.0)
local v2 = Vector3()  -- Default (0, 0, 0)
```

**Properties:**
- `x` (float) - X component, read/write
- `y` (float) - Y component, read/write
- `z` (float) - Z component, read/write

**Methods:**
- `Length()` - Returns float, magnitude of vector
<!-- - `Normalize()` - Normalizes vector in place (commented out in binding) -->

**Example:**
```lua
local forward = Vector3(1.0, 0.0, 0.0)
local length = forward:Length()  -- Returns 1.0

-- Construct from camera direction
local moveVector = Vector3(
    camera.forward.x * speed,
    camera.forward.y * speed,
    camera.forward.z * speed
)
```

---

### CameraComponent

The camera component provides control over the camera's view, position, rotation, and various camera modes. It manages the player's perspective with support for first-person, orbital, and follow camera modes.

#### Getting the Component

```lua
local cameraComponent = player:getCameraComponent()
local camera = cameraComponent:getCamera()
```

---

### Camera

Handles view frustum, direction vectors, and stick-based camera control (orbiting/third-person).

**Read-Only Properties:**
- `forward` (Vector3) - Forward direction vector
- `up` (Vector3) - Up direction vector
- `right` (Vector3) - Right direction vector
- `position` (Vector3) - Camera world position
- `deltaTime` (float) - Delta time for this frame

**Methods:**

Position Movement:
```lua
camera:setPosition(x, y, z)
camera:getPosition()
camera:moveForward(moveSpeed)
camera:moveBackward(moveSpeed)
camera:moveLeft(moveSpeed)
camera:moveRight(moveSpeed)
camera:moveUp(moveSpeed)
camera:moveDown(moveSpeed)
```

Target & Up Vector (for orbit camera):
```lua
camera:setTarget(x, y, z)
camera:getTarget()
camera:setUp(x, y, z)
camera:getUp()
```

Rotation:
```lua
camera:rotateYaw(angleSpeed)
camera:rotatePitch(angleSpeed)
camera:switchToFPSMode()
```

Zoom (Stick Length - for orbiting camera):
```lua
camera:getStickLength()      -- Distance from target
camera:setStickLength(length)
camera:getStickDirection()   -- Direction of stick as Vector3; The same as Camera direction
```

View Dimensions:
```lua
camera:getViewWidth()
camera:getViewHeight()
camera:setNearZ(distance)
camera:getNearZ()
camera:setFarZ(distance)
camera:getFarZ()
camera:setReferenceLen(length)
camera:getReferenceLen()
```

**Example (Kinematic Camera):**
```lua
function onLookAround(deltaX, deltaY, wheelDelta)
    if not player then return end
    
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local deltaTime = camera.deltaTime
    local sensitivity = 1.0
    
    -- Rotate object
    local yawDelta = deltaX * sensitivity * deltaTime
    local pitchDelta = deltaY * sensitivity * deltaTime

    local transform = player:getTransform()
    transform.rotation.y = transform.rotation.y + yawDelta
    
    -- Zoom with mouse wheel
    if wheelDelta ~= 0 then
        local currentLen = camera:getStickLength()
        local zoomAmount = wheelDelta * deltaTime
        local newLen = currentLen - zoomAmount
        if newLen >= 0.0 and newLen <= 100.0 then
            camera:setStickLength(newLen)
        end
    end
end
```

---

### PhysicsComponent

Direct access to physics engine (Jolt Physics). Apply forces, impulses, torques, and query velocities.

**Force & Impulse Methods:**
```lua
physics:addForce(vector3)           -- Apply constant force
physics:addImpulse(vector3)         -- Apply instantaneous impulse
physics:addTorque(vector3)          -- Apply rotational torque
physics:addAngularImpulse(vector3)  -- Apply instantaneous angular impulse
```

**Query Methods:**
```lua
physics:getAccumulatedForce()    -- Returns Vector3
physics:getAccumulatedTorque()   -- Returns Vector3
physics:getLinearVelocity()      -- Returns Vector3
physics:getAngularVelocity()     -- Returns Vector3
physics:getPointVelocity(point)  -- Returns Vector3 (velocity at specific point)
physics:getPosition()            -- Returns Vector3
physics:getRotation()            -- Returns Quaternion
```

**Velocity Control:**
```lua
physics:setLinearVelocity(vector3)  -- Direct velocity override
physics:setAngularVelocity(vector3) -- Direct angular velocity override
```

**Reset Methods:**
```lua
physics:resetForce()  -- Clear accumulated force
physics:resetTorque() -- Clear accumulated torque
```

**Example (Dynamic Movement):**
```lua
local moveForce = 1000.0

function onMoveForward(action)
    if not player then return end
    
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local physics = player:getPhysics()
    
    if action == "down" then
        print("Moving forward...")
        local forward = Vector3(
            camera.forward.x * moveForce,
            camera.forward.y * moveForce,
            camera.forward.z * moveForce
        )
        physics:addImpulse(forward)
    end
end

function onJump(action)
    if action == "down" then
        local physics = player:getPhysics()
        local up = Vector3(0, moveForce, 0)
        physics:addImpulse(up)
    end
end
```

---

---

## UUID & Scene Helpers

The engine exposes a `UUID` usertype (implemented as `SE::UUIDhilo`) for safe 32-bit access from Lua. This represents a 64-bit UUID split into two 32-bit integers to avoid precision loss in Lua numbers.

### `UUID` (`SE::UUIDhilo`)

- Fields: `hi` (uint32), `lo` (uint32)
- Methods: `toString()` — returns the full UUID as a string
- Comparison: `isEqual(other)` — compares `hi` and `lo` fields

Example:

```lua
local id = UUID() -- constructor
id.hi = 12345
id.lo = 67890
print(id:toString())
```

### `PlayerObject` and `GameObject` UUID access

Both `PlayerObject` and `GameObject` expose a `getUUID()` method which returns the `UUID` (`SE::UUIDhilo`) representing the object's identifier. `PlayerObject` also exposes `getName()`.

Example:

```lua
local pid = player:getUUID()
print("Player UUID:", pid:toString())
print("Player name:", player:getName())
```

For any `GameObject` (base type) the following getters are available (component getters shown here as examples):

```lua
local t = gameObject:getTransform()   -- returns TransformComponent
local p = gameObject:getPhysics()     -- returns PhysicsComponent (if present)
local name = gameObject:getName()      -- object name (string)
local id = gameObject:getUUID()        -- returns UUID (hi/lo)
```

### Scene helpers

- `removeGameObjectByUUID(uuid)` — remove a game object (and its children) from the active scene; accepts a `UUID` (hi/lo) value.
- `getGameObjectByUUID(uuid)` — returns the `GameObject` instance for the given `UUID`, or `nil` if not found.

Examples:

```lua
-- Remove an object by UUID
local id = someObject:getUUID()
removeGameObjectByUUID(id)

-- Find an object by UUID and access its transform
local other = getGameObjectByUUID(id)
if other then
    local t = other:getTransform()
    print("Found object position:", t.position.x, t.position.y, t.position.z)
end
```


## Common Patterns

### Static Movement (Kinematic)

Move player by directly modifying transform position.

**From player_controller.lua:**
```lua
local moveSpeed = 0.1

function onMoveForward(action)
    if not player then return end
    
    local transform = player:getTransform()
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    
    if action == "down" then
        print("Moving forward...")
        local forward = camera.forward
        transform.position.x = transform.position.x + forward.x * moveSpeed
        transform.position.y = transform.position.y + forward.y * moveSpeed
        transform.position.z = transform.position.z + forward.z * moveSpeed
    end
end

function onMoveBackward(action)
    if not player then return end
    
    local transform = player:getTransform()
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    
    if action == "down" then
        print("Moving backward...")
        local forward = camera.forward
        transform.position.x = transform.position.x - forward.x * moveSpeed
        transform.position.y = transform.position.y - forward.y * moveSpeed
        transform.position.z = transform.position.z - forward.z * moveSpeed
    end
end

function onStrafeLeft(action)
    if not player then return end
    
    local transform = player:getTransform()
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    
    if action == "down" then
        print("Strafing left...")
        local right = camera.right
        transform.position.x = transform.position.x - right.x * moveSpeed
        transform.position.y = transform.position.y - right.y * moveSpeed
        transform.position.z = transform.position.z - right.z * moveSpeed
    end
end

function onStrafeRight(action)
    if not player then return end
    
    local transform = player:getTransform()
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    
    if action == "down" then
        print("Strafing right...")
        local right = camera.right
        transform.position.x = transform.position.x + right.x * moveSpeed
        transform.position.y = transform.position.y + right.y * moveSpeed
        transform.position.z = transform.position.z + right.z * moveSpeed
    end
end

function onJump(action)
    if not player then return end
    
    if action == "down" then
        print("Jump!")
        local transform = player:getTransform()
        transform.position.y = transform.position.y + 0.5
    end
end

function onDown(action)
    if not player then return end
    
    if action == "down" then
        print("Go Down!")
        local transform = player:getTransform()
        transform.position.y = transform.position.y - 0.5
    end
end
```

---

### Dynamic Movement (Physics-Based)

Apply impulses and forces through the physics engine.

**From player_controller_dynamic.lua:**
```lua
local moveForce = 1000.0

function onMoveForward(action)
    if not player then return end
    
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local physics = player:getPhysics()
    
    if action == "down" then
        print("Moving forward...")
        local forward = Vector3(
            camera.forward.x * moveForce,
            camera.forward.y * moveForce,
            camera.forward.z * moveForce
        )
        physics:addImpulse(forward)
    end    
end

function onMoveBackward(action)
    if not player then return end
        
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local physics = player:getPhysics()
    
    if action == "down" then
        print("Moving backward...")
        local forward = Vector3(
            camera.forward.x * -moveForce,
            camera.forward.y * -moveForce,
            camera.forward.z * -moveForce
        )
        physics:addImpulse(forward)
    end    
end

function onStrafeLeft(action)
    if not player then return end
     
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local physics = player:getPhysics()
    
    if action == "down" then
        print("Moving left...")
        local left = Vector3(
            camera.right.x * -moveForce,
            camera.right.y * -moveForce,
            camera.right.z * -moveForce
        )
        physics:addImpulse(left)
    end    
end

function onStrafeRight(action)
    if not player then return end
         
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local physics = player:getPhysics()
    
    if action == "down" then
        print("Moving right...")
        local right = Vector3(
            camera.right.x * moveForce,
            camera.right.y * moveForce,
            camera.right.z * moveForce
        )
        physics:addImpulse(right)
    end   
end

local lastJumpTime = 0
local jumpCooldown = 3

function onJump(action)
    if not player then
        print("Error: player object not available")
        return
    end
         
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local physics = player:getPhysics()

    if action == "down" then
        local currentTime = os.clock()

        print("currentTime: " .. currentTime .. ", lastJumpTime: " .. lastJumpTime .. ", cooldown: " .. jumpCooldown)

        if currentTime - lastJumpTime > jumpCooldown then
            lastJumpTime = currentTime
            local up = Vector3.new(0, moveForce * 10, 0)
            physics:addImpulse(up)
            print("Jump!")
            return
        end
    end   
end

function onDown(action)
    if not player then return end
         
    local physics = player:getPhysics()
    
    if action == "down" then
        print("Down!")
        local down = Vector3(0, -moveForce, 0)
        physics:addImpulse(down)
    end   
end

function onTorque(action)
    if not player then return end
         
    local physics = player:getPhysics()
    
    if action == "down" then
        print("Torque!")
        local torque = Vector3(0, moveForce, 0)
        physics:addTorque(torque)
    end   
end

function onAngularImpulse(action)
    if not player then return end
         
    local physics = player:getPhysics()
    
    if action == "down" then
        print("AngularImpulse!")
        local angularImpulse = Vector3(0, moveForce, 0)
        physics:addAngularImpulse(angularImpulse)
    end   
end
```

---

### Camera Control (Mouse Look)

**From both player_controller.lua and player_controller_dynamic.lua:**
```lua
function onLookAround(deltaX, deltaY, wheelDelta)
    if not player then return end
    
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local deltaTime = camera.deltaTime
    local sensitivity = 1.0
    
    -- Calculate rotation deltas
    local yawDelta = deltaX * sensitivity * deltaTime
    local pitchDelta = deltaY * sensitivity * deltaTime
    
    print("Looking: " .. yawDelta .. " (yaw), " .. pitchDelta .. " (pitch)")
    
    -- Handle mouse wheel zoom (for stick-based/orbit camera)
    if wheelDelta ~= 0 then
        local currentLen = camera:getStickLength()
        local zoomAmount = wheelDelta * deltaTime
        local newLen = currentLen - zoomAmount
        
        -- Clamp zoom distance
        if newLen >= 0.0 and newLen <= 100.0 then
            camera:setStickLength(newLen)
        end
    end
end
```

---

### State Management with Cooldowns

```lua
local lastJumpTime = 0
local jumpCooldown = 3

function onJump(action)
    if not player then
        print("Error: player object not available")
        return
    end
         
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local physics = player:getPhysics()

    if action == "down" then
        local currentTime = os.clock()

        if currentTime - lastJumpTime > jumpCooldown then
            lastJumpTime = currentTime
            local up = Vector3.new(0, moveForce * 10, 0)
            physics:AddImpulse(up)
            print("Jump!")
            return
        end
    end   
end
```

---

### Toggle State (Crouch/Sprint)

```lua
local isCrouching = false

function onCrouch(action)
    if action ~= "down" then return end
    if not player then return end
    
    isCrouching = not isCrouching
    
    local transform = player:getTransform()
    if isCrouching then
        transform.scale.y = 0.5
        print("Crouching")
    else
        transform.scale.y = 1.0
        print("Standing")
    end
end
```

---

## Input Handling

### Input Callback Signature

All input callbacks receive an `action` parameter indicating the input state:

```lua
function onKeyAction(action)
    if action == "down" then
        -- Key pressed (called once per key press)
    elseif action == "up" then
        -- Key released (called once per key release)
    end
end
```

### Mouse Action Callback

Mouse movement and wheel events pass precise values:

```lua
function onLookAround(deltaX, deltaY, wheelDelta)
    -- deltaX: Horizontal mouse movement (pixels)
    -- deltaY: Vertical mouse movement (pixels)
    -- wheelDelta: Mouse wheel movement
end
```

---

## Physics & Movement

### Key Differences: Kinematic vs. Dynamic

**Kinematic (Static Movement):**
- Directly modify transform.position
- Instant, predictable movement
- No collision response
- Use when: walking, scripted sequences, UI-controlled movement

```lua
local moveSpeed = 0.1
transform.position.x = transform.position.x + moveSpeed
```

**Dynamic (Physics-Based):**
- Apply forces/impulses through physics engine
- Affected by gravity, collisions, physics solver
- More realistic, responds to world
- Use when: ragdoll, objects affected by world, realistic combat

```lua
local moveForce = 1000.0
local impulse = Vector3(camera.forward.x * moveForce, 0, camera.forward.z * moveForce)
physics:AddImpulse(impulse)
```

### Velocity Queries

```lua
local physics = player:getPhysics()

-- Get current velocities
local linearVel = physics:getLinearVelocity()
local angularVel = physics:getAngularVelocity()

-- get velocity at specific world point
local velAtPoint = physics:getPointVelocity(Vector3(0, 1, 0))

-- Calculate speed
local speed = linearVel:Length()
```

---

## Debugging & Best Practices

### Logging

Use the `print()` function for debugging:

```lua
print("Player name: " .. player:getName())
print("Position: " .. player:getTransform().position.x)
print("Action triggered")
```

All output goes to the engine console.

### Best Practices

1. **Check for `nil` player:**
   ```lua
   if not player then
       print("Error: player object not available")
       return
   end
   ```

2. **Validate input action:**
   ```lua
   function onAction(action)
       if action == "down" then
           -- Handle press
       end
   end
   ```

3. **Use local variables for components:**
   ```lua
   function onMoveForward(action)
       if not player then return end
       
       local transform = player:getTransform()
       local camera = player:getCamera()
       -- Reuse components in function
   end
   ```

4. **Performance: Separate hot-path logic**
   - Simple input: Lua is fine
   - Per-frame updates: Consider C++
   - Physics: Can be Lua (engine handles heavy lifting)
   - Rendering: Must be C++

5. **Use configuration variables:**
   ```lua
   local moveSpeed = 0.1
   local mouseSensitivity = 1.0
   local jumpForce = 1000.0
   
   -- Adjust at top of script
   ```

6. **Comment your functions:**
   ```lua
   -- Move player forward in camera direction
   -- Called on W key press
   function onMoveForward(action)
       -- ...
   end
   ```

### Common Issues

**"Error: player object not available"**
- Solution: Ensure `player:SetPlayerObject()` called in C++
- Check function guards: `if not player then return end`

**Movement feels jerky**
- Kinematic: Reduce `moveSpeed`, respect deltaTime
- Dynamic: Use smaller forces or impulses

**Camera not rotating**
- Check mouse callback `onLookAround` is bound
- Verify camera:getStickLength() > 0 for orbit camera

**Physics not responding**
- Ensure physics body exists and is dynamic
- Check mass/friction settings in C++
- Verify impulse magnitude is sufficient

---

## Example: Complete Player Controller

**Static Movement with Kinematic Camera:**
```lua
local moveSpeed = 0.1
local mouseSensitivity = 1.0

function onMoveForward(action)
    if not player then return end
    if action ~= "down" then return end
    
    local transform = player:getTransform()
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    transform.position.x = transform.position.x + camera.forward.x * moveSpeed
    transform.position.y = transform.position.y + camera.forward.y * moveSpeed
    transform.position.z = transform.position.z + camera.forward.z * moveSpeed
end

function onLookAround(deltaX, deltaY, wheelDelta)
    if not player then return end
    
    local cameraComponent = player:getCameraComponent()
    local camera = cameraComponent:getCamera()
    local deltaTime = camera.deltaTime
    
	-- This behaviour is handled inside C++ engine
	-- So no need for rotation camera in Lua
    -- camera:RotateYaw(deltaX * mouseSensitivity * deltaTime)
    -- camera:RotatePitch(deltaY * mouseSensitivity * deltaTime)
    
    if wheelDelta ~= 0 then
        local newLen = camera:getStickLength() - wheelDelta * deltaTime
        if newLen >= 0.0 and newLen <= 100.0 then
            camera:SetStickLength(newLen)
        end
    end
end

function onInit()
    print("Player controller ready!")
end

onInit()
```

---

## File References

- **Example Scripts:** [Projects/DefaultScene/Scripts/](Projects/DefaultScene/Scripts/)
  - `player_controller.lua` - Kinematic movement example
  - `player_controller_dynamic.lua` - Physics-based movement example
- **Implementation:** [SunshineEngine/src/PlayerObject/](SunshineEngine/src/PlayerObject/)
  - `PlayerLuaKeyActionsMapping.cpp` - Lua binding implementation
  - `PlayerController.cpp` - Input routing
