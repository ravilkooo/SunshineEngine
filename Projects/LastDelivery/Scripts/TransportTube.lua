behavior = {}
local magnetForce = 10000
local innerObjects = {}
local forceDir

function behavior:start()

    
    local rot = self.owner:getTransform():getAbsoluteRotation()
    local cx = math.cos(rot.x)
    local sx = math.sin(rot.x)
    local cy = math.cos(rot.y)
    local sy = math.sin(rot.y)
    local cz = math.cos(rot.z)
    local sz = math.sin(rot.z)
    forceDir = Vector3.new()
    forceDir.x = -cy * sz
    forceDir.y = cy * cz + sx * sy * sz
    forceDir.z = cx * sy * sz - sx * cz
    print("Force dir: " .. forceDir.x .. ", " .. forceDir.y .. ", " .. forceDir.z)

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
        phys:addForce(forceDir * magnetForce)
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
