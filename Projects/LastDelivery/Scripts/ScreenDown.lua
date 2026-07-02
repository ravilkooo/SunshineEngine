behavior = {}
local waitTime = 4.5
local moveSpeed = 0.5
local dissapearTime = 2
local currTime
local startTime

function behavior:start()
    startTime = os.clock()
    return
end

function behavior:update(dt)
    currTime = os.clock()
    local tr = self.owner:getTransform()
    if (currTime - startTime < waitTime) then
        -- just wait
    elseif (currTime - startTime < waitTime + dissapearTime) then
        tr.m_localPosition.z = tr.m_localPosition.z - moveSpeed * (currTime - startTime - waitTime)
    else
        removeGameObjectByUUID(self.owner:getUUID())
    end

    return "success"
end

function behavior:destroy()
    return
end

return behavior
