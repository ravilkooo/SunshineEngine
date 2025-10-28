behavior = {}

function behavior:start()
    self.speedY = 1.0
    print("Behavior started", self.id)
end

function behavior:update(dt)
    print("Update:", self.id, dt)
    local tr = self.owner:getTransform()
    tr.m_position.y = tr.m_position.y - self.speedY * dt
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
