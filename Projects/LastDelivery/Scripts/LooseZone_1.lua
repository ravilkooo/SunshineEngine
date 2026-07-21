behavior = {}

local playerObj
local playerUUID

local spawnUUID

function behavior:start()

    playerUUID = UUID.new()
    playerUUID.hi = 4011023819
    playerUUID.lo = 3110370002
    
    playerObj = getGameObjectByUUID(playerUUID)

    spawnUUID = UUID.new()
    spawnUUID.hi = 1631968236
    spawnUUID.lo = 1182997423

    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if event == "enter" then
                if (otherUUID:isEqual(playerUUID)) then
                    local spawnPos = getGameObjectByUUID(spawnUUID):getTransform():getAbsolutePosition()
                    local controller = playerObj:getCharacterController()
                    controller:setPosition(spawnPos)
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
