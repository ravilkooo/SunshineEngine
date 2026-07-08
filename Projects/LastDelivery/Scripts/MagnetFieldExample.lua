behavior = {}
local magnetForce = 1000000
local innerObjects = {}

function behavior:start()


    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if event == "enter" then
                local obj = getGameObjectByUUID(otherUUID)
                local phys = obj:getPhysics()
                if (phys) then
                    innerObjects[otherUUID.hi] = phys
                end
                print("Platform Trigger enter", otherUUID.hi, otherUUID.lo)
            elseif event == "exit" then
                innerObjects[otherUUID.hi] = nil
                print("Platform Trigger exit", otherUUID.hi, otherUUID.lo)
            end
        end)
    end
end

function behavior:update(dt)

    local turnMagnet = InputSystem:getAxis("TurnMagnet")

    for hi, phys in pairs(innerObjects) do
        phys:addForce(Vector3.new(0, magnetForce * turnMagnet,0))
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
