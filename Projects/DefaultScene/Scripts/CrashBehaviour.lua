behavior = {}

local playerUUID
local playerObj

-- =========================================================
-- HELPERS
-- =========================================================

local function isMoving(bc)
    local x = bc:MB_getFloat("moveX") or 0.0
    local z = bc:MB_getFloat("moveZ") or 0.0

    return math.abs(x) > 0.01 or math.abs(z) > 0.01
end

-- =========================================================
-- INIT
-- =========================================================

local function InitPlayerBehavior(self)

    local bc = self.owner:getBehavior()

    if not bc then
        print("[ERROR] BehaviorController not found")
        return
    end

    -- =====================================================
    -- MEMORYBOARD INIT
    -- =====================================================

    bc:MB_setBool("isGrounded", true)

    bc:MB_setBool("jumpPressed", false)
    bc:MB_setBool("attackPressed", false)

    bc:MB_setFloat("moveX", 0.0)
    bc:MB_setFloat("moveZ", 0.0)

    -- =====================================================
    -- STATES
    -- =====================================================

    bc:S_add("Grounded")
    bc:S_add("Airborne")

    bc:S_setInitial("Grounded")

    -- =====================================================
    -- STATE TRANSITIONS
    -- =====================================================

    -- Grounded -> Airborne
    bc:CT_add("Grounded", "Airborne",
        function(goid, bc)

            local grounded = bc:MB_getBool("isGrounded")

            return grounded == false
        end)

    -- Airborne -> Grounded
    bc:CT_add("Airborne", "Grounded",
        function(goid, bc)

            local grounded = bc:MB_getBool("isGrounded")

            return grounded == true
        end)

    -- =====================================================
    -- GROUNDED PATTERNS
    -- =====================================================

    bc:P_add("Grounded", "Idle")
    bc:P_add("Grounded", "Move")
    bc:P_add("Grounded", "Attack")

    -- =====================================================
    -- IDLE PATTERN
    -- =====================================================

    bc:P_setEvaluateUtility("Grounded", "Idle",
        function(goid, bc)

            local moving = isMoving(bc)
            local attacking = bc:MB_getBool("attackPressed")

            if not moving and not attacking then
                return 1
            end

            return 0
        end)

    bc:P_setOnStart("Grounded", "Idle",
        function(goid, bc)

            -- print("[Idle] Start")

            -- play animation
            -- self.owner:playAnimation("idle")
        end)

    -- =====================================================
    -- MOVE PATTERN
    -- =====================================================

    bc:P_setEvaluateUtility("Grounded", "Move",
        function(goid, bc)

            local moving = isMoving(bc)
            local attacking = bc:MB_getBool("attackPressed")

            if moving and not attacking then
                return 10
            end

            return 0
        end)

    bc:P_setOnStart("Grounded", "Move",
        function(goid, bc)

            print("[Move] Start")

            -- play animation
            -- self.owner:playAnimation("run")
        end)

    bc:P_setOnUpdate("Grounded", "Move",
        function(goid, bc, dt)

            local x = bc:MB_getFloat("moveX") or 0.0
            local z = bc:MB_getFloat("moveZ") or 0.0

            -- movement code
            -- self.owner:addMovement(...)

            print("[Move] x =", x, "z =", z)
        end)

    -- =====================================================
    -- ATTACK PATTERN
    -- =====================================================

    bc:P_setEvaluateUtility("Grounded", "Attack",
        function(goid, bc)

            local attackPressed = bc:MB_getBool("attackPressed")

            if attackPressed then
                return 100
            end

            return 0
        end)

    bc:P_setOnStart("Grounded", "Attack",
        function(goid, bc)

            print("[Attack] Pattern Start")

            -- play animation
            -- self.owner:playAnimation("attack")
        end)

    bc:P_setOnComplete("Grounded", "Attack",
        function(goid, bc, result)

            print("[Attack] Pattern Complete")

            bc:MB_setBool("attackPressed", false)

            local camera = playerObj:getCameraComponent():getCamera()
            local rot = camera:getSpringArmRotation()
            local physics = playerObj:getPhysics()
            physics:setRotation(Vector3.new(0, rot.y, 0))
        end)

    bc:P_setOnUpdate("Grounded", "Attack",
        function(goid, bc, dt)

            local camera = playerObj:getCameraComponent():getCamera()
            local rot = camera:getSpringArmRotation()
            local physics = playerObj:getPhysics()
            
            local currentTime = os.clock()
            local rotSpeed = 30

            physics:setRotation(Vector3.new(0, rot.y + currentTime * rotSpeed, 0))
        end)

    -- =====================================================
    -- ATTACK ACTIONS
    -- =====================================================

    bc:A_add("Grounded", "Attack", "Windup")
    bc:A_add("Grounded", "Attack", "Hit")
    bc:A_add("Grounded", "Attack", "Recovery")

    -- =====================================================
    -- WINDUP
    -- =====================================================

    bc:A_setOnStart("Grounded", "Attack", "Windup",
        function(goid, bc)

            print("[Attack] Windup Start")

            bc:MB_setFloat("attackTimer", 0.0)
        end)

    bc:A_setOnUpdate("Grounded", "Attack", "Windup",
        function(goid, bc, dt)

            local t = bc:MB_getFloat("attackTimer") or 0.0

            t = t + dt

            bc:MB_setFloat("attackTimer", t)

            if t >= 0.2 then
                return 1 -- succeeded
            end

            return 0 -- running
        end)

    -- =====================================================
    -- HIT
    -- =====================================================

    bc:A_setOnStart("Grounded", "Attack", "Hit",
        function(goid, bc)

            print("[Attack] HIT")

            -- spawn hitbox
            -- damage enemies
        end)

    bc:A_setOnUpdate("Grounded", "Attack", "Hit",
        function(goid, bc, dt)

            return 1
        end)

    -- =====================================================
    -- RECOVERY
    -- =====================================================

    bc:A_setOnStart("Grounded", "Attack", "Recovery",
        function(goid, bc)

            print("[Attack] Recovery Start")

            bc:MB_setFloat("recoveryTimer", 0.0)
        end)

    bc:A_setOnUpdate("Grounded", "Attack", "Recovery",
        function(goid, bc, dt)

            local t = bc:MB_getFloat("recoveryTimer") or 0.0

            t = t + dt

            bc:MB_setFloat("recoveryTimer", t)

            if t >= 0.3 then
                return 1
            end

            return 0
        end)

    -- =====================================================
    -- AIRBORNE PATTERNS
    -- =====================================================

    bc:P_add("Airborne", "Jump")
    bc:P_add("Airborne", "Fall")

    -- =====================================================
    -- JUMP PATTERN
    -- =====================================================

    bc:P_setEvaluateUtility("Airborne", "Jump",
        function(goid, bc)

            local velocityY = bc:MB_getFloat("velocityY") or 0.0

            if velocityY > 0.0 then
                return 10
            end

            return 0
        end)

    bc:P_setOnStart("Airborne", "Jump",
        function(goid, bc)

            print("[Jump] Start")

            -- play animation
            -- self.owner:playAnimation("jump")
        end)

    -- =====================================================
    -- FALL PATTERN
    -- =====================================================

    bc:P_setEvaluateUtility("Airborne", "Fall",
        function(goid, bc)

            local velocityY = bc:MB_getFloat("velocityY") or 0.0

            if velocityY < 0.0 then
                return 10
            end

            return 0
        end)

    bc:P_setOnStart("Airborne", "Fall",
        function(goid, bc)

            print("[Fall] Start")

            -- play animation
            -- self.owner:playAnimation("fall")
        end)

    print("[PlayerBehavior] Initialized")
end

-- =========================================================
-- START
-- =========================================================

function behavior:start()
    playerObj = getPlayerObject()
    playerUUID = playerObj:getUUID()

    InitPlayerBehavior(self)
end

-- =========================================================
-- UPDATE
-- =========================================================

function behavior:update(dt)

    local bc = self.owner:getBehavior()

    if not bc then
        return
    end

    -- =====================================================
    -- INPUT
    -- =====================================================

    local physics = playerObj:getPhysics()
    local velocity = physics:getLinearVelocity()


    -- =====================================================
    -- ATTACK INPUT
    -- =====================================================

    -- if Input.isMousePressed(0) then
    --     bc:MB_setBool("attackPressed", true)
    -- end

    -- =====================================================
    -- JUMP INPUT
    -- =====================================================

    -- if Input.isKeyPressed(KEY_SPACE) then
    --
    --     if bc:MB_getBool("isGrounded") then
    --
    --         bc:MB_setBool("isGrounded", false)
    --
    --         bc:MB_setFloat("velocityY", 5.0)
    --     end
    -- end

    -- =====================================================
    -- SIMPLE GRAVITY EXAMPLE
    -- =====================================================

    local grounded = bc:MB_getBool("isGrounded")

    if not grounded then

        local velocityY = velocity.y
        bc:MB_setFloat("velocityY", velocityY)

        -- fake landing example

        if velocityY == 0.0 then
            bc:MB_setBool("isGrounded", true)
            physics:setLinearVelocity(Vector3.new(velocity.x, 0, velocity.z))
            bc:MB_setFloat("velocityY", 0.0)
        end
    end
end

-- =========================================================
-- DESTROY
-- =========================================================

function behavior:destroy()

end

return behavior