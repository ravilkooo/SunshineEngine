local behavior = {}
local firstPos
local secondPos
local routePart
local routeLen
local destination
local speed

function behavior:start()
    local beh = self.owner:getBehavior()
    if (beh) then
        print("Platform behavior found")
        speed = beh:MB_getFloat("speed")
        print("speed = " .. speed)
        firstPos = beh:MB_getVector3("Position1")
        secondPos = beh:MB_getVector3("Position2")
        destination = 2
        routePart = 0
        print("Before len")
        routeLen = (firstPos - secondPos):length()
        print("After len")
    else
        print("Platform No behavior found")
    end
end

local function switchDestination(self)
    local beh = self.owner:getBehavior()
    if (destination == 1) then
        destination = 2
        firstPos = beh:MB_getVector3("Position1")
        secondPos = beh:MB_getVector3("Position2")
    else
        destination = 1
        firstPos = beh:MB_getVector3("Position2")
        secondPos = beh:MB_getVector3("Position1")
    end
    routePart = 0
end

function behavior:update(dt)

    routePart = routePart + speed / routeLen
    routePart = math.min(1, routePart)
    local destinationPos = firstPos +  (secondPos - firstPos) * routePart

    if (routePart == 1) then
        switchDestination(self)
    end

    local ph = self.owner:getPhysics()

    ph:moveKinematicPosition(destinationPos, dt)

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
