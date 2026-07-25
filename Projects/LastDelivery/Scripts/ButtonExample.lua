local behavior = {}
local pressed = false
local gate1
local gate2
local firstPos
local secondPos

local pushAnimation = false
local endAnimation = false

local routePart
local routeLen

local speed = 0.005

local function pushButton(self)
    local tr = self.owner:getTransform()
    firstPos = tr.localPosition
    secondPos = Vector3.new(firstPos)
    secondPos.y = firstPos.y - 0.2
    routeLen = (firstPos - secondPos):length()
    print("routeLen = " .. routeLen)
    routePart = 0
    pushAnimation = true
end

function behavior:start()

    local beh = self.owner:getBehavior()
    gate1 = beh:MB_getUUID("gate1")
    gate2 = beh:MB_getUUID("gate2")

    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if event == "enter" then
                pressed = true

                local gate1_obj = getGameObjectByUUID(gate1)
                local gate2_obj = getGameObjectByUUID(gate2)

                local beh1 = gate1_obj:getBehavior()
                beh1:MB_setBool("startReq", true)
                
                local beh2 = gate2_obj:getBehavior()
                beh2:MB_setBool("startReq", true)

                pushButton(self)
            end
        end)
    end
end

function behavior:update(dt)
    if (pushAnimation and not endAnimation) then

        routePart = routePart + speed / routeLen
        routePart = math.min(1, routePart)
        local destinationPos = firstPos + (secondPos - firstPos) * routePart

        local tr = self.owner:getTransform()
        tr.localPosition = destinationPos

        if (routePart == 1) then
            endAnimation = true
        end
    end
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
