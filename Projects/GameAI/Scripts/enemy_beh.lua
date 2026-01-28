behavior = {}

local nilUUID
local playerObj
local playerUUID
local selfUUID

-- Read from MemoryBoard
local function MBf(bc, key, def)
    local v = bc:MB_getFloat(key)
    return v or def
end

local function MBi(bc, key, def)
    local v = bc:MB_getInt(key)
    return v or def
end

local function MBb(bc, key, def)
    local v = bc:MB_getBool(key)
    if v == nil then return def end
    return v
end

local function MBv(bc, key)
    return bc:MB_getVector3(key)
end

local function getForward(obj)
    local objRot = obj:getTransform().m_rotation
    return Vector3.new(math.sin(objRot.x), 0, math.cos(objRot.x))
end

local function sum(v1, v2)
    return Vector3.new(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z)
end

local function sub(v1, v2)
    return Vector3.new(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z)
end

local function dotVector(v, s)
    return Vector3.new(v.x * s, v.y * s, v.z * s)
end



function rotateToObjDirection(self, obj)
    local playerPos = obj:getTransform():getAbsolutePosition()
    
    local tf = self.owner:getTransform()
    local enemyPos = tf:getAbsolutePosition()
    local enemyYaw = self.owner:getTransform().m_rotation.y

    -- Direction on XZ plane
    local dx = playerPos.x - enemyPos.x
    local dz = playerPos.z - enemyPos.z

    -- Engine rotations are Vector3, interpret Y as yaw in radians
    local newYaw = math.atan(dx, dz)

    local tf = self.owner:getTransform()

    self.owner:getTransform().m_rotation.y = newYaw
end

--------------------------------------------------
--  Init MemoryBoard keys
--------------------------------------------------
local function InitMemoryBoard(self, bc)
    nilUUID = UUID.new()
    nilUUID.hi = 0
    nilUUID.lo = 0

    -- Эти ключи должны обновляться игрой / perception-системой
    bc:MB_setUUID("targetID", nilUUID)
    bc:MB_setVector3("waypointPosition", Vector3.new(0,0,0))
    bc:MB_setVector3("targetPosition", Vector3.new(0,0,0))
    bc:MB_setVector3("lastSeenPosition", Vector3.new(0,0,0))
    bc:MB_setVector3("lastHeardNoise", Vector3.new(0,0,0))
    bc:MB_setFloat("healthPercent", 100.0)
    bc:MB_setFloat("timeSinceLastSeen", 999.0)
    bc:MB_setFloat("timeSinceHeardNoise", 999.0)
    bc:MB_setFloat("distanceToTarget", 999.0)
    bc:MB_setBool("isTargetBehindCover", false)
    bc:MB_setBool("isTargetFacingMe", false)
    bc:MB_setInt("ammoCount", 30)
    bc:MB_setBool("grenadeReady", false)
    bc:MB_setVector3("safeSpotPosition", Vector3.new(0,0,0))
    bc:MB_setFloat("pressureLevel", 0.0)      -- 0..1
    bc:MB_setFloat("uncertaintyLevel", 0.0)   -- 0..1

    print("Initialized MemoryBoard for NPC Soldier")
end

local function clamp(x, a, b)
    if x < a then return a end
    if x > b then return b end
    return x
end

--------------------------------------------------
--  STATES
--  Patrol, Hunt, Combat, Retreat, Idle
--------------------------------------------------
local function SetupStates(self, bc)
    bc:S_add("Patrol")
    bc:S_add("Hunt")
    bc:S_add("Combat")
    bc:S_add("Retreat")
    bc:S_add("Idle")
    bc:S_setInitial("Patrol")
    
    -- Add state entry callbacks for debugging
    bc:S_setOnEnter("Patrol", function(goid, bc)
        print("[STATE] === ENTERING PATROL ===")
    end)
    bc:S_setOnEnter("Hunt", function(goid, bc)
        print("[STATE] === ENTERING HUNT ===")
    end)
    bc:S_setOnEnter("Combat", function(goid, bc)
        print("[STATE] === ENTERING COMBAT ===")
    end)
    bc:S_setOnEnter("Retreat", function(goid, bc)
        print("[STATE] === ENTERING RETREAT ===")
    end)
    bc:S_setOnEnter("Idle", function(goid, bc)
        print("[STATE] === ENTERING IDLE ===")
    end)
    
    print("Initialized States")
end

--------------------------------------------------
--  CONDITIONAL TRANSITIONS (FSM)
--------------------------------------------------
local function SetupTransitions(self, bc)
    ------------------------------------------------
    -- Event-driven реализуем через MemoryBoard-флаги
    -- События: OnNoiseHeard, OnSeeEnemy, WasAttacked
    -- Предполагаем, что gameplay-код выставляет эти флаги и потом сбрасывает
    ------------------------------------------------

    -- Patrol -> Hunt  (OnNoiseHeard)
    bc:CT_add("Patrol", "Hunt", function(goid, bc)
        local result = MBb(bc, "event_OnNoiseHeard", false) == true
        if result then print("[TRANSITION] Patrol->Hunt triggered (OnNoiseHeard)") end
        return result
    end)

    -- Patrol -> Combat (OnSeeEnemy, WasAttacked)
    bc:CT_add("Patrol", "Combat", function(goid, bc)
        local onSee = MBb(bc, "event_OnSeeEnemy", false)
        local wasAttacked = MBb(bc, "event_WasAttacked", false)
        local result = onSee or wasAttacked
        if result then print("[TRANSITION] Patrol->Combat triggered (OnSeeEnemy=" .. tostring(onSee) .. " WasAttacked=" .. tostring(wasAttacked) .. ")") end
        return result
    end)

    -- Hunt -> Combat (OnSeeEnemy, WasAttacked)
    bc:CT_add("Hunt", "Combat", function(goid, bc)
        local onSee = MBb(bc, "event_OnSeeEnemy", false)
        local wasAttacked = MBb(bc, "event_WasAttacked", false)
        local result = onSee or wasAttacked
        if result then print("[TRANSITION] Hunt->Combat triggered (OnSeeEnemy=" .. tostring(onSee) .. " WasAttacked=" .. tostring(wasAttacked) .. ")") end
        return result
    end)

    -- Hunt -> Patrol  (IF timeSinceHeardNoise > 15)
    bc:CT_add("Hunt", "Patrol", function(goid, bc)
        local t = MBf(bc, "timeSinceHeardNoise", 999.0)
        local result = t > 15.0
        if result then print("[TRANSITION] Hunt->Patrol triggered (timeSinceHeardNoise=" .. t .. ")") end
        return result
    end)

    -- Combat -> Retreat (IF healthPercent < 35)
    bc:CT_add("Combat", "Retreat", function(goid, bc)
        local hp = MBf(bc, "healthPercent", 100.0)
        local result = hp < 35.0
        if result then print("[TRANSITION] Combat->Retreat triggered (healthPercent=" .. hp .. ")") end
        return result
    end)

    -- Combat -> Hunt (IF timeSinceLastSeen > 10)
    bc:CT_add("Combat", "Hunt", function(goid, bc)
        local t = MBf(bc, "timeSinceLastSeen", 999.0)
        local result = t > 10.0
        if result then print("[TRANSITION] Combat->Hunt triggered (timeSinceLastSeen=" .. t .. ")") end
        return result
    end)

    -- Retreat -> Combat (IF healthPercent >= 70 AND targetID != null)
    bc:CT_add("Retreat", "Combat", function(goid, bc)
        local hp = MBf(bc, "healthPercent", 100.0)
        local tid = bc:MB_getUUID("targetID")
        local hasTarget = tid and not tid.isEqual(nilUUID) or false
        local result = hp >= 70.0 and hasTarget
        if result then print("[TRANSITION] Retreat->Combat triggered (hp=" .. hp .. " hasTarget=" .. tostring(hasTarget) .. ")") end
        return result
    end)

    -- Retreat -> Patrol (IF healthPercent >= 70 AND targetID == null)
    bc:CT_add("Retreat", "Patrol", function(goid, bc)
        local hp = MBf(bc, "healthPercent", 100.0)
        local tid = bc:MB_getUUID("targetID")
        local hasTarget = tid and not tid.isEqual(nilUUID) or false
        local result = hp >= 70.0 and not hasTarget
        if result then print("[TRANSITION] Retreat->Patrol triggered (hp=" .. hp .. " hasTarget=" .. tostring(hasTarget) .. ")") end
        return result
    end)

    -- Idle: переходы можно настроить по своему (например, если есть цель -> Combat)
    bc:CT_add("Idle", "Patrol", function(goid, bc)
        -- когда ничего не происходит, NPC начинает патрулировать
        print("[TRANSITION] Idle->Patrol triggered (auto transition)")
        return true
    end)

    print("Initialized Transitions")
end

--------------------------------------------------
--  PATROL STATE
--  WalkRoute, IdleLookAround, SuspiciousScan
--------------------------------------------------
local function SetupPatrol(self, bc)
    ------------------------------------------------
    -- Pattern: WalkRoute
    -- Actions: PickNextWaypoint, MoveTo(waypointPosition, slow)
    ------------------------------------------------
    bc:P_add("Patrol", "WalkRoute")

    -- Utility:
    -- calm = 1.0 - pressureLevel
    -- curious = Clamp(1.0 - uncertaintyLevel, 0.2,1.0)
    -- U = 0.4 * calm * curious
    bc:P_setEvaluateUtility("Patrol", "WalkRoute",
        function(goid, bc)
            local pressure = MBf(bc, "pressureLevel", 0.0)
            local uncertainty = MBf(bc, "uncertaintyLevel", 0.0)

            local calm = 1.0 - pressure
            local curious = clamp(1.0 - uncertainty, 0.2, 1.0)
            local U = 0.4 * calm * curious
            print("[PATROL] WalkRoute utility: " .. U .. " (pressure=" .. pressure .. " uncertainty=" .. uncertainty .. ")")
            return U
        end)

    -- Action 1: PickNextWaypoint (через callback)
    bc:A_add("Patrol", "WalkRoute", "PickNextWaypoint")
    bc:A_setOnUpdate("Patrol", "WalkRoute", "PickNextWaypoint",
        function(goid, bc, dt)
            -- Примитивная функция: просим движковый NPC-компонент выбрать следующую точку
            -- Предполагается, что C++/Lua код владельца сделает это и запишет waypointPosition
            if self.owner.pickNextWaypoint then
                local wp = self.owner:pickNextWaypoint()
                if wp then
                    bc:MB_setVector3("waypointPosition", wp)
                end
            end
            return 1 -- Succeeded
        end)

    -- Action 2: MoveToWaypoint(slow)
    bc:A_add("Patrol", "WalkRoute", "MoveToWaypoint")
    bc:A_setOnUpdate("Patrol", "WalkRoute", "MoveToWaypoint",
        function(goid, bc, dt)
            local wp = MBv(bc, "waypointPosition")
            if not wp then return 2 end -- Fail, нет точки
            if self.owner.moveTo then
                local done = self.owner:moveTo(wp, "slow")
                if done then
                    return 1 -- arrived
                else
                    return 0 -- running
                end
            end
            return 2
        end)

    ------------------------------------------------
    -- Pattern: IdleLookAround
    -- Actions: Stop(), LookAround(Random(3,5))
    ------------------------------------------------
    bc:P_add("Patrol", "IdleLookAround")
    -- Utility:
    -- alert = Clamp(uncertaintyLevel + pressureLevel, 0.0,1.0)
    -- U = 0.2 + 0.6 * alert
    bc:P_setEvaluateUtility("Patrol", "IdleLookAround",
        function(goid, bc)
            local pressure = MBf(bc, "pressureLevel", 0.0)
            local uncertainty = MBf(bc, "uncertaintyLevel", 0.0)
            local alert = clamp(uncertainty + pressure, 0.0, 1.0)
            local U = 0.2 + 0.6 * alert
            print("[PATROL] IdleLookAround utility: " .. U .. " (alert=" .. alert .. ")")
            return U
        end)

    bc:A_add("Patrol", "IdleLookAround", "Stop")
    bc:A_setOnUpdate("Patrol", "IdleLookAround", "Stop",
        function(goid, bc, dt)
            if self.owner.stopMovement then
                self.owner:stopMovement()
            end
            return 1
        end)

    bc:A_add("Patrol", "IdleLookAround", "LookAround")
    bc:A_setOnStart("Patrol", "IdleLookAround", "LookAround",
        function(goid, bc)
            local dur = math.random(3, 5)
            bc:MB_setFloat("idleLookDuration", dur)
            bc:MB_setFloat("idleLookTimer", 0.0)
        end)
    bc:A_setOnUpdate("Patrol", "IdleLookAround", "LookAround",
        function(goid, bc, dt)
            local t = MBf(bc, "idleLookTimer", 0.0)
            local d = MBf(bc, "idleLookDuration", 3.0)
            t = t + dt
            bc:MB_setFloat("idleLookTimer", t)

            if self.owner.lookAround then
                self.owner:lookAround()
            end

            if t >= d then
                return 1
            end
            return 0
        end)

    ------------------------------------------------
    -- Pattern: SuspiciousScan
    -- Actions: TurnTo(lastHeardNoise), Scan(Random(2,7))
    ------------------------------------------------
    bc:P_add("Patrol", "SuspiciousScan")
    -- Utility:
    -- if lastHeardNoise == null or timeSinceHeardNoise > 10 -> U=0
    -- fresh = Clamp(1.0 - timeSinceHeardNoise/5.0, 0.0,1.0)
    -- U = Clamp(0.4 + 0.6 * max(fresh, uncertaintyLevel), 0.0,1.0)
    bc:P_setEvaluateUtility("Patrol", "SuspiciousScan",
        function(goid, bc)
            local hasNoise = MBv(bc, "lastHeardNoise")
            local timeNoise = MBf(bc, "timeSinceHeardNoise", 999.0)
            if (not hasNoise) or (timeNoise > 10.0) then
                print("[PATROL] SuspiciousScan utility: 0.0 (no noise or too old)")
                return 0.0
            end

            local uncertainty = MBf(bc, "uncertaintyLevel", 0.0)
            local fresh = clamp(1.0 - timeNoise / 5.0, 0.0, 1.0)
            local U = clamp(0.4 + 0.6 * math.max(fresh, uncertainty), 0.0, 1.0)
            print("[PATROL] SuspiciousScan utility: " .. U .. " (fresh=" .. fresh .. " uncertainty=" .. uncertainty .. ")")
            return U
        end)

    bc:A_add("Patrol", "SuspiciousScan", "TurnToNoise")
    bc:A_setOnUpdate("Patrol", "SuspiciousScan", "TurnToNoise",
        function(goid, bc, dt)
            local pos = MBv(bc, "lastHeardNoise")
            if not pos then return 2 end
            if self.owner.turnTo then
                self.owner:turnTo(pos)
            end
            return 1
        end)

    bc:A_add("Patrol", "SuspiciousScan", "Scan")
    bc:A_setOnStart("Patrol", "SuspiciousScan", "Scan",
        function(goid, bc)
            local dur = math.random(2, 7)
            bc:MB_setFloat("scanDuration", dur)
            bc:MB_setFloat("scanTimer", 0.0)
        end)
    bc:A_setOnUpdate("Patrol", "SuspiciousScan", "Scan",
        function(goid, bc, dt)
            local t = MBf(bc, "scanTimer", 0.0)
            local d = MBf(bc, "scanDuration", 2.0)
            t = t + dt
            bc:MB_setFloat("scanTimer", t)

            if self.owner.scanForTargets then
                self.owner:scanForTargets()
            end

            if t >= d then
                return 1
            end
            return 0
        end)

    print("Initialized Patrol State")
end

--------------------------------------------------
--  HUNT STATE
--  MoveToNoise, SpiralSearch, PredictiveFlank
--------------------------------------------------
local function SetupHunt(self, bc)
    ------------------------------------------------
    -- Pattern: MoveToNoise
    -- Actions: MoveTo(lastHeardNoise, slow)
    ------------------------------------------------
    bc:P_add("Hunt", "MoveToNoise")

    -- Utility:
    -- sound = Clamp(1.0 - timeSinceHeardNoise/10.0, 0.0,1.0)
    -- U = Clamp(0.5*uncertaintyLevel + 0.5*sound, 0.0,1.0)
    bc:P_setEvaluateUtility("Hunt", "MoveToNoise",
        function(goid, bc)
            local t = MBf(bc, "timeSinceHeardNoise", 999.0)
            local sound = clamp(1.0 - t / 10.0, 0.0, 1.0)
            local uncertainty = MBf(bc, "uncertaintyLevel", 0.0)
            local U = clamp(0.5 * uncertainty + 0.5 * sound, 0.0, 1.0)
            print("[HUNT] MoveToNoise utility: " .. U .. " (sound=" .. sound .. " uncertainty=" .. uncertainty .. ")")
            return U
        end)

    bc:A_add("Hunt", "MoveToNoise", "MoveToNoisePos")
    bc:A_setOnUpdate("Hunt", "MoveToNoise", "MoveToNoisePos",
        function(goid, bc, dt)
            local pos = MBv(bc, "lastHeardNoise")
            if not pos then return 2 end
            if self.owner.moveTo then
                local done = self.owner:moveTo(pos, "slow")
                return done and 1 or 0
            end
            return 2
        end)

    ------------------------------------------------
    -- Pattern: SpiralSearch(lastHeardNoise, radius=15)
    ------------------------------------------------
    bc:P_add("Hunt", "SpiralSearch")

    -- U = 0.3 + 0.6 * Clamp(timeSinceHeardNoise/15.0, 0.0,1.0)
    bc:P_setEvaluateUtility("Hunt", "SpiralSearch",
        function(goid, bc)
            local t = MBf(bc, "timeSinceHeardNoise", 999.0)
            local U = 0.3 + 0.6 * clamp(t / 15.0, 0.0, 1.0)
            print("[HUNT] SpiralSearch utility: " .. U .. " (timeSinceNoise=" .. t .. ")")
            return U
        end)

    bc:A_add("Hunt", "SpiralSearch", "Spiral")
    bc:A_setOnUpdate("Hunt", "SpiralSearch", "Spiral",
        function(goid, bc, dt)
            local center = MBv(bc, "lastHeardNoise")
            if not center then return 2 end
            if self.owner.spiralSearch then
                local done = self.owner:spiralSearch(center, 15.0)
                return done and 1 or 0
            end
            return 2
        end)

    ------------------------------------------------
    -- Pattern: PredictiveFlank
    -- Actions: MoveTo(PredictEnemyEscapeVector(), slow), LookAlongEscapeRoute()
    ------------------------------------------------
    bc:P_add("Hunt", "PredictiveFlank")

    -- U = Clamp(0.2 + 0.7*(1.0 - uncertaintyLevel), 0.0,1.0)
    bc:P_setEvaluateUtility("Hunt", "PredictiveFlank",
        function(goid, bc)
            local uncertainty = MBf(bc, "uncertaintyLevel", 0.0)
            local U = clamp(0.2 + 0.7 * (1.0 - uncertainty), 0.0, 1.0)
            print("[HUNT] PredictiveFlank utility: " .. U .. " (uncertainty=" .. uncertainty .. ")")
            return U
        end)

    bc:A_add("Hunt", "PredictiveFlank", "MoveToPredicted")
    bc:A_setOnUpdate("Hunt", "PredictiveFlank", "MoveToPredicted",
        function(goid, bc, dt)
            if not self.owner.predictEnemyEscapeVector then return 2 end
            local pos = self.owner:predictEnemyEscapeVector()
            if not pos then return 2 end
            bc:MB_setVector3("predictedEscapePos", pos)
            if self.owner.moveTo then
                local done = self.owner:moveTo(pos, "slow")
                return done and 1 or 0
            end
            return 2
        end)

    bc:A_add("Hunt", "PredictiveFlank", "LookAlongRoute")
    bc:A_setOnUpdate("Hunt", "PredictiveFlank", "LookAlongRoute",
        function(goid, bc, dt)
            if self.owner.lookAlongEscapeRoute then
                self.owner:lookAlongEscapeRoute()
            end
            return 1
        end)

    print("Initialized Hunt State")
end

--------------------------------------------------
--  COMBAT STATE
--  ChaseTarget, StrafeShoot, GrenadeAndCover,
--  SuppressAndAdvance, RepositionForAngle
--------------------------------------------------
local function SetupCombat(self, bc)
    ------------------------------------------------
    -- Pattern: ChaseTarget
    -- Actions: MoveTo(targetPosition, fast)
    ------------------------------------------------
    bc:P_add("Combat", "ChaseTarget")

    -- If targetID == null -> U=0
    -- dfactor = Clamp(1.0 - distanceToTarget/20.0, 0.0,1.0)
    -- U = Clamp(0.4 + 0.6 * max(dfactor,pressureLevel), 0.0,1.0)
    bc:P_setEvaluateUtility("Combat", "ChaseTarget",
        function(goid, bc)
            local tid = bc:MB_getUUID("targetID")
            if not tid then 
                print("[COMBAT] ChaseTarget utility: 0.0 (no target)")
                return 0.0 
            end

            local dist = MBf(bc, "distanceToTarget", 999.0)
            local dfactor = clamp(1.0 - dist / 20.0, 0.0, 1.0)
            local pressure = MBf(bc, "pressureLevel", 0.0)
            local U = clamp(0.4 + 0.6 * math.max(dfactor, pressure), 0.0, 1.0)
            print("[COMBAT] ChaseTarget utility: " .. U .. " (dist=" .. dist .. " dfactor=" .. dfactor .. ")")
            return U
        end)

    bc:A_add("Combat", "ChaseTarget", "MoveToTargetFast")
    bc:A_setOnUpdate("Combat", "ChaseTarget", "MoveToTargetFast",
        function(goid, bc, dt)
            local pos = MBv(bc, "targetPosition")
            if not pos then return 2 end
            if self.owner.moveTo then
                local done = self.owner:moveTo(pos, "fast")
                return done and 1 or 0
            end
            return 2
        end)

    ------------------------------------------------
    -- Pattern: StrafeShoot
    -- Actions: Strafe(), AimAt(targetPosition), ShootBurst()
    ------------------------------------------------
    bc:P_add("Combat", "StrafeShoot")

    -- if targetID == null or ammoCount <= 0 -> U=0
    -- erisk = isTargetFacingMe ? 1.0 : 0.5
    -- U = Clamp(0.8 * healthPercent * erisk, 0.0,1.0)
    bc:P_setEvaluateUtility("Combat", "StrafeShoot",
        function(goid, bc)
            local tid = bc:MB_getUUID("targetID")
            local ammo = MBi(bc, "ammoCount", 0)
            if not tid or ammo <= 0 then 
                print("[COMBAT] StrafeShoot utility: 0.0 (no target or ammo=" .. ammo .. ")")
                return 0.0 
            end

            local facing = MBb(bc, "isTargetFacingMe", true)
            local erisk = facing and 1.0 or 0.5
            local hp = MBf(bc, "healthPercent", 100.0) / 100.0
            local U = clamp(0.8 * hp * erisk, 0.0, 1.0)
            print("[COMBAT] StrafeShoot utility: " .. U .. " (hp=" .. (hp*100) .. "% facing=" .. tostring(facing) .. " erisk=" .. erisk .. ")")
            return U
        end)

    bc:A_add("Combat", "StrafeShoot", "Strafe")
    bc:A_setOnUpdate("Combat", "StrafeShoot", "Strafe",
        function(goid, bc, dt)
            if self.owner.strafe then
                self.owner:strafe()
            end
            return 1
        end)

    bc:A_add("Combat", "StrafeShoot", "AimAtTarget")
    bc:A_setOnUpdate("Combat", "StrafeShoot", "AimAtTarget",
        function(goid, bc, dt)
            local pos = MBv(bc, "targetPosition")
            if self.owner.aimAt and pos then
                self.owner:aimAt(pos)
            end
            return 1
        end)

    bc:A_add("Combat", "StrafeShoot", "ShootBurst")
    bc:A_setOnUpdate("Combat", "StrafeShoot", "ShootBurst",
        function(goid, bc, dt)
            if self.owner.shootBurst then
                self.owner:shootBurst()
            end
            return 1
        end)

    ------------------------------------------------
    -- Pattern: GrenadeAndCover
    -- Actions: ThrowGrenade(targetPosition), MoveToCover(fast)
    ------------------------------------------------
    bc:P_add("Combat", "GrenadeAndCover")

    -- if NOT grenadeReady or distanceToTarget > 15 -> U=0
    -- tvalue = isTargetBehindCover ? 1.0 : 0.4
    -- desp = Clamp(1.0 - healthPercent/100.0, 0.0,1.0)
    -- U = Clamp(0.5 * tvalue + 0.5 * desp, 0.0,1.0)
    bc:P_setEvaluateUtility("Combat", "GrenadeAndCover",
        function(goid, bc)
            local grenade = MBb(bc, "grenadeReady", false)
            local dist = MBf(bc, "distanceToTarget", 999.0)
            if not grenade or dist > 15.0 then 
                print("[COMBAT] GrenadeAndCover utility: 0.0 (grenade=" .. tostring(grenade) .. " dist=" .. dist .. ")")
                return 0.0 
            end

            local behind = MBb(bc, "isTargetBehindCover", false)
            local tvalue = behind and 1.0 or 0.4
            local hp = MBf(bc, "healthPercent", 100.0)
            local desp = clamp(1.0 - hp / 100.0, 0.0, 1.0)
            local U = clamp(0.5 * tvalue + 0.5 * desp, 0.0, 1.0)
            print("[COMBAT] GrenadeAndCover utility: " .. U .. " (behind=" .. tostring(behind) .. " hp=" .. hp .. ")")
            return U
        end)

    bc:A_add("Combat", "GrenadeAndCover", "ThrowGrenade")
    bc:A_setOnUpdate("Combat", "GrenadeAndCover", "ThrowGrenade",
        function(goid, bc, dt)
            local pos = MBv(bc, "targetPosition")
            if not pos then return 2 end
            if self.owner.throwGrenade then
                self.owner:throwGrenade(pos)
            end
            return 1
        end)

    bc:A_add("Combat", "GrenadeAndCover", "MoveToCoverFast")
    bc:A_setOnUpdate("Combat", "GrenadeAndCover", "MoveToCoverFast",
        function(goid, bc, dt)
            if self.owner.moveToCover then
                local done = self.owner:moveToCover("fast")
                return done and 1 or 0
            end
            return 2
        end)

    ------------------------------------------------
    -- Pattern: SuppressAndAdvance
    -- Actions: SuppressFire(), AdvanceSlowly()
    ------------------------------------------------
    bc:P_add("Combat", "SuppressAndAdvance")

    -- if ammoCount <= 0 -> U=0
    -- safety = Clamp(healthPercent/100.0, 0.0,1.0)
    -- U = Clamp(0.3 + 0.5 * pressureLevel * safety, 0.0,1.0)
    bc:P_setEvaluateUtility("Combat", "SuppressAndAdvance",
        function(goid, bc)
            local ammo = MBi(bc, "ammoCount", 0)
            if ammo <= 0 then 
                print("[COMBAT] SuppressAndAdvance utility: 0.0 (ammo=0)")
                return 0.0 
            end

            local hp = MBf(bc, "healthPercent", 100.0)
            local safety = clamp(hp / 100.0, 0.0, 1.0)
            local pressure = MBf(bc, "pressureLevel", 0.0)
            local U = clamp(0.3 + 0.5 * pressure * safety, 0.0, 1.0)
            print("[COMBAT] SuppressAndAdvance utility: " .. U .. " (ammo=" .. ammo .. " hp=" .. hp .. " pressure=" .. pressure .. ")")
            return U
        end)

    bc:A_add("Combat", "SuppressAndAdvance", "SuppressFire")
    bc:A_setOnUpdate("Combat", "SuppressAndAdvance", "SuppressFire",
        function(goid, bc, dt)
            if self.owner.suppressFire then
                self.owner:suppressFire()
            end
            return 1
        end)

    bc:A_add("Combat", "SuppressAndAdvance", "AdvanceSlowly")
    bc:A_setOnUpdate("Combat", "SuppressAndAdvance", "AdvanceSlowly",
        function(goid, bc, dt)
            if self.owner.advanceSlowly then
                self.owner:advanceSlowly()
            end
            return 1
        end)

    ------------------------------------------------
    -- Pattern: RepositionForAngle
    -- Actions: MoveTo(BestFireAnglePosition(), fast), Aim()
    ------------------------------------------------
    bc:P_add("Combat", "RepositionForAngle")

    -- if targetID == null -> U=0
    -- adesire = isTargetBehindCover ? 1.0 : 0.4
    -- sinst = Clamp(1.0 - healthPercent/100.0, 0.2,1.0)
    -- U = Clamp(adesire * sinst, 0.0,1.0)
    bc:P_setEvaluateUtility("Combat", "RepositionForAngle",
        function(goid, bc)
            local tid = bc:MB_getUUID("targetID")
            if not tid then 
                print("[COMBAT] RepositionForAngle utility: 0.0 (no target)")
                return 0.0 
            end

            local behind = MBb(bc, "isTargetBehindCover", false)
            local adesire = behind and 1.0 or 0.4
            local hp = MBf(bc, "healthPercent", 100.0)
            local sinst = clamp(1.0 - hp / 100.0, 0.2, 1.0)
            local U = clamp(adesire * sinst, 0.0, 1.0)
            print("[COMBAT] RepositionForAngle utility: " .. U .. " (behind=" .. tostring(behind) .. " hp=" .. hp .. ")")
            return U
        end)

    bc:A_add("Combat", "RepositionForAngle", "MoveToBestAngle")
    bc:A_setOnUpdate("Combat", "RepositionForAngle", "MoveToBestAngle",
        function(goid, bc, dt)
            if not self.owner.bestFireAnglePosition then return 2 end
            local pos = self.owner:bestFireAnglePosition()
            if not pos then return 2 end
            if self.owner.moveTo then
                local done = self.owner:moveTo(pos, "fast")
                return done and 1 or 0
            end
            return 2
        end)

    bc:A_add("Combat", "RepositionForAngle", "Aim")
    bc:A_setOnUpdate("Combat", "RepositionForAngle", "Aim",
        function(goid, bc, dt)
            if self.owner.aim then
                self.owner:aim()
            end
            return 1
        end)

    print("Initialized Combat State")
end

--------------------------------------------------
--  RETREAT STATE
--  RunToSafeSpot, Heal, PeekAndThreaten
--------------------------------------------------
local function SetupRetreat(self, bc)
    ------------------------------------------------
    -- Pattern: RunToSafeSpot
    -- Actions: FindSafeLocation(), MoveTo(safeSpotPosition, fast)
    ------------------------------------------------
    bc:P_add("Retreat", "RunToSafeSpot")

    -- panic = Clamp((40 - healthPercent)/40.0, 0.0,1.0)
    -- U = Clamp(0.6 + 0.4 * panic, 0.0,1.0)
    bc:P_setEvaluateUtility("Retreat", "RunToSafeSpot",
        function(goid, bc)
            local hp = MBf(bc, "healthPercent", 100.0)
            local panic = clamp((40.0 - hp) / 40.0, 0.0, 1.0)
            local U = clamp(0.6 + 0.4 * panic, 0.0, 1.0)
            print("[RETREAT] RunToSafeSpot utility: " .. U .. " (hp=" .. hp .. " panic=" .. panic .. ")")
            return U
        end)

    bc:A_add("Retreat", "RunToSafeSpot", "FindSafeLocation")
    bc:A_setOnUpdate("Retreat", "RunToSafeSpot", "FindSafeLocation",
        function(goid, bc, dt)
            if self.owner.findSafeLocation then
                local pos = self.owner:findSafeLocation()
                if pos then
                    bc:MB_setVector3("safeSpotPosition", pos)
                    return 1
                end
            end
            return 2
        end)

    bc:A_add("Retreat", "RunToSafeSpot", "MoveToSafeSpot")
    bc:A_setOnUpdate("Retreat", "RunToSafeSpot", "MoveToSafeSpot",
        function(goid, bc, dt)
            local pos = MBv(bc, "safeSpotPosition")
            if not pos then return 2 end
            if self.owner.moveTo then
                local done = self.owner:moveTo(pos, "fast")
                return done and 1 or 0
            end
            return 2
        end)

    ------------------------------------------------
    -- Pattern: Heal
    -- Actions: UseMedkit()
    ------------------------------------------------
    bc:P_add("Retreat", "Heal")

    -- psurv = Clamp((70 - healthPercent)/50.0, 0.0,1.0)
    -- UHeal = 0 if hp >= 70
    --         1.0 if hp < 35
    --         Clamp(0.5 + 0.5*psurv, 0.0,1.0) otherwise
    bc:P_setEvaluateUtility("Retreat", "Heal",
        function(goid, bc)
            local hp = MBf(bc, "healthPercent", 100.0)
            if hp >= 70.0 then 
                print("[RETREAT] Heal utility: 0.0 (hp too high=" .. hp .. ")")
                return 0.0 
            end
            if hp < 35.0 then 
                print("[RETREAT] Heal utility: 1.0 (hp critical=" .. hp .. ")")
                return 1.0 
            end
            local psurv = clamp((70.0 - hp) / 50.0, 0.0, 1.0)
            local U = clamp(0.5 + 0.5 * psurv, 0.0, 1.0)
            print("[RETREAT] Heal utility: " .. U .. " (hp=" .. hp .. " psurv=" .. psurv .. ")")
            return U
        end)

    bc:A_add("Retreat", "Heal", "UseMedkit")
    bc:A_setOnUpdate("Retreat", "Heal", "UseMedkit",
        function(goid, bc, dt)
            if self.owner.useMedkit then
                self.owner:useMedkit()
            end
            return 1
        end)

    ------------------------------------------------
    -- Pattern: PeekAndThreaten
    -- Actions: PeekFromCover(), FireSingleShot(), HideBack()
    ------------------------------------------------
    bc:P_add("Retreat", "PeekAndThreaten")

    -- if hp > 60 or hp < 20 -> U=0
    -- rtol = Clamp(healthPercent/100.0, 0.0,1.0)
    -- U = Clamp(0.3 + 0.4 * pressureLevel * rtol, 0.0,1.0)
    bc:P_setEvaluateUtility("Retreat", "PeekAndThreaten",
        function(goid, bc)
            local hp = MBf(bc, "healthPercent", 100.0)
            if hp > 60.0 or hp < 20.0 then 
                print("[RETREAT] PeekAndThreaten utility: 0.0 (hp out of range=" .. hp .. ")")
                return 0.0 
            end
            local rtol = clamp(hp / 100.0, 0.0, 1.0)
            local pressure = MBf(bc, "pressureLevel", 0.0)
            local U = clamp(0.3 + 0.4 * pressure * rtol, 0.0, 1.0)
            print("[RETREAT] PeekAndThreaten utility: " .. U .. " (hp=" .. hp .. " pressure=" .. pressure .. ")")
            return U
        end)

    bc:A_add("Retreat", "PeekAndThreaten", "PeekFromCover")
    bc:A_setOnUpdate("Retreat", "PeekAndThreaten", "PeekFromCover",
        function(goid, bc, dt)
            if self.owner.peekFromCover then
                self.owner:peekFromCover()
            end
            return 1
        end)

    bc:A_add("Retreat", "PeekAndThreaten", "FireSingleShot")
    bc:A_setOnUpdate("Retreat", "PeekAndThreaten", "FireSingleShot",
        function(goid, bc, dt)
            if self.owner.fireSingleShot then
                self.owner:fireSingleShot()
            end
            return 1
        end)

    bc:A_add("Retreat", "PeekAndThreaten", "HideBack")
    bc:A_setOnUpdate("Retreat", "PeekAndThreaten", "HideBack",
        function(goid, bc, dt)
            if self.owner.hideBack then
                self.owner:hideBack()
            end
            return 1
        end)

    print("Initialized Retreat State")
end

--------------------------------------------------
--  IDLE STATE
--  Просто пассивное ожидание / осмотр
--------------------------------------------------
local function SetupIdle(self, bc)
    bc:P_add("Idle", "StandAndScan")

    bc:P_setEvaluateUtility("Idle", "StandAndScan",
        function(goid, bc)
            -- базовое состояние, чтобы не было -inf
            return 0.1
        end)

    bc:A_add("Idle", "StandAndScan", "IdleLook")
    bc:A_setOnUpdate("Idle", "StandAndScan", "IdleLook",
        function(goid, bc, dt)
            if self.owner.idleLook then
                self.owner:idleLook()
            end
            return 0 -- можно держать вечным, т.к. переходы делает FSM
        end)
    
    print("Initialized Idle State")
end

--------------------------------------------------
--  PERCEPTION SETUP FOR NPC SOLDIER
--------------------------------------------------

local function InitPerception(self, bc)
    local perception = self.owner:getPerception()
    if not perception then
        print("[ERROR] PerceptionComponent not found")
        return
    end

    local perceptionSystem = getPerceptionSystem()
    perceptionSystem:registerTeam(1)   -- команда игрока
    perceptionSystem:registerTeam(2)   -- команда врагов

    if not playerObj then
        print("[ERROR] playerObj is nil - cannot initialize perception teams")
        return
    end
    
    perceptionSystem:addToTeam(1, playerObj:getPerception())
    perceptionSystem:addToTeam(2, self.owner:getPerception())

    -- кто кого должен видеть
    perceptionSystem:addSightTargetTeamIDsInTeam(2, 1)  -- враги (2) замечают игроков (1)
    perceptionSystem:addSightTargetTeamIDsInTeam(1, 2)  -- опционально: игрок видит врагов
    



    ------------------------------------------------
    -- Base sight / hearing setup (tune as needed)
    ------------------------------------------------
    perception:setCanSee(true)
    perception:setSight(
        10.0,                   -- sightRadius
        15.0,                   -- loseRadius
        100.0,                  -- FOV in degrees
        Vector3.new(0.0, 1.7, 0.0), -- eyesOffset
        false                   -- canSeeThroughObjects
    )

    perception:setCanHear(true)
    perception:setHearing(
        16.0,   -- hearingRadius
        0.25,   -- threshold
        1.0     -- sensitivity
    )

    ------------------------------------------------
    -- SIGHT CALLBACK
    --  * updates: targetID, targetPosition, lastSeenPosition,
    --             timeSinceLastSeen, distanceToTarget,
    --             isTargetFacingMe, pressureLevel, uncertaintyLevel
    ------------------------------------------------
    
    perception:addSightCallback(function(targetId, newCond)
        -- newCond: true = now visible, false = lost
        if newCond then
            -- Target acquired
            bc:MB_setUUID("targetID", targetId)
            print("[PERCEPTION] TARGET SIGHTED: " .. targetId:toString())
            
            -- We assume you can query target position & facing via some API.
            -- Replace `getGameObjectByUUID` / `getForward` with your engine helpers.
            local targetObj = getGameObjectByUUID(targetId)
            if targetObj then
                local tpos = targetObj:getTransform():getAbsolutePosition()
                bc:MB_setVector3("targetPosition", tpos)
                bc:MB_setVector3("lastSeenPosition", tpos)
                print("[PERCEPTION] Target Position: x=" .. tpos.x .. ", y=" .. tpos.y .. ", z=" .. tpos.z)

                local myPos = self.owner:getTransform():getAbsolutePosition()
                local dist = sub(tpos, myPos):length()
                bc:MB_setFloat("distanceToTarget", dist)
                print("[PERCEPTION] Distance to Target: " .. dist)
            end

            -- We roughly say: when enemy is seen, uncertainty drops, pressure grows
            local pressure = bc:MB_getFloat("pressureLevel") or 0.0
            local uncertainty = bc:MB_getFloat("uncertaintyLevel") or 0.0
            local newPressure = math.min(1.0, pressure + 0.3)
            local newUncertainty = math.max(0.0, uncertainty - 0.4)
            bc:MB_setFloat("pressureLevel", newPressure)
            bc:MB_setFloat("uncertaintyLevel", newUncertainty)
            print("[PERCEPTION] Updated - Pressure: " .. newPressure .. ", Uncertainty: " .. newUncertainty)

            -- Reset last seen timer
            bc:MB_setFloat("timeSinceLastSeen", 0.0)

            -- Simple heuristic for isTargetFacingMe (if you have a better API – plug it here)
            if targetObj then
                local myPos = self.owner:getTransform():getAbsolutePosition()
                local tpos = targetObj:getTransform():getAbsolutePosition()
                local toMe = sub(myPos, tpos):normalized()
                local forward = getForward(targetObj)
                local dot = dotVector(forward, toMe)
                local isFacing = dot > 0.6
                bc:MB_setBool("isTargetFacingMe", isFacing)
                print("[PERCEPTION] Target Facing Me: " .. tostring(isFacing) .. " (dot=" .. dot .. ")")
            else
                bc:MB_setBool("isTargetFacingMe", true)
                print("[PERCEPTION] Could not get target object for facing check")
            end
        else
            print("[PERCEPTION] TARGET LOST")
            bc:MB_setUUID("targetID", nilUUID)

            -- Target lost (still keep targetID for Hunt; timers will handle transitions)
            bc:MB_setFloat("timeSinceLastSeen", 0.0)
            -- Uncertainty grows when we lose visual contact
            local uncertainty = bc:MB_getFloat("uncertaintyLevel") or 0.0
            local newUncertainty = math.min(1.0, uncertainty + 0.3)
            bc:MB_setFloat("uncertaintyLevel", newUncertainty)
            print("[PERCEPTION] Updated Uncertainty on lost: " .. newUncertainty)
        end
    end)

    ------------------------------------------------
    -- HEARING CALLBACK
    --  * updates: lastHeardNoise, timeSinceHeardNoise,
    --             pressureLevel, uncertaintyLevel
    ------------------------------------------------
    perception:addHearingCallback(function(sourceId, loudness)
        print("[PERCEPTION] NOISE HEARD - Loudness: " .. loudness)
        -- Assume source position can be obtained similarly
        local srcObj = getGameObjectByUUID and getGameObjectByUUID(sourceId) or nil
        if srcObj then
            local pos = srcObj:getTransform():getAbsolutePosition()
            bc:MB_setVector3("lastHeardNoise", pos)
            print("[PERCEPTION] Noise Source: x=" .. pos.x .. ", y=" .. pos.y .. ", z=" .. pos.z)
        else
            -- Fallback: use own position + some offset if needed
            local myPos = self.owner:getTransform():getAbsolutePosition()
            bc:MB_setVector3("lastHeardNoise", myPos)
            print("[PERCEPTION] Noise Source: Could not determine, using own position")
        end

        -- Reset timer since last noise
        bc:MB_setFloat("timeSinceHeardNoise", 0.0)

        -- Noise increases pressure & uncertainty (depends on loudness)
        local pressure = bc:MB_getFloat("pressureLevel") or 0.0
        local uncertainty = bc:MB_getFloat("uncertaintyLevel") or 0.0

        local pDelta = math.min(0.4, loudness / 100.0)
        local uDelta = math.min(0.4, loudness / 120.0)

        local newPressure = math.min(1.0, pressure + pDelta)
        local newUncertainty = math.min(1.0, uncertainty + uDelta)
        bc:MB_setFloat("pressureLevel", newPressure)
        bc:MB_setFloat("uncertaintyLevel", newUncertainty)
        print("[PERCEPTION] Updated - Pressure: " .. newPressure .. ", Uncertainty: " .. newUncertainty)
    end)

    ------------------------------------------------
    -- DAMAGE CALLBACK
    --  * updates: healthPercent, pressureLevel
    ------------------------------------------------
    perception:addDamageCallback(function(instigatorId, dmg)
        local hp = bc:MB_getFloat("healthPercent") or 100.0
        hp = math.max(0.0, hp - dmg)
        bc:MB_setFloat("healthPercent", hp)
        print("[PERCEPTION] DAMAGE TAKEN: " .. dmg .. " | Health now: " .. hp)

        local pressure = bc:MB_getFloat("pressureLevel") or 0.0
        local newPressure = math.min(1.0, pressure + dmg / 100.0)
        bc:MB_setFloat("pressureLevel", newPressure)
        print("[PERCEPTION] Pressure increased to: " .. newPressure)
    end)

    print("Initialized Perception for NPC Soldier")
end

--------------------------------------------------
--  PERCEPTION TIMERS UPDATE (call from behavior:update)
--------------------------------------------------

local function UpdatePerceptionTimers(self, bc, dt)
    -- timeSinceLastSeen
    local tSeen = bc:MB_getFloat("timeSinceLastSeen") or 999.0
    bc:MB_setFloat("timeSinceLastSeen", tSeen + dt)

    -- timeSinceHeardNoise
    local tNoise = bc:MB_getFloat("timeSinceHeardNoise") or 999.0
    bc:MB_setFloat("timeSinceHeardNoise", tNoise + dt)

    -- distanceToTarget (if target still valid)
    local tid = bc:MB_getUUID("targetID")

    if tid then
        local targetObj = getGameObjectByUUID(tid)
        if targetObj then
            local tpos = targetObj:getTransform():getAbsolutePosition()
            bc:MB_setVector3("targetPosition", tpos)
            local myPos = self.owner:getTransform():getAbsolutePosition()
            local dist = sub(tpos, myPos):length()
            bc:MB_setFloat("distanceToTarget", dist)
            -- print("Updated distanceToTarget: " .. dist)

            rotateToObjDirection(self, targetObj)
        end
    end
end


--------------------------------------------------
--  behavior:start / update / destroy
--------------------------------------------------
function behavior:start()
    playerObj = getPlayerObject()
    playerUUID = playerObj:getUUID()
    selfUUID = self.owner:getUUID()

    local bc = self.owner:getBehavior()
    if not bc then
        print("[ERROR] BehaviorController not found")
        return
    end
    InitPerception(self, bc)
    InitMemoryBoard(self, bc)
    SetupStates(self, bc)
    SetupTransitions(self, bc)
    SetupPatrol(self, bc)
    SetupHunt(self, bc)
    SetupCombat(self, bc)
    SetupRetreat(self, bc)
    SetupIdle(self, bc)
end

function behavior:update(dt)
    local bc = self.owner:getBehavior()
    if not bc then return end

    UpdatePerceptionTimers(self, bc, dt)
    -- Всё делается внутри BehaviorController по твоему Execution Model.
    -- Здесь можно, например, обновлять таймеры perception, но основная логика выше.
end

function behavior:destroy()
end

return behavior
