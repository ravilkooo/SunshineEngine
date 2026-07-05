behavior = {}

local playerUUID
local playerObj
local floatingSpeed = 4
local floatingAmp = 0.1
local yMidLevel

local function TestPerceptionSystem(self)
    local ps = getPerceptionSystem()

    local teamPlayer = 1
    local teamEnemy = 2
    local invalidTeam = 0xFFFFFFFF

    -- =========================
    -- REGISTER
    -- =========================

    ps:registerTeam(teamPlayer)
    ps:registerTeam(teamEnemy)
    ps:registerTeam(invalidTeam)
    
    ps:addSightTargetTeamIDsInTeam(teamEnemy, teamPlayer)
    local perception = self.owner:getPerception()
    ps:addToTeam(teamPlayer, perception)
end

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
    TestPerceptionSystem(self)
    
    local tr = self.owner:getTransform()
    yMidLevel = tr.localPosition.y
end

function behavior:update(dt)
    local tr = self.owner:getTransform()
    tr.localPosition = Vector3.new(tr.localPosition.x, yMidLevel + floatingAmp * math.sin(os.clock() * floatingSpeed), tr.localPosition.z)

    local inputSystem = getInputSystem()
    local speed = 0.1
    local char = self.owner:getCharacterComponent()

    if (char.m_isPlayerControlled) then
        local camera = self.owner:getCameraComponent()

        local inputValue = inputSystem:getAxis2D("Forward", "Right")
        char.m_moveInput = Vector2.new(inputValue.y, inputValue.x)
        camera:getCamera():rotateSpringArmYaw(0.4 * inputSystem:getMouseDeltaX())
        camera:getCamera():rotateSpringArmPitch(0.4 * inputSystem:getMouseDeltaY())

        local mouseWheelDelta = inputSystem:getMouseWheelDelta()
        if (mouseWheelDelta ~= 0) then
            camera:getCamera():zoomSpringArm(-1 * mouseWheelDelta)
        end

        if (inputSystem:isPressed("Jump")) then
            print("== JUMP! ==")
            char.m_jumpRequested = true
        end
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
