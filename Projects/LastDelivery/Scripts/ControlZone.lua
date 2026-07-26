local behavior = {}


local sortZoneColorCount = { 0, 0, 0 }
local reqSortZoneColorCount = { 2, 2, 1 }

local finishedTask = false

function incrementColorCount(color)
    sortZoneColorCount[color] = sortZoneColorCount[color] + 1
    print("colors: " .. sortZoneColorCount[1] .. ", " .. sortZoneColorCount[2] .. ", " .. sortZoneColorCount[3])
end

function decrementColorCount(color)
    sortZoneColorCount[color] = sortZoneColorCount[color] - 1
    print("colors: " .. sortZoneColorCount[1] .. ", " .. sortZoneColorCount[2] .. ", " .. sortZoneColorCount[3])
end

function checkZoneColorCount()
    local res = true
    for i = 1, 3 do
        res = res and (sortZoneColorCount[i] == reqSortZoneColorCount[i])
    end
    if (res) then
        finishedTask = true
        disableMagnetControl()
        setCameraByUUID(playerUUID)
        local player = getGameObjectByUUID(playerUUID)
        local char = player:getCharacterComponent()
        char.isPlayerControlled = true
    end
    return res
end

function behavior:start()
    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if not finishedTask and (event == "enter") then
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
