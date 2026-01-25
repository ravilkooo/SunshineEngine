behavior = {}

function behavior:start()
    self.speedMyX = 1000.0
    self.checkTechnique = false
    -- print("Behavior started", self.id)
end

function behavior:update(dt)
    local physics = self.owner:getPhysics()
    physics:addImpulse(Vector3.new(self.speedMyX,0,0))
    -- print("Behavior update", self.id)

    local transform = self.owner:getTransform()
    if (transform.m_position.x > 20) then
        removeGameObjectByUUID(self.owner:getUUID())
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior