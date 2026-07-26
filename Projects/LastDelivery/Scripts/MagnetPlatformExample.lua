local behavior = {}
local magnetSpeed = 10
local enableMagnet = false

function enableMagnetControl()
    enableMagnet = true
end

function disableMagnetControl()
    enableMagnet = false
end

function behavior:start()

end

function behavior:update(dt)

    if (enableMagnet) then
        
        local inputValue = InputSystem:getAxis2D("Forward", "Right")
        local upMagnet = InputSystem:getAxis("UpMagnet")
        local deltaPos = Vector3.new(-inputValue.x, upMagnet, inputValue.y) * magnetSpeed * dt
        -- print(deltaPos.x .. " " .. deltaPos.y .. " " .. deltaPos.z)

        local ph = self.owner:getPhysics()
        local pos = ph:getPosition()

        ph:moveKinematicPosition(pos + deltaPos, dt)
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
