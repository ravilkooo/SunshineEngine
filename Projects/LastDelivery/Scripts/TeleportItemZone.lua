local behavior = {}

local spawnPos

function behavior:start()
    
    local beh = self.owner:getBehavior()
    spawnPos = beh:MB_getVector3("spawnPos")

    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if event == "enter" then
                local obj = getGameObjectByUUID(otherUUID)
                local phys = obj:getPhysics()
                if (phys) then
                    phys:setPosition(spawnPos)
                    print("teleported " .. otherUUID:toString() .. " to pos " .. spawnPos.x .. ", " .. spawnPos.y .. ", " .. spawnPos.z)
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
