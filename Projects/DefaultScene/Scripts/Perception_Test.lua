behavior = {}

local function vecEqual(a, b)
    return math.abs(a.x - b.x) < 0.001 and math.abs(a.y - b.y) < 0.001 and math.abs(a.z - b.z) < 0.001
end

local function testEqual(name, expected, actual)
    if expected == actual then
        print("[OK]", name)
    else
        print("[FAIL]", name, "expected:", expected, "actual:", actual)
    end
end

local function testEqualFloat(name, expected, actual)
    if math.abs(expected - actual) < 0.001 then
        print("[OK]", name)
    else
        print("[FAIL]", name, "expected:", expected, "actual:", actual)
    end
end

local function testVec(name, expected, actual)
    if vecEqual(expected, actual) then
        print("[OK]", name)
    else
        print("[FAIL]", name,
            "expected:", expected.x, expected.y, expected.z,
            "actual:",   actual.x,   actual.y,   actual.z)
    end
end

function TestPerceptionComponent(self)

    local perception = self.owner:getPerception()

    if not perception then
        print("[ERROR] PerceptionComponent not found")
        return
    end

    print("=== PerceptionComponent test start ===")

    -- =========================
    -- SIGHT
    -- =========================

    perception:setCanSee(true); testEqual("CanSee", true, perception:getCanSee())

    local loseRadius = 30.0
    perception:setLoseRadius(loseRadius); testEqualFloat("LoseRadius", loseRadius, perception:getLoseRadius())

    local sightRadius = 25.0
    perception:setSightRadius(sightRadius); testEqualFloat("SightRadius", sightRadius, perception:getSightRadius())

    local fov = 90.0
    perception:setFieldOfView(fov); testEqualFloat("FieldOfView", fov, perception:getFieldOfView())

    local eyesOffset = Vector3.new(0.0, 1.7, 0.0)
    perception:setEyesOffset(eyesOffset); testVec("EyesOffset", eyesOffset, perception:getEyesOffset())

    perception:setCanSeeThroughObjects(true); testEqual("CanSeeThroughObjects", true, perception:getCanSeeThroughObjects())

    local sr, lr, fov2 = 40.0, 50.0, 110.0
    local eo2 = Vector3.new(0.1, 1.8, 0.0)
    local seeThrough = false

    perception:setSight(sr, lr, fov2, eo2, seeThrough)
    testEqualFloat("SetSight/SightRadius", sr, perception:getSightRadius())
    testEqualFloat("SetSight/LoseRadius",  lr, perception:getLoseRadius())
    testEqualFloat("SetSight/FOV",         fov2, perception:getFieldOfView())
    testVec("SetSight/EyesOffset", eo2, perception:getEyesOffset())
    testEqual("SetSight/CanSeeThroughObjects", seeThrough, perception:getCanSeeThroughObjects())

    -- =========================
    -- HEARING
    -- =========================

    perception:setCanHear(true); testEqual("CanHear", true, perception:getCanHear())

    local hearingRadius = 20.0
    perception:setHearingRadius(hearingRadius); testEqualFloat("HearingRadius", hearingRadius, perception:getHearingRadius())

    local threshold = 0.35
    perception:setThreshold(threshold); testEqualFloat("Threshold", threshold, perception:getThreshold())

    local sensitivity = 1.25
    perception:setSensitivity(sensitivity); testEqualFloat("Sensitivity", sensitivity, perception:getSensitivity())

    local hr2, th2, se2 = 35.0, 0.5, 2.0
    perception:setHearing(hr2, th2, se2)
    testEqualFloat("SetHearing/HearingRadius", hr2, perception:getHearingRadius())
    testEqualFloat("SetHearing/Threshold",     th2, perception:getThreshold())
    testEqualFloat("SetHearing/Sensitivity",   se2, perception:getSensitivity())

    print("=== PerceptionComponent test end ===")
end

local function TestPerceptionSystem(self)

    print("=== PerceptionSystem test start ===")

    local ps = getPerceptionSystem()

    local teamA = 1
    local teamB = 2
    local invalidTeam = 0xFFFFFFFF

    -- =========================
    -- REGISTER
    -- =========================

    testEqual("RegisterTeam A", true, ps:registerTeam(teamA))
    testEqual("RegisterTeam B", true, ps:registerTeam(teamB))
    testEqual("RegisterTeam duplicate", false, ps:registerTeam(teamA))
    testEqual("RegisterTeam invalid", false, ps:registerTeam(invalidTeam))

    -- =========================
    -- SIGHT / HEARING TARGETS
    -- =========================

    testEqual("AddSightTargetTeamIDsInTeam", true, ps:addSightTargetTeamIDsInTeam(teamA, teamB))
    testEqual("AddHearingSourceTeamIDsInTeam", true, ps:addHearingSourceTeamIDsInTeam(teamA, teamB))
    testEqual("AddSightTarget duplicate", false, ps:addSightTargetTeamIDsInTeam(teamA, teamB))
    testEqual("AddHearingSource duplicate", false, ps:addHearingSourceTeamIDsInTeam(teamA, teamB))
    testEqual("AddSightTarget not existing team", false, ps:addSightTargetTeamIDsInTeam(999, teamB))

    testEqual("RemoveSightTargetTeamIDsInTeam", true, ps:removeSightTargetTeamIDsInTeam(teamA, teamB))
    testEqual("RemoveHearingSourceTeamIDsInTeam", true, ps:removeHearingSourceTeamIDsInTeam(teamA, teamB))
    testEqual("RemoveSightTarget missing", false, ps:removeSightTargetTeamIDsInTeam(teamA, teamB))
    testEqual("RemoveHearingTarget missing", false, ps:removeHearingSourceTeamIDsInTeam(teamA, teamB))

    testEqual("ClearSightTargetTeamIDsInTeam", true, ps:clearSightTargetTeamIDsInTeam(teamA))
    testEqual("ClearHearingSourceTeamIDsInTeam", true, ps:clearHearingSourceTeamIDsInTeam(teamA))
    testEqual("ClearSightTargetTeamIDsInTeam not existing team", false, ps:clearSightTargetTeamIDsInTeam(999))
    testEqual("ClearHearingSourceTeamIDsInTeam not existing team", false, ps:clearHearingSourceTeamIDsInTeam(999))

    -- =========================
    -- ADD / REMOVE / CLEAR TEAM 
    -- =========================

    local perception = self.owner:getPerception()

    testEqual("AddToTeam null", false, ps:addToTeam(teamA, nil))
    testEqual("AddToTeam not existing team", false, ps:addToTeam(999, perception))
    testEqual("AddToTeam", true, ps:addToTeam(teamA, perception))
    testEqual("AddToTeam duplicate", false, ps:addToTeam(teamA, perception))
    testEqual("AddToTeam another team", false, ps:addToTeam(teamB, perception))

    testEqual("RemoveFromTeam", true, ps:removeFromTeam(perception))
    testEqual("RemoveFromTeam again", false, ps:removeFromTeam(perception))

    ps:addSightTargetTeamIDsInTeam(teamA, teamB)
    ps:addHearingSourceTeamIDsInTeam(teamA, teamB)
    ps:addToTeam(teamA, perception)

    testEqual("ClearTeam", true, ps:clearTeam(teamA))
    testEqual("ClearTeam not existing team", false, ps:clearTeam(999))

    testEqual("AddSightTargetTeamIDsInTeam after clear", true, ps:addSightTargetTeamIDsInTeam(teamA, teamB))
    testEqual("AddHearingSourceTeamIDsInTeam after clear", true, ps:addHearingSourceTeamIDsInTeam(teamA, teamB))
    testEqual("AddToTeam after clear", true, ps:addToTeam(teamA, perception))

    print("=== PerceptionSystem test end ===")
end

function TestPerceptionComponentWithPerceptionSystem(self)

    print("=== PerceptionComponent with Perception System test start ===")

    local ps = getPerceptionSystem()
    local perception = self.owner:getPerception()

    local teamA = 1
    local teamB = 2

    ps:clearTeam(teamA)
    ps:clearTeam(teamB)

    perception:setCanSee(true)
    perception:setSight(10, 20, 90, Vector3.new(0.0, 0.0, 0.0), false)

    perception:setCanHear(true)
    perception:setHearing(1000, 0, 1)

    ps:addSightTargetTeamIDsInTeam(teamA, teamB)
    ps:addHearingSourceTeamIDsInTeam(teamA, teamB)
    ps:addHearingSourceTeamIDsInTeam(teamA, teamA)
    ps:addToTeam(teamA, perception)

    -- =========================
    -- CALLBACKS
    -- =========================

    -- ---------- SIGHT CALLBACK ----------
    local sightTargetId = nil

    local sightCbId = perception:addSightCallback(
        function(targetId, newCond)
            print("I can see", targetId, newCond)
        end)

    testEqual("AddSightCallback id valid", true, sightCbId ~= 0)

    -- perception:removeSightCallback(sightCbId)
    -- perception:clearSightCallbacks()

    -- ---------- HEARING CALLBACK ----------
    local hearingCbId = perception:addHearingCallback(
        function(sourceId, loudness)
             print("I can hear", sourceId, "with loudness", loudness)
        end)

    testEqual("AddHearingCallback id valid", true, hearingCbId ~= 0)

    -- perception:removeHearingCallback(hearingCbId)
    -- perception:clearHearingCallbacks()

    -- ---------- DAMAGE CALLBACK ----------
    local damageCbId = perception:addDamageCallback(
        function(instId, dmg)
            print("I can feel pain", instId, "with damage", dmg)
        end)

    testEqual("AddDamageCallback id valid", true, damageCbId ~= 0)

    -- perception:removeDamageCallback(damageCbId)
    -- perception:clearDamageCallbacks()

    print("=== PerceptionComponent with Perception System end ===")
end

function behavior:start()
    TestPerceptionComponent(self)
    TestPerceptionSystem(self)
    TestPerceptionComponentWithPerceptionSystem(self)
end

local first = true

function behavior:update(dt)
    if first then 
        first = false
        local perception = self.owner:getPerception()
        perception:makeNoise(75)
        perception:dealDamage(perception, 69)
    end
end

function behavior:destroy()

end

return behavior
