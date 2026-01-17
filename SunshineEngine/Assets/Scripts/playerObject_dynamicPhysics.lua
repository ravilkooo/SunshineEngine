behavior = {}

function behavior:start()
    self.sensitivity = 50.0
    -- print("Behavior started", self.id)
end

function behavior:update(dt)
    local physics = self.owner:getPhysics()
    local cameraComponent = self.owner:getCameraComponent()
    local camera = cameraComponent:getCamera()
    
    local currentTime = os.clock()

    camera:setStickLength(20 + 3 * math.sin(currentTime))
    -- print("Behavior update", self.id)
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior