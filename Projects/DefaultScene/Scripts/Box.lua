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
                    local audio = getAudioSystem()
                    audio:play("pause")
                    removeGameObjectByUUID(self.owner:getUUID())
                end
            end
        end
    end)
    -- print("Behavior started", self.id
end

function behavior:update(dt)
    
    if not collected then

        local currentTime = os.clock()
        self.owner:getTransform().m_rotation = Vector3.new(0, rotSpeed * currentTime, 0)
        
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
