local behavior = {}

local spawnPos
local countItems = 0
local bridge

local function showBridge(self)
    local bridge_obj = getGameObjectByUUID(bridge)
    
    local beh1 = bridge_obj:getBehavior()
    beh1:MB_setBool("startReq", true) 
end

function behavior:start()
    
    local beh = self.owner:getBehavior()
    spawnPos = beh:MB_getVector3("spawnPos")
        
    bridge = beh:MB_getUUID("bridge")

    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if event == "enter" then
                local obj = getGameObjectByUUID(otherUUID)
                local phys = obj:getPhysics()
                if (phys) then
                    phys:setPosition(spawnPos)
                    print("teleported " .. otherUUID:toString() .. " to pos " .. spawnPos.x .. ", " .. spawnPos.y .. ", " .. spawnPos.z)
                    countItems = countItems + 1

                    if (countItems == 9) then
                        showBridge(self)
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
