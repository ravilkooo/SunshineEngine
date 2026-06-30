behavior = {}
magnetSpeed = 0.1

function behavior:start()

end

function behavior:update(dt)

    local inputSystem = getInputSystem()
    local inputValue = inputSystem:getAxis2D("ForwardMagnet", "RightMagnet")
    local upMagnet = inputSystem:getAxis("UpMagnet")

    local tr = self.owner:getTransform()
    tr.m_position = tr.m_position + Vector3.new(inputValue.x, upMagnet, inputValue.y) * magnetSpeed

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
