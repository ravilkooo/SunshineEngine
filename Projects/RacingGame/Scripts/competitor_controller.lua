behavior = {}

local tracePoints = {}
local currentTPIndex = 0
local nextTPIndex = 0

local circle = -1
local circleChanged = false

local moveSpeed = 3500
local returningSpeed = 200

local started = false

function behavior:start()
    
    tracePoint01 = UUID.new()
    tracePoint01.hi = 468301622
    tracePoint01.lo = 3045275865
    table.insert(tracePoints, tracePoint01)

    tracePoint02 = UUID.new()
    tracePoint02.hi = 468301622
    tracePoint02.lo = 3045275866
    table.insert(tracePoints, tracePoint02)

    tracePoint03 = UUID.new()
    tracePoint03.hi = 468301622
    tracePoint03.lo = 3045275867
    table.insert(tracePoints, tracePoint03)

    tracePoint04 = UUID.new()
    tracePoint04.hi = 468301622
    tracePoint04.lo = 3045275868
    table.insert(tracePoints, tracePoint04)

    tracePoint05 = UUID.new()
    tracePoint05.hi = 468301622
    tracePoint05.lo = 3045275869
    table.insert(tracePoints, tracePoint05)

    tracePoint06 = UUID.new()
    tracePoint06.hi = 468301622
    tracePoint06.lo = 3045275870
    table.insert(tracePoints, tracePoint06)

    tracePoint07 = UUID.new()
    tracePoint07.hi = 468301622
    tracePoint07.lo = 3045275871
    table.insert(tracePoints, tracePoint07)

    tracePoint08 = UUID.new()
    tracePoint08.hi = 468301622
    tracePoint08.lo = 3045275872
    table.insert(tracePoints, tracePoint08)

    tracePoint09 = UUID.new()
    tracePoint09.hi = 468301622
    tracePoint09.lo = 3045275873
    table.insert(tracePoints, tracePoint09)

    tracePoint10 = UUID.new()
    tracePoint10.hi = 468301622
    tracePoint10.lo = 3045275874
    table.insert(tracePoints, tracePoint10)

    tracePoint11 = UUID.new()
    tracePoint11.hi = 468301622
    tracePoint11.lo = 3045275875
    table.insert(tracePoints, tracePoint11)

    tracePoint12 = UUID.new()
    tracePoint12.hi = 468301622
    tracePoint12.lo = 3045275876
    table.insert(tracePoints, tracePoint12)

    tracePoint13 = UUID.new()
    tracePoint13.hi = 468301622
    tracePoint13.lo = 3045275877
    table.insert(tracePoints, tracePoint13)

    tracePoint14 = UUID.new()
    tracePoint14.hi = 468301622
    tracePoint14.lo = 3045275878
    table.insert(tracePoints, tracePoint14)

    tracePoint15 = UUID.new()
    tracePoint15.hi = 468301622
    tracePoint15.lo = 3045275879
    table.insert(tracePoints, tracePoint15)

    tracePoint16 = UUID.new()
    tracePoint16.hi = 468301622
    tracePoint16.lo = 3045275880
    table.insert(tracePoints, tracePoint16)

    tracePoint17 = UUID.new()
    tracePoint17.hi = 468301622
    tracePoint17.lo = 3045275881
    table.insert(tracePoints, tracePoint17)

    tracePoint18 = UUID.new()
    tracePoint18.hi = 468301622
    tracePoint18.lo = 3045275882
    table.insert(tracePoints, tracePoint18)

    tracePoint19 = UUID.new()
    tracePoint19.hi = 468301622
    tracePoint19.lo = 3045275883
    table.insert(tracePoints, tracePoint19)

    tracePoint20 = UUID.new()
    tracePoint20.hi = 468301622
    tracePoint20.lo = 3045275884
    table.insert(tracePoints, tracePoint20)

    tracePoint21 = UUID.new()
    tracePoint21.hi = 468301622
    tracePoint21.lo = 3045275885
    table.insert(tracePoints, tracePoint21)

    tracePoint22 = UUID.new()
    tracePoint22.hi = 468301622
    tracePoint22.lo = 3045275886
    table.insert(tracePoints, tracePoint22)

    --perception
    local perceptionSystem = getPerceptionSystem()
    perceptionSystem:registerTeam(2)

    local perception = self.owner:getPerception()
    perceptionSystem:addToTeam(2, perception)
end

function behavior:update(dt)
    
    --floating
    local physics = self.owner:getPhysics()
    currentTime = os.clock()
    if (currentTime % 1 < 0.5) then
        local up = Vector3.new(0, 1500, 0)
        physics:addImpulse(up)
    end
     
    local transform = self.owner:getTransform()
    local currentLocation = Vector3.new(transform.m_position.x, transform.m_position.y, transform.m_position.z)

    --checking if should move next (TPL for trace point location)
    for idx, value in pairs(tracePoints) do
        local possibleTPO = getGameObjectByUUID(value)
        local possibleTPT = possibleTPO:getTransform()
        local possibleTPL = Vector3.new(possibleTPT.m_position.x, possibleTPT.m_position.y, possibleTPT.m_position.z)
        
        local dx = currentLocation.x - possibleTPL.x
        local dy = currentLocation.y - possibleTPL.y
        local dz = currentLocation.z - possibleTPL.z

        local distance = math.sqrt(dx * dx + dy * dy + dz * dz)
        if (distance < 15.0) then
            currentTPIndex = idx
            if (idx + 1 < #tracePoints) then
                nextTPIndex = idx + 1              
            else
                nextTPIndex = 1
            end
        end
    end

    if (nextTPIndex == 2) then
        if (circleChanged ~= true) then
            circle = circle + 1
            circleChanged = true
        end
    else
        circleChanged = false
    end

    --moving
    if (circle < 3) then
        local target = getGameObjectByUUID(tracePoints[nextTPIndex])
        local targetTransform = target:getTransform()
        local forward = Vector3.new(targetTransform.m_position.x - currentLocation.x, targetTransform.m_position.y - currentLocation.y, targetTransform.m_position.z - currentLocation.z)
        local forwardLength = math.sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z)
        forward = Vector3.new((forward.x / forwardLength) * moveSpeed, (forward.y / forwardLength) * moveSpeed, (forward.z / forwardLength) * moveSpeed)
        --if staying
        local velocity = physics:getLinearVelocity()
        local speed = math.sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z)
        if (speed > 10) then
            started = true
        end
        if (speed < 5 and started == true) then
            local backward = Vector3.new(-forward.x * 10, -forward.y * 10, -forward.z * 10)
            physics:addImpulse(backward)
            started = false
        else
            physics:addImpulse(forward)
        end
    end

    --rotating
    local rotation = physics:getRotation()
    if (math.abs(rotation.x) > 0.2 or math.abs(rotation.z) > 0.2) then
        local force = Vector3.new(rotation.x * -returningSpeed, rotation.y * -returningSpeed, rotation.z * -returningSpeed)
        physics:addAngularImpulse(force)
    end

    --upper border
    local position = physics:getPosition()
    if (position.y >= 40) then
        local down = Vector3.new(0, -5500, 0)
        physics:addImpulse(down)
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior