-- 2D Platformer Player Controller
local moveForce = 800.0
local jumpForce = 2500.0
local lastJumpTime = 0
local jumpCooldown = 3
local moveDirection = 0
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

-- Jump
function onJump(action)
    if not player then
        return
    end
    
    local physics = player:getPhysics()
    
    if action == "down" then
        local currentTime = os.clock()
        
        if currentTime - lastJumpTime > jumpCooldown then
            lastJumpTime = currentTime
            local jump = Vector3.new(0, jumpForce * 10, 0)
            physics:addImpulse(jump)
            print("Jump!")
        end
    end
end

-- Interaction callback
function onUpGravity(action)
    if not player then
        -- print("Error: player object not available")
        return
    end

    if action == "pressed" then
        setGlobalGravity(Vector3.new(0,9,0))
    end
    
    if action == "up" then
        setGlobalGravity(Vector3.new(0,-9.8,0))
    end
end

-- Mouse/Look callback
function onLookAround(deltaX, deltaY, wheelDelta)
    if not player then
        print("Error: player object not available")
        return
    end

    local camera = player:getCameraComponent():getCamera()

    -- Apply mouse sensitivity
    local deltaTime = camera.deltaTime
    local sensitivity = 1
    local yawDelta = deltaX * sensitivity
    local pitchDelta = deltaY * sensitivity
    
    camera:rotateSpringArmYawPitch(yawDelta, pitchDelta)

    -- print("yawDelta: " .. yawDelta .. ", pitchDelta: " .. pitchDelta)
    
    -- local physics = player:getPhysics()
    -- local rotation = physics:getRotation()
    -- local stickDir = camera:getStickDirection()
    -- stickDir.y = 0
    -- local stickDirLen = stickDir:Length()
    -- -- stickDir.x = stickDir.x / stickDirLen
    -- -- stickDir.z = stickDir.z / stickDirLen
    -- physics:setRotation(Vector3.new(rotation.x, math.atan(stickDir.z, stickDir.x), rotation.z))

    -- camera:setStickDirection()

    -- Handle mouse wheel for zooming (adjust reference length)
    if wheelDelta ~= 0 then
        local currentLen = camera:getSpringArmLength()
        local zoomAmount = wheelDelta * deltaTime
        local newLen = currentLen - zoomAmount
        -- Clamp zoom distance between 1 and 100 units

        -- local rot = camera:getSpringArmRotation()
        local rot = camera:getSpringArmRootOffset()
        -- local rot = camera:getCameraRotation()
        local newRotZ = rot.y + 0.001 * wheelDelta
        -- camera:setSpringArmRotation(Vector3.new(rot.x, rot.y, newRotZ))
        -- camera:setCameraRotation(Vector3.new(rot.x, rot.y, newRotZ))
        camera:setSpringArmRootOffset(Vector3.new(rot.x, newRotZ, rot.z))
        
        -- if newLen >= 0.0 and newLen <= 100.0 then
        --     camera:setSpringArmLength(newLen)
        -- end
    end
end

-- Initialization function (called once when script loads)
function onInit()
    print("2D Platformer Player Controller initialized!")
    print("Controls: Move Left/Right to move, Jump to jump")
end

-- Call init
onInit()
