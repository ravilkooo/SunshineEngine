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
    playerUUID.hi = 851949874
    playerUUID.lo = 907375339
    playerObj = getGameObjectByUUID(playerUUID)

    local trigger = self.owner:getTrigger()


    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if not collected then
                if event == "enter" then
                    if otherUUID:isEqual(playerUUID) then
                        local bounce = self.owner:getBouncePad()
                        bounce:bounceCharacter(playerUUID)
                    end
                end
            end
        end)
    end
end

function behavior:update(dt)
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
