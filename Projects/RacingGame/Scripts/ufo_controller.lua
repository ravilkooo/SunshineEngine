-- Player Controller Lua Script
-- This script defines callback functions for player input
-- Access to 'player' object is provided by the C++ engine

-- Global state for movement
local moveSpeed = 3500
local mouseSensitivity = 1.0

-- Movement callback functions
function onMoveForward(action)
    if not player then return end
    
    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()
    
    if action == "down" then
        local forward = Vector3.new(camera.forward.x * moveSpeed, camera.forward.y * moveSpeed, camera.forward.z * moveSpeed)
        physics:addImpulse(forward)
    end
end

function onMoveBackward(action)
    if not player then return end

    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()
    
    if action == "down" then
        local backward = Vector3.new(camera.forward.x * -moveSpeed, camera.forward.y * -moveSpeed, camera.forward.z * -moveSpeed)
        physics:addImpulse(backward)
    end
end

function onMoveUp(action)
    if not player then return end
    
    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()
    
    if action == "down" then
        local up = Vector3.new(camera.up.x * moveSpeed, camera.up.y * moveSpeed, camera.up.z * moveSpeed)
        physics:addImpulse(up)
    end    
end

function onMoveDown(action)
    if not player then return end
    
    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()
    
    if action == "down" then
        local down = Vector3.new(camera.up.x * -moveSpeed, camera.up.y * -moveSpeed, camera.up.z * -moveSpeed)
        physics:addImpulse(down)
    end    
end

function onStrafeLeft(action)
    if not player then return end
end

function onStrafeRight(action)
    if not player then return end
end

-- Special function called every frame (if bound)
function onUpdate(deltaTime)
    if not player then return end
end

-- Mouse/Look callback
function onLookAround(deltaX, deltaY, wheelDelta)
    print("look around")
    if not player then return end
    camera:switchToFPSMode()
    print("Looking: " .. deltaX .. ", " .. deltaY)
    -- Camera rotation is typically handled in C++ for performance
    -- but you can add custom look behavior here
end

-- Utility function to get player info
function getPlayerInfo()
    if not player then
        return "Player not available"
    end
    
    local transform = player:GetTransform()
    return "Player: " .. player:GetName() .. 
           " at (" .. transform.position.x .. ", " .. 
           transform.position.y .. ", " .. 
           transform.position.z .. ")"
end

-- Debug function
function debugPlayerState()
    print("=== Player Debug Info ===")
    print(getPlayerInfo())
    
    if player then
        local camera = player:GetCamera()
        print("Camera forward: " .. camera.forward.x .. ", " .. 
              camera.forward.y .. ", " .. camera.forward.z)
    end
    print("Move speed: " .. moveSpeed)
    print("========================")
end

-- Initialization function (called once when script loads)
function onInit()
    print("Player controller Lua script initialized!")
    print("Available functions: onMoveForward, onMoveBackward, onStrafeLeft, onStrafeRight")
    print("                     onJump, onCrouch, onInteract, onToggleSprint")
end

-- Call init
onInit()
