local behavior = {}
local color
local innerObjects = {}

function behavior:start()
    
    local beh = self.owner:getBehavior()
    print("beh")
    color = beh:MB_getInt("color")
    print("save color")

    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
                    
            print(otherUUID:toString())
            local item = getGameObjectByUUID(otherUUID)
            local beh1 = item:getBehavior()

            if (beh1) then
                print("beh1")
                if (beh1:MB_hasKey("color")) then
                    local itemColor = beh1:MB_getInt("color")
                    print(itemColor)
                    if (itemColor == color) then
                        print("itemColor == color")
                        print(event)
                        if event == "enter" then
                            incrementColorCount(itemColor)
                            checkZoneColorCount()
                            
                            local phys = item:getPhysics()
                            if (phys) then
                                innerObjects[otherUUID.hi] = phys
                            end
                        elseif event == "exit" then
                            innerObjects[otherUUID.hi] = nil
                            decrementColorCount(itemColor)
                            local phys = item:getPhysics()
                            if (phys:isActive()) then
                                print("exit like active")
                            else
                                print("exit like not active")
                            end
                        end

                    end
                end
            end
            
        end)
    end
end

function behavior:update(dt)
    for hi, phys in pairs(innerObjects) do
        -- phys:activate()
    end
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
