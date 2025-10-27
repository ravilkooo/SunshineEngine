behavior = {}

function behavior:start()
    print("Behavior started", self.id)
end

function behavior:update(dt)
    print("Update:", self.id, dt)
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
