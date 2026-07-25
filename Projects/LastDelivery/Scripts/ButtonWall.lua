local behavior = {}
local pressed = false
local wall
local firstPos
local secondPos

local pushAnimation = false
local endAnimation = false

local routePart
local routeLen

local speed = 0.005

local function startAnimation(self)
    pressed = true
    local tr = self.owner:getTransform()
    firstPos = tr.localPosition
    secondPos = Vector3.new(firstPos)

    local beh = self.owner:getBehavior()
    local depth = beh:MB_getFloat("pressDepth")

    secondPos.y = firstPos.y - depth
    routeLen = (firstPos - secondPos):length()
    print("routeLen = " .. routeLen)
    routePart = 0
    pushAnimation = true
end

local function pushButton(self)
    local wall_obj = getGameObjectByUUID(wall)
    
    local beh1 = wall_obj:getBehavior()
    beh1:MB_setBool("startReq", true)
    
    startAnimation(self)
end

function behavior:start()

    local beh = self.owner:getBehavior()
    wall = beh:MB_getUUID("wallUUID")

    local trigger = self.owner:getTrigger()

    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if event == "enter" then
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
