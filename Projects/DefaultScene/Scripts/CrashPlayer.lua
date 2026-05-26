-- 2D Platformer Player Controller
local moveForce = 500.0

local behObj
local behUUID

local moveX = 0.0
local moveZ = 0.0

local pressedW = false
local pressedA = false
local pressedS = false
local pressedD = false

-- Movement callback functions
function onMoveForward(action)
    if not player then
        print("Error: player object not available")
        return
    end
      
    if action == "down" then
        pressedW = true
        
        if not behObj then
            behObj = getGameObjectByUUID(behUUID)
        end
        local bc = behObj:getBehavior()
        
        if pressedS then
            moveZ = 0.0
            bc:MB_setFloat("moveZ", moveZ)
            return
        end
        
        moveZ = 1.0
        bc:MB_setFloat("moveZ", moveZ)
        
        -- print("Moving forward...")
        local camForward = player:getCameraComponent():getCamera().forward
        
        Vector3.normalize(camForward)
        
        local forward = Vector3.new(camForward.x * moveForce, 0, camForward.z * moveForce)
        local physics = player:getPhysics()
        physics:addImpulse(forward)
    elseif action == "up" then
        pressedW = false

        if not behObj then
            behObj = getGameObjectByUUID(behUUID)
        end
        local bc = behObj:getBehavior()

        if pressedS then
            moveZ = -1.0
        else
            moveZ = -0.0
        end

        bc:MB_setFloat("moveZ", moveZ)
    end
end

function onMoveBackward(action)
    if not player then
        -- print("Error: player object not available")
        return
    end
      
    if action == "down" then
        pressedS = true
        
        if not behObj then
            behObj = getGameObjectByUUID(behUUID)
        end
        local bc = behObj:getBehavior()
        
        if pressedW then
            moveZ = 0.0
            bc:MB_setFloat("moveZ", moveZ)
            return
        end
        
        moveZ = -1.0
        bc:MB_setFloat("moveZ", moveZ)
        
        -- print("Moving forward...")
        local camForward = player:getCameraComponent():getCamera().forward
        
        Vector3.normalize(camForward)
        
        local forward = Vector3.new(-camForward.x * moveForce, 0, -camForward.z * moveForce)
        local physics = player:getPhysics()
        physics:addImpulse(forward)
    elseif action == "up" then
        pressedS = false
        
        if not behObj then
            behObj = getGameObjectByUUID(behUUID)
        end
        local bc = behObj:getBehavior()

        if pressedW then
            moveZ = 1.0
        else
            moveZ = 0.0
        end

        bc:MB_setFloat("moveZ", moveZ)
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
    
    
    if action == "down" then
        if not behObj then
            behObj = getGameObjectByUUID(behUUID)
        end
        local bc = behObj:getBehavior()

        if bc:MB_getBool("isGrounded") then
            bc:MB_setBool("isGrounded", false)
            bc:MB_setFloat("velocityY", 5.0)
            
            local physics = player:getPhysics()

            local velocity = physics:getLinearVelocity()
            physics:setLinearVelocity(Vector3.new(velocity.x, velocity.y + 5.0, velocity.z))
        end
    end
end

-- Interaction callback
function onAtack(action)
    if not player then
        -- print("Error: player object not available")
        return
    end

    if action == "down" then
        if not behObj then
            behObj = getGameObjectByUUID(behUUID)
        end
        local bc = behObj:getBehavior()
        bc:MB_setBool("attackPressed", true)
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


    if not behObj then
        behObj = getGameObjectByUUID(behUUID)
    end
    local bc = behObj:getBehavior()
    if not (bc:S_getCurrent() == "Attack") then
        local rot = camera:getSpringArmRotation()
        local physics = player:getPhysics()
        physics:setRotation(Vector3.new(0, rot.y, 0))
    end


    -- Handle mouse wheel for zooming (adjust reference length)
    if wheelDelta ~= 0 then
        local currentLen = camera:getSpringArmLength()
        local zoomAmount = wheelDelta * deltaTime
        local newLen = currentLen - zoomAmount
        -- Clamp zoom distance between 1 and 100 units
        if newLen >= 0.0 and newLen <= 100.0 then
            camera:setSpringArmLength(newLen)
        end
    end
end

-- Initialization function (called once when script loads)
function onInit()
    print("2D Platformer Player Controller initialized!")
    print("Controls: Move Left/Right to move, Jump to jump")

    behUUID = UUID.new()
    behUUID.hi = 3821726256
    behUUID.lo = 1576604890
end

-- Call init
onInit()
