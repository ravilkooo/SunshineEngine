behavior = {}

function behavior:start()
    self.speedMyY = 1.0
    self.checkTechnique = false
    print("Behavior started", self.id)
end

function behavior:update(dt)

    local rc = self.owner:getRender()
    if rc and rc:hasTechnique("GPass") and self.checkTechnique == false then
        print("Use GPass!")
        self.checkTechnique = true
    end

    local tr = self.owner:getTransform()
    tr.m_position.x = tr.m_position.x - self.speedMyY * dt
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
