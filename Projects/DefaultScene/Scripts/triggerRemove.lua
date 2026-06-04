behavior = {}

local playerUUID
local playerObj
local lastScreamTime = 4
local screamCooldown = 2
local playerStartPos

function behavior:start()

    playerUUID = UUID.new()
    playerUUID.hi = 851949874
    playerUUID.lo = 907375339
    playerObj = getGameObjectByUUID(playerUUID)
    playerStartPos = playerObj:getTransform():getAbsolutePosition()

    local trigger = self.owner:getTrigger()

    trigger:setLuaCallback(function(event, otherUUID)
        if event == "enter" then
            if (otherUUID:isEqual(playerUUID)) then
                removeGameObjectByUUID(self.owner:getUUID())
                local physics = playerObj:getPhysics()
                physics:setPosition(playerStartPos)
                -- physics:setGravityFactor(0.1)
                local audio = getAudioSystem()
                audio:play("pause")
            end
        end
    end)
end

function behavior:update(dt)
    
    local currentTime = os.clock()
    if (currentTime - lastScreamTime > screamCooldown) then
        lastScreamTime = currentTime
        -- local audio = getAudioSystem()
        -- audio:play("jaguar")
        -- print("CATCH ME IF YOU CAN!!!")
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior