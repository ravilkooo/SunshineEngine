-- Player Controller Lua Script
-- This script defines callback functions for player input
-- Access to 'player' object is provided by the C++ engine

-- Global state for smooth movement
local moveSpeed = 0.1
local mouseSensitivity = 1.0

-- Movement callback functions
function onMoveForward(action)
    if not player then
        print("Error: player object not available")
        return
    end
    
    local transform = player:getTransform()
    local camera = player:getCamera()
    
    if action == "down" then
        print("Moving forward...")
        local forward = camera.forward
        transform.position.x = transform.position.x + forward.x * moveSpeed
        transform.position.y = transform.position.y + forward.y * moveSpeed
        transform.position.z = transform.position.z + forward.z * moveSpeed
    end
end

function onMoveBackward(action)
    if not player then
        print("Error: player object not available")
        return
    end

    local transform = player:getTransform()
    local camera = player:getCamera()

    if action == "down" then
        print("Moving backward...")
        local forward = camera.forward
        transform.position.x = transform.position.x - forward.x * moveSpeed
        transform.position.y = transform.position.y - forward.y * moveSpeed
        transform.position.z = transform.position.z - forward.z * moveSpeed
    end
end

function onStrafeLeft(action)
    if not player then
        print("Error: player object not available")
        return
    end

    local transform = player:getTransform()
    local camera = player:getCamera()

    if action == "down" then
        print("Strafing left...")
        local right = camera.right
        transform.position.x = transform.position.x - right.x * moveSpeed
        transform.position.y = transform.position.y - right.y * moveSpeed
        transform.position.z = transform.position.z - right.z * moveSpeed
    end
end

function onStrafeRight(action)
    if not player then
        print("Error: player object not available")
        return
    end

    local transform = player:getTransform()
    local camera = player:getCamera()

    if action == "down" then
        print("Strafing right...")
        local right = camera.right
        transform.position.x = transform.position.x + right.x * moveSpeed
        transform.position.y = transform.position.y + right.y * moveSpeed
        transform.position.z = transform.position.z + right.z * moveSpeed
    end
end

function onJump(action)
    if not player then
        print("Error: player object not available")
        return
    end
    
    if action == "down" then
        print("Jump!")
        local transform = player:getTransform()
        -- Simple jump - add upward velocity
        transform.position.y = transform.position.y + 0.5
    end
end

function onDown(action)
    if not player then
        print("Error: player object not available")
        return
    end
    
    if action == "down" then
        print("Go Down!")
        local transform = player:getTransform()
        -- Simple go down - add downward velocity
        transform.position.y = transform.position.y - 0.5
    end
end

-- Mouse/Look callback
function onLookAround(deltaX, deltaY, wheelDelta)
    if not player then
        print("Error: player object not available")
        return
    end
    
    local camera = player:getCamera()
    
    -- Apply mouse sensitivity
    local deltaTime = camera.deltaTime
    local sensitivity = 1
    local yawDelta = deltaX * sensitivity * deltaTime
    local pitchDelta = deltaY * sensitivity * deltaTime
    
    local transform = player:getTransform()
    transform.rotation.y = transform.rotation.y + yawDelta

    print("Looking: " .. yawDelta .. " (yaw), " .. pitchDelta .. " (pitch)")
    
    -- Handle mouse wheel for zooming (adjust reference length)
    if wheelDelta ~= 0 then
        local currentLen = camera:getStickLength()
        local zoomAmount = wheelDelta * deltaTime
        local newLen = currentLen - zoomAmount
        -- Clamp zoom distance between 1 and 100 units
        if newLen >= 0.0 and newLen <= 100.0 then
            camera:setStickLength(newLen)
        end
    end

    -- Rotate camera based on mouse movement
    --[[
    Note: Positive yawDelta rotates to the right, positive pitchDelta looks up
    camera:RotateYaw(yawDelta)
    camera:RotatePitch(pitchDelta)
    
    -- Handle mouse wheel for zooming (adjust reference length)
    if wheelDelta ~= 0 then
        local currentLen = camera:getReferenceLen()
        local zoomAmount = wheelDelta * 0.1
        local newLen = currentLen - zoomAmount
        -- Clamp zoom distance between 1 and 100 units
        if newLen >= 1.0 and newLen <= 100.0 then
            camera:setReferenceLen(newLen)
        end
    end
    
    ]]
end

-- Initialization function (called once when script loads)
function onInit()
    print("Player controller Lua script initialized!")
    print("Available functions: onMoveForward, onMoveBackward, onStrafeLeft, onStrafeRight")
    print("                    onJump, onCrouch, onInteract, onToggleSprint")
end

-- Call init
onInit()
