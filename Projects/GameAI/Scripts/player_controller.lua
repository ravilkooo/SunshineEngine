-- Player Controller Lua Script
-- This script defines callback functions for player input
-- Access to 'player' object is provided by the C++ engine

-- Global state for smooth movement
local moveSpeed = 0.1
local mouseSensitivity = 1.0
local initStickLen

-- Movement callback functions
function onMoveForward(action)
    if not player then
        -- print("Error: player object not available")
        return
    end
    
    local transform = player:getTransform()
    local camera = player:getCamera()
    initStickLen = camera:getStickLength()
    
    if action == "down" then
        -- print("Moving forward...")
        -- get forward direction from camera
        local forward = camera.forward
        transform.m_position.x = transform.m_position.x + forward.x * moveSpeed
        transform.m_position.y = transform.m_position.y + forward.y * moveSpeed
        transform.m_position.z = transform.m_position.z + forward.z * moveSpeed
    end
end

function onMoveBackward(action)
    if not player then return end
    
    local transform = player:getTransform()
    local camera = player:getCamera()
    
    if action == "down" then
        -- print("Moving backward...")
        local forward = camera.forward
        transform.m_position.x = transform.m_position.x - forward.x * moveSpeed
        transform.m_position.y = transform.m_position.y - forward.y * moveSpeed
        transform.m_position.z = transform.m_position.z - forward.z * moveSpeed
    end
end

function onStrafeLeft(action)
    if not player then return end
    
    local transform = player:getTransform()
    local camera = player:getCamera()
    
    if action == "down" then
        -- print("Strafing left...")
        local right = camera.right
        transform.m_position.x = transform.m_position.x - right.x * moveSpeed
        transform.m_position.y = transform.m_position.y - right.y * moveSpeed
        transform.m_position.z = transform.m_position.z - right.z * moveSpeed
    end
end

function onStrafeRight(action)
    if not player then return end
    
    local transform = player:getTransform()
    local camera = player:getCamera()
    
    if action == "down" then
        -- print("Strafing right...")
        local right = camera.right
        transform.m_position.x = transform.m_position.x + right.x * moveSpeed
        transform.m_position.y = transform.m_position.y + right.y * moveSpeed
        transform.m_position.z = transform.m_position.z + right.z * moveSpeed
    end
end

function onJump(action)
    if not player then return end
    
    if action == "down" then
        print("Jump!")
        local transform = player:getTransform()
        -- Simple jump - add upward velocity
        transform.m_position.y = transform.m_position.y + 0.5
    end
end

function onCrouch(action)
    if not player then return end
    
    local transform = player:getTransform()
    
    if action == "down" then
        print("Crouching...")
        transform.scale.y = 0.5
    elseif action == "up" then
        print("Standing up...")
        transform.scale.y = 1.0
    end
end

function onInteract(action)
    if not player then return end
    
    if action == "down" then
        print("Interacting with object...")
        -- Raycast or proximity check would go here
        local transform = player:getTransform()
        print("Player position: " .. transform.m_position.x .. ", " .. 
              transform.m_position.y .. ", " .. transform.m_position.z)
    end
end

function onToggleSprint(action)
    if action == "down" then
        moveSpeed = moveSpeed * 2.0
        print("Sprinting! Speed: " .. moveSpeed)
    elseif action == "up" then
        moveSpeed = moveSpeed * 0.5
        print("Normal speed: " .. moveSpeed)
    end
end

-- Special function called every frame (if bound)
function onUpdate(deltaTime)
    if not player then return end
    
    -- Continuous logic can go here
    -- Example: breathing animation, stamina regen, etc.
end

-- Mouse/Look callback
function onLookAround(deltaX, deltaY, wheelDelta)
    if not player then return end
    
    local camera = player:getCamera()
    if camera then
        local currentStickLen = camera:getStickLength()
        -- Adjust camera orientation based on mouse movement
        camera:setStickLength(currentStickLen + wheelDelta * 1)
    end

    print("Looking: " .. deltaX .. ", " .. deltaY)
    -- Camera rotation is typically handled in C++ for performance
    -- but you can add custom look behavior here

end

-- Utility function to get player info
function getPlayerInfo()
    if not player then
        return "Player not available"
    end
    
    local transform = player:getTransform()
    return "Player: " .. player:getName() .. 
           " at (" .. transform.m_position.x .. ", " .. 
           transform.m_position.y .. ", " .. 
           transform.m_position.z .. ")"
end

-- Debug function
function debugPlayerState()
    print("=== Player Debug Info ===")
    print(getPlayerInfo())
    
    if player then
        local camera = player:getCamera()
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
    print("                    onJump, onCrouch, onInteract, onToggleSprint")
end

-- Call init
onInit()
