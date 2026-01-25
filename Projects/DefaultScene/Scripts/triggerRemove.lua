behavior = {}

local playerUUID
local lastScreamTime = 4
local screamCooldown = 2

function behavior:start()

    playerUUID = UUID.new()
    playerUUID.hi = 253145895
    playerUUID.lo = 2320618671
    
    local trigger = self.owner:getTrigger()

    trigger:setLuaCallback(function(event, otherUUID)
        if event == "enter" then
            if (otherUUID:isEqual(playerUUID)) then
                removeGameObjectByUUID(self.owner:getUUID())
            end
        end
    end)
end

function behavior:update(dt)
    
    local currentTime = os.clock()
    if (currentTime - lastScreamTime > screamCooldown) then
        lastScreamTime = currentTime
        print("CATCH ME IF YOU CAN!!!")
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior