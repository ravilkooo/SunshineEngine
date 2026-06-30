behavior = {}
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
                print("Trigger enter", otherUUID.hi, otherUUID.lo)
            elseif event == "exit" then
                innerObjects[otherUUID.hi] = nil
                print("Trigger exit", otherUUID.hi, otherUUID.lo)
            end
        end)
    end
end

function behavior:update(dt)
    for hi, phys in pairs(innerObjects) do
        phys:addForce(Vector3.new(0,100000,0))
    end
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
