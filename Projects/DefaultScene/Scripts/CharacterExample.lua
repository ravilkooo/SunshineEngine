behavior = {}

local playerUUID
local playerObj
local collected = false
local rotSpeed = 10
local radius = 3
local upOffset = 0.6
local scale = 0.1

-- can be made unique for every object
-- for example: based on initial position of object or time
local angleOffet = 0.2

local function TestCharacterFields(self)
    local char = self.owner:getCharacterComponent()

    print(" == Character fields == ")
    print("m_isPlayerControlled = " .. (char.m_isPlayerControlled and "true" or "false"))
    print("m_isDead = " .. (char.m_isDead and "true" or "false"))
    print("m_isAttacking = " .. (char.m_isAttacking and "true" or "false"))
    print("m_moveInput = " .. char.m_moveInput.x .. ", " .. char.m_moveInput.y)
    print("m_jumpRequested = " .. (char.m_jumpRequested and "true" or "false"))
    print("m_attackRequested = " .. (char.m_attackRequested and "true" or "false"))
    print("m_yaw = " .. char.m_yaw)
    print("m_pitch = " .. char.m_pitch)

    local charContr = self.owner:getCharacterController()
    print("")
    print(" == Controller fields == ")
    print("m_velocity = " .. charContr.m_velocity.x .. ", " .. charContr.m_velocity.y .. ", " .. charContr.m_velocity.z)
    print("m_grounded = " .. (charContr.m_grounded and "true" or "false"))
    print("m_groundNormal = " .. charContr.m_groundNormal.x .. ", " .. charContr.m_groundNormal.y .. ", " .. charContr.m_groundNormal.z)
    print("m_moveSpeed = " .. charContr.m_moveSpeed)
    print("m_acceleration = " .. charContr.m_acceleration)
    print("m_airAcceleration = " .. charContr.m_airAcceleration)
    print("m_jumpSpeed = " .. charContr.m_jumpSpeed)
    print("m_gravity = " .. charContr.m_gravity)
    print("m_maxFallSpeed = " .. charContr.m_maxFallSpeed)
    print("m_enableStickToFloor = " .. (charContr.m_enableStickToFloor and "true" or "false"))
    print("m_stepHeight = " .. charContr.m_stepHeight)
    print("m_maxSlopeAngle = " .. charContr.m_maxSlopeAngle)

    local cam = self.owner:getCameraComponent():getCamera()
    print("")
    print(" == Controller fields == ")
    print("armLen = " .. cam:getSpringArmLength())
    local armRot = cam:getSpringArmRotation()
    print("armRot = " .. armRot.x .. ", " .. armRot.y .. ", " .. armRot.z)
    local armOff = cam:getSpringArmRootOffset()
    print("armOff = " .. armOff.x .. ", " .. armOff.y .. ", " .. armOff.z)
    local camRot = cam:getCameraRotation()
    print("camRot = " .. camRot.x .. ", " .. camRot.y .. ", " .. camRot.z)



end

function behavior:start()
    
    playerUUID = UUID.new()
    playerUUID.hi = 253145895
    playerUUID.lo = 2320618671
    playerObj = getGameObjectByUUID(playerUUID)

    TestCharacterFields(self)
end

function behavior:update(dt)
    local tr = self.owner:getTransform()
    local inputSystem = getInputSystem()
    local speed = 0.1

    tr.m_rotation.y = tr.m_rotation.y + speed * inputSystem:getAxis("Forward")

    if (inputSystem:isPressed("RAction")) then
        -- print("RACTION")
        tr.m_rotation.y = tr.m_rotation.y + 0.5
    elseif (inputSystem:isReleased("RAction")) then
        -- print("RACTION")
        tr.m_rotation.y = tr.m_rotation.y - 0.5
    end

    if (inputSystem:isPressed("FAction")) then
        -- print("FACTION")
        tr.m_rotation.y = tr.m_rotation.y - 0.5
    end

    if not collected then

        -- local currentTime = os.clock()
        -- self.owner:getTransform().m_rotation = Vector3.new(0, rotSpeed * currentTime, 0)
        
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
