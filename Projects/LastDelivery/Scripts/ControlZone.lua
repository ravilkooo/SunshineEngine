behavior = {}

local playerObj
local playerUUID

local spawnUUID

function behavior:start()

    playerUUID = UUID.new()
    playerUUID.hi = 4011023819
    playerUUID.lo = 3110370002

    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if event == "enter" then
                if (otherUUID:isEqual(playerUUID)) then
                    
                    local player = getGameObjectByUUID(playerUUID)
                    local char = player:getCharacterComponent()
                    
                    char.isPlayerControlled = false
                    char.moveInput = Vector2.new(0,0)
                    
                    local beh = self.owner:getBehavior()
                    local cameraUUID = beh:MB_getUUID("cameraUUID")
                    setCameraByUUID(cameraUUID)
                    enableMagnetControl()

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
