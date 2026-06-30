behavior = {}

local playerUUID
local playerObj
local rotationEnabled = false
local startTime = 0
local rotationDuration = 0.7

function behavior:start()
    -- TestCharacterFields(self)
end

function behavior:update(dt)
    -- local tr = self.owner:getTransform()
    local inputSystem = getInputSystem()

    if (inputSystem:isPressed("FAction")) then
        print("== FAction! ==")
        rotationEnabled = true
        startTime = os.clock()
    end



    if rotationEnabled then
        local currentTime = (os.clock() - startTime) / rotationDuration
        if currentTime < 1 then
            self.owner:getTransform().m_rotation = Vector3.new(math.pi * 0.5 * math.sin(math.pi * currentTime), 0, 0)
        else
            rotationEnabled = false
            self.owner:getTransform().m_rotation = Vector3.new(0, 0, 0)
        end
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
