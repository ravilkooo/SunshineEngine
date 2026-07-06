behavior = {}
local magnetForce = 1000000
local takenObject
local playerUUID = UUID.new()
local playerObj
local playerChar

function behavior:start()
    playerUUID.hi = 4011023819
    playerUUID.lo = 3110370002
    playerObj = getGameObjectByUUID(playerUUID)
    playerChar = playerObj:getCharacterComponent()

    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if event == "enter" and not takenObject then
                local obj = getGameObjectByUUID(otherUUID)
                local phys = obj:getPhysics()
                if (phys) then
                    takenObject = phys
                end
                print("Magnet Trigger enter", otherUUID.hi, otherUUID.lo)
            elseif event == "exit" then
                takenObject = nil
                print("Magnet Trigger exit", otherUUID.hi, otherUUID.lo)
            end
        end)
    end
end

function behavior:update(dt)

    local turnMagnet = InputSystem:getAxis("TurnMagnet")

    if takenObject then
        local playerYaw = playerChar.yaw
        local forceDir = Vector3.new(math.sin(playerYaw), 0, math.cos(playerYaw))
        takenObject:addForce(forceDir * magnetForce * turnMagnet)
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
