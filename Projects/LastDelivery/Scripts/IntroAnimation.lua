behavior = {}

local playerUUID
local playerObj
local floatingSpeed = 6.0
local floatingAmp = 0.1
local moveSpeed = 0.5
local animationDuration = 1.0
local yMidLevel
local startX = -4.0
local startTime

function behavior:start()
    local tr = self.owner:getTransform()
    yMidLevel = tr.m_localPosition.y
    tr.m_position.x = startX
    moveSpeed = -2.0 * startX / animationDuration

    startTime = os.clock()
end

function behavior:update(dt)
    local tr = self.owner:getTransform()
    tr.m_localPosition.y = yMidLevel +  floatingAmp * math.sin(os.clock() * floatingSpeed)
    tr.m_position.x = tr.m_position.x + moveSpeed * dt

    local currTime = os.clock()
    if (currTime - startTime > animationDuration) then
        removeGameObjectByUUID(self.owner:getUUID())
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
