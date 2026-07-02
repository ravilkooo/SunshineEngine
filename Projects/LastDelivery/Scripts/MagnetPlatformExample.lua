behavior = {}
magnetSpeed = 10

function behavior:start()

end

function behavior:update(dt)

    local inputSystem = getInputSystem()
    local inputValue = inputSystem:getAxis2D("ForwardMagnet", "RightMagnet")
    local upMagnet = inputSystem:getAxis("UpMagnet")
    local deltaPos = Vector3.new(inputValue.x, upMagnet, inputValue.y) * magnetSpeed * dt
    -- print(deltaPos.x .. " " .. deltaPos.y .. " " .. deltaPos.z)

    local ph = self.owner:getPhysics()
    local pos = ph:getPosition()

    ph:moveKinematicPosition(pos + deltaPos, dt)

    -- local tr = self.owner:getTransform()
    -- tr.m_position = tr.m_position + Vector3.new(inputValue.x, upMagnet, inputValue.y) * magnetSpeed

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
