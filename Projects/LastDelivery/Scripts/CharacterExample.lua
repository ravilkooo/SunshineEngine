behavior = {}

local playerUUID
local playerObj
local floatingSpeed = 4
local floatingAmp = 0.1
local yMidLevel

local particlesUUID

controlPlayerMove = true

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
    print("isPlayerControlled = " .. (char.isPlayerControlled and "true" or "false"))
    print("isDead = " .. (char.isDead and "true" or "false"))
    print("isAttacking = " .. (char.isAttacking and "true" or "false"))
    print("moveInput = " .. char.moveInput.x .. ", " .. char.moveInput.y)
    print("jumpRequested = " .. (char.jumpRequested and "true" or "false"))
    print("attackRequested = " .. (char.attackRequested and "true" or "false"))
    print("yaw = " .. char.yaw)
    print("pitch = " .. char.pitch)

    local charContr = self.owner:getCharacterController()
    print("")
    print(" == Controller fields == ")
    print("velocity = " .. charContr.velocityVector.x .. ", " .. charContr.velocityVector.y .. ", " .. charContr.velocityVector.z)
    print("grounded = " .. (charContr.grounded and "true" or "false"))
    print("groundNormal = " .. charContr.groundNormal.x .. ", " .. charContr.groundNormal.y .. ", " .. charContr.groundNormal.z)
    print("moveSpeed = " .. charContr.moveSpeed)
    print("acceleration = " .. charContr.acceleration)
    print("airAcceleration = " .. charContr.airAcceleration)
    print("jumpSpeed = " .. charContr.jumpSpeed)
    print("gravity = " .. charContr.gravity)
    print("maxFallSpeed = " .. charContr.maxFallSpeed)
    print("enableStickToFloor = " .. (charContr.enableStickToFloor and "true" or "false"))
    print("stepHeight = " .. charContr.stepHeight)
    print("maxSlopeAngle = " .. charContr.maxSlopeAngle)

    local cam = self.owner:getCameraComponent():getCamera()
    print("")
    print(" == Controller fields == ")
    print("armLen = " .. cam.springArmLength)
    local armRot = cam.springArmRotation
    print("armRot = " .. armRot.x .. ", " .. armRot.y .. ", " .. armRot.z)
    local armOff = cam.springArmRootOffset
    print("armOff = " .. armOff.x .. ", " .. armOff.y .. ", " .. armOff.z)
    local camRot = cam.cameraRotation
    print("camRot = " .. camRot.x .. ", " .. camRot.y .. ", " .. camRot.z)



end

function behavior:start()
    TestPerceptionSystem(self)
    TestCharacterFields(self)
    
    local tr = self.owner:getTransform()
    yMidLevel = tr.localPosition.y
end

-- local frameNum = 0

function behavior:update(dt)
    local tr = self.owner:getTransform()
    local pos = tr.localPosition

    -- frameNum = frameNum + 1
    -- print("Frame #" .. frameNum)

    tr.localPosition = Vector3.new(
        pos.x,
        yMidLevel + floatingAmp * math.sin(os.clock() * floatingSpeed),
        pos.z
    )

    local speed = 0.1
    local char = self.owner:getCharacterComponent()

    if (char.isPlayerControlled) then
        local camera = self.owner:getCameraComponent():getCamera()

        local inputValue = InputSystem:getAxis2D("Right", "Forward")
        char.moveInput = inputValue
        camera:rotateSpringArmYaw(0.4 * InputSystem:getMouseDeltaX())
        camera:rotateSpringArmPitch(0.4 * InputSystem:getMouseDeltaY())

        local mouseWheelDelta = InputSystem:getMouseWheelDelta()
        if (mouseWheelDelta ~= 0) then
            camera:zoomSpringArm(-1 * mouseWheelDelta)
        end

        if (InputSystem:isPressed("Jump")) then
            print("== JUMP! ==")
            char.jumpRequested = true
        end

        local grab = self.owner:getGrabComponent()
        if (InputSystem:isPressed("Grab")) then
            if (grab:isGrabbing()) then
                print("== RELEASE! ==")
                char.releaseRequested = true
                disableCharacterParticles()
            else
                print("== GRAB! ==")
                char.grabRequested = true
                enableCharacterParticles()
            end
        end
        if (InputSystem:isReleased("Grab") and not grab:isGrabbing()) then
            disableCharacterParticles()
        end
        if (InputSystem:isPressed("Throw")) then
            if (grab:isGrabbing()) then
                print("== THROW! ==")
                char.throwRequested = true
                disableCharacterParticles()
            end
        end
    end
    -- print("char pitch = " .. char.pitch)
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
