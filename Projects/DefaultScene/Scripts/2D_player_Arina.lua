-- 2D Platformer Player Controller
local moveForce = 800.0
local jumpForce = 2500.0
local lastJumpTime = 0
local jumpCooldown = 3
local moveDirection = 0

-- Move left
function onMoveLeft(action)
    if not player then
        return
    end
    
    local physics = player:getPhysics()
    
    if action == "down" then
        local left = Vector3.new(-moveForce, 0, 0)
        physics:addImpulse(left)
    end
     
    if action == "pressed" then
        moveDirection = 0
        physics:setRotation(Vector3.new(0,3.14159265,0))
    end
end

-- Move right
function onMoveRight(action)
    if not player then
        return
    end
    
    local physics = player:getPhysics()
    
    if action == "pressed" then
        moveDirection = 0
        physics:setRotation(Vector3.new(0,0,0))
    end
    if action == "down" then
        local right = Vector3.new(moveForce, 0, 0)
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


-- Mouse/Look callback
function onLookAround(deltaX, deltaY, wheelDelta)
    -- if not player then
    --     print("Error: player object not available")
    --     return
    -- end

    -- local camera = player:getCameraComponent():getCamera()

    -- Apply mouse sensitivity
    -- local deltaTime = camera.deltaTime
    -- local deltaTime = camera.deltaTime
    -- local sensitivity = 1
    -- local yawDelta = deltaX * sensitivity * deltaTime
    -- local pitchDelta = deltaY * sensitivity * deltaTime
    -- 
    -- -- Handle mouse wheel for zooming (adjust reference length)
    -- if wheelDelta ~= 0 then
    --     local currentLen = camera:getStickLength()
    --     local zoomAmount = wheelDelta * deltaTime
    --     local newLen = currentLen - zoomAmount
    --     -- Clamp zoom distance between 1 and 100 units
    --     if newLen >= 0.0 and newLen <= 100.0 then
    --         camera:setStickLength(newLen)
    --     end
    -- end
end

-- Initialization function (called once when script loads)
function onInit()
    print("2D Platformer Player Controller initialized!")
    print("Controls: Move Left/Right to move, Jump to jump")
end

-- Call init
onInit()
