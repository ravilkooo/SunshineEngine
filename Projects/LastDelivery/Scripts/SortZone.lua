behavior = {}
local magnetForce = 10000
local innerObjects = {}
local forceDirections = {}


function behavior:start()

    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if event == "enter" then
                local obj = getGameObjectByUUID(otherUUID)
                local phys = obj:getPhysics()
                local beh = obj:getBehavior()
                if (beh and phys) then
                    innerObjects[otherUUID.hi] = phys
                    local col = beh:MB_getInt("color")
                    print("Color for object with UUID: " .. otherUUID.hi .. ", " .. otherUUID.lo .. " is: " .. col)
                    if (col) then
                        if (col == 1) then
                            forceDirections[otherUUID.hi] = Vector3.new(1, 0, 0)
                        elseif (col == 2) then
                            forceDirections[otherUUID.hi] = Vector3.new(-1, 0, 0)
                        elseif (col == 3) then
                            forceDirections[otherUUID.hi] = Vector3.new(0, 0, 1)
                        end
                    end
                end
                print("Trigger enter", otherUUID.hi, otherUUID.lo)
            elseif event == "exit" then
                innerObjects[otherUUID.hi] = nil
                forceDirections[otherUUID.hi] = nil
                print("Trigger exit", otherUUID.hi, otherUUID.lo)
            end
        end)
    end
end

function behavior:update(dt)

    for hi, phys in pairs(innerObjects) do
        phys:addForce(forceDirections[hi] * magnetForce)
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
