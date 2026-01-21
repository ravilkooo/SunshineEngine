-- Player Controller Lua Script
-- This script defines callback functions for player input
-- Access to 'player' object is provided by the C++ engine

-- Global state for smooth movement
local moveForce = 1000.0
local mouseSensitivity = 1.0

-- Movement callback functions
function onMoveForward(action)
    if not player then
        print("Error: player object not available")
        return
    end
    
    
    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()
    
    if action == "down" then
        -- print("Moving forward...")
        local forward = Vector3.new(camera.forward.x * moveForce, camera.forward.y * moveForce, camera.forward.z * moveForce)
        physics:addImpulse(forward)
    end    
end

function onMoveBackward(action)
    if not player then
        -- print("Error: player object not available")
        return
    end

    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()

    if action == "down" then
        -- print("Moving backward...")
        local forward = Vector3.new(camera.forward.x * -moveForce, camera.forward.y * -moveForce, camera.forward.z * -moveForce)
        physics:addImpulse(forward)
    end    
end

function onStrafeLeft(action)
    if not player then
        -- print("Error: player object not available")
        return
    end
     
    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()

    if action == "down" then
        -- print("Moving left...")
        local left = Vector3.new(camera.right.x * -moveForce, camera.right.y * -moveForce, camera.right.z * -moveForce)
        physics:addImpulse(left)
    end    
end

function onStrafeRight(action)
    if not player then
        -- print("Error: player object not available")
        return
    end

    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()

    if action == "down" then
        -- print("Moving right...")
        local right = Vector3.new(camera.right.x * moveForce, camera.right.y * moveForce, camera.right.z * moveForce)
        physics:addImpulse(right)
    end   
end

local lastJumpTime = 0
local jumpCooldown = 3

function onJump(action)
    if not player then
        -- print("Error: player object not available")
        return
    end

    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()

    if action == "down" then
        local currentTime = os.clock()

        -- print("currentTime: " .. currentTime .. ", lastJumpTime: " .. lastJumpTime .. ", cooldown: " .. jumpCooldown)

        if currentTime - lastJumpTime > jumpCooldown then
            lastJumpTime = currentTime
            local up = Vector3.new(0, moveForce * 10, 0)
            physics:addImpulse(up)
            -- print("Jump!")
            return
        end
    end   
end

function onDown(action)
    if not player then
        -- print("Error: player object not available")
        return
    end

    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()

    if action == "down" then
        -- print("Down!")
        local down = Vector3.new(0, -moveForce, 0)
        physics:addImpulse(down)
    end   
end

function onAngularImpulse(action)
    if not player then
        -- print("Error: player object not available")
        return
    end

    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()

    if action == "down" then
        -- print("AngularImpulse!")
        local down = Vector3.new(0, moveForce, 0)
        physics:addAngularImpulse(down)
    end   
end

function onAngularImpulse_inv(action)
    if not player then
        -- print("Error: player object not available")
        return
    end

    local camera = player:getCameraComponent():getCamera()
    local physics = player:getPhysics()

    if action == "down" then
        -- print("AngularImpulse!")
        local down = Vector3.new(0, -moveForce, 0)
        physics:addAngularImpulse(down)
    end   
end

-- Interaction callback
function onInteract(action)
    if not player then
        -- print("Error: player object not available")
        return
    end

    if action == "pressed" then
        print("Interact action triggered!")
        -- Implement interaction logic here
        local customUUID = UUID.new()
        customUUID.hi = 163351393
        customUUID.lo = 285791296
        local customObject = getGameObjectByUUID(customUUID)
        local customTransform = customObject:getTransform()
        print("Transform: " .. customTransform.m_position.x .. ", " .. customTransform.m_position.y .. ", " .. customTransform.m_position.z)
        
        removeGameObjectByUUID(customUUID)
    end
end

-- Mouse/Look callback
function onLookAround(deltaX, deltaY, wheelDelta)
    if not player then
        -- print("Error: player object not available")
        return
    end

    local camera = player:getCameraComponent():getCamera()

    -- Apply mouse sensitivity
    local deltaTime = camera.deltaTime
    local sensitivity = 1
    local yawDelta = deltaX * sensitivity * deltaTime
    local pitchDelta = deltaY * sensitivity * deltaTime
    
    -- transform.rotation.y = transform.rotation.y + yawDelta
    
    -- print("Looking: " .. yawDelta .. " (yaw), " .. pitchDelta .. " (pitch)")
    
    -- local physics = player:getPhysics()
    -- local transform = player:getTransform()
    -- local stickDir = camera:getStickDirection()
    -- stickDir.y = 0
    -- local stickDirLen = stickDir:Length()
    -- stickDir.x = stickDir.x / stickDirLen
    -- stickDir.z = stickDir.z / stickDirLen
    -- local objForwardDir = Vector3.new(math.cos(transform.rotation.y), 0, math.sin(transform.rotation.y))
    -- local dot = stickDir.x * objForwardDir.x + stickDir.z * objForwardDir.z
    -- local angle = math.acos(dot)
    -- if (deltaY < 0) then
    --     angle = -angle
    -- end
    -- print("Angle between stick and object forward: " .. angle)
    -- physics:AddAngularImpulse(Vector3.new(0, angle * 50.0, 0))
    
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
end

-- Initialization function (called once when script loads)
function onInit()
    -- print("Player controller Lua script initialized!")
    -- print("Available functions: onMoveForward, onMoveBackward, onStrafeLeft, onStrafeRight")
    -- print("                    onJump, onCrouch, onInteract, onToggleSprint")
end

-- Call init
onInit()
