behavior = {}

function behavior:start()
    self.speedX = 1000.0
    self.checkTechnique = false
    -- print("Behavior started", self.id)
end

function behavior:update(dt)
    local physics = self.owner:getPhysics()
    physics:addForce(Vector3.new(self.speedX,0,0))
    -- print("Behavior update", self.id)
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior