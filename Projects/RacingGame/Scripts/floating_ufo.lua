behavior = {}

local returningSpeed = 100

function behavior:start()

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