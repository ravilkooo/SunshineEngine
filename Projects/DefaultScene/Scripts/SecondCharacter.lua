behavior = {}

local playerUUID
local playerObj
local collected = false
local rotSpeed = 10
local radius = 3
local upOffset = 0.6
local scale = 0.1

-- can be made unique for every object
-- for example: based on initial position of object or time
local angleOffet = 0.2


function behavior:start()
    -- TestCharacterFields(self)
end

function behavior:update(dt)
    -- local tr = self.owner:getTransform()
    local inputSystem = getInputSystem()
    local speed = 0.1
    local char = self.owner:getCharacterComponent()

    local inputValue = inputSystem:getAxis2D("Forward2", "Right2")
    char.m_moveInput = Vector2.new(inputValue.y, inputValue.x)

    -- local mouseMove = Vector2.new(, inputSystem:getMouseDeltaY())
    char.m_yaw = char.m_yaw + 0.004 * inputSystem:getMouseDeltaX()
    -- print(inputSystem:getMouseDeltaX())
    -- char.m_pitch = char.m_pitch + inputSystem:getMouseDeltaY()
    
    -- tr.m_rotation.y = tr.m_rotation.y + speed * inputSystem:getAxis("Forward")
    
    
    -- if (inputSystem:isPressed("RAction")) then
        -- print("RACTION")
        -- tr.m_rotation.y = tr.m_rotation.y + 0.5
    -- elseif (inputSystem:isReleased("RAction")) then
        -- print("RACTION")
        -- tr.m_rotation.y = tr.m_rotation.y - 0.5
    -- end

    if (inputSystem:isPressed("Jump2")) then
        print("== JUMP2! ==")
        char.m_jumpRequested = true
        -- tr.m_rotation.y = tr.m_rotation.y - 0.5
    end



    if not collected then

        -- local currentTime = os.clock()
        -- self.owner:getTransform().m_rotation = Vector3.new(0, rotSpeed * currentTime, 0)
        
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
