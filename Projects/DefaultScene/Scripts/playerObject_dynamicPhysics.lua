behavior = {}

function behavior:start()
    self.sensitivity = 50.0
    
    -- Perception
    local teamB = 2

    local ps = getPerceptionSystem()
    local perception = self.owner:getPerception()

    ps:addToTeam(teamB, perception)

    -- local camera = self.owner:getCameraComponent():getCamera()
    -- camera:setAsMainCamera()
end

function behavior:update(dt)
    local physics = self.owner:getPhysics()
    local rot = physics:getRotation()
    physics:setRotation(Vector3.new(0, rot.y, 0))
    -- print("Behavior update", self.id)
    
    -- local uuid = self.owner:getUUID()
    -- print("Owner UUID: " .. uuid.hi .. " " .. uuid.lo)
    local customUUID = UUID.new()
    customUUID.hi = 163351393
    customUUID.lo = 285791296
    local customObject = getGameObjectByUUID(customUUID)
    if customObject then
        -- local customPhysics = customObject:getPhysics()
        -- customPhysics:addForce(Vector3.new(0, 100.0 * math.sin(currentTime), 0))
        local customTransform = customObject:getTransform()
        -- print("Transform: " .. customTransform.m_position.x .. ", " .. customTransform.m_position.y .. ", " .. customTransform.m_position.z)
    end
    
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior