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
    
    playerUUID = UUID.new()
    playerUUID.hi = 253145895
    playerUUID.lo = 2320618671
    playerObj = getGameObjectByUUID(playerUUID)

    local trigger = self.owner:getTrigger()

    trigger:setLuaCallback(function(event, otherUUID)
        if not collected then
            if event == "enter" then
                if otherUUID:isEqual(playerUUID) then
                    collected = true
                    angleOffet = self.owner:getTransform().m_position.x
                    self.owner:getTransform().m_scaleFactor = Vector3.new(scale, scale, scale)
                end
            end
        end
    end)
    -- print("Behavior started", self.id
end

function behavior:update(dt)
    
    if collected then

        local currentTime = os.clock()
        local playerPos = playerObj:getTransform():getAbsolutePosition()

        local newPos = Vector3.new(
            playerPos.x + radius * math.cos(angleOffet + rotSpeed * currentTime),
            playerPos.y + upOffset,
            playerPos.z + radius * math.sin(angleOffet + rotSpeed * currentTime))

        self.owner:getTransform().m_position = newPos
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
