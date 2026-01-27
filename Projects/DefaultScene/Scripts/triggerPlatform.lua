behavior = {}

local startPosition
local topPosition
local platformUUID
local platformObj
local upSpeed
local bodiesOnPlatform = 0

function behavior:start()

    platformUUID = UUID.new()
    platformUUID.hi = 3086971655
    platformUUID.lo = 2539245992

    platformObj = getGameObjectByUUID(platformUUID)

    startPosition = platformObj:getTransform():getAbsolutePosition()
    topPosition = Vector3.new(startPosition.x + 10, startPosition.y, startPosition.z)

    upSpeed = 3;

    bodiesOnPlatform = 0;
    
    local trigger = self.owner:getTrigger()

    trigger:setLuaCallback(function(event, otherUUID)
        if event == "enter" then
            bodiesOnPlatform = bodiesOnPlatform + 1;
            print("Object entered trigger:", otherUUID:toString(), ", bodiesOnPlatform = ", bodiesOnPlatform)
        elseif event == "exit" then
            if bodiesOnPlatform > 0 then
                bodiesOnPlatform = bodiesOnPlatform - 1
            else
                bodiesOnPlatform = 0
            end
            print("Object left trigger:", otherUUID:toString(), ", bodiesOnPlatform = ", bodiesOnPlatform)
        end
    end)
end

function behavior:update(dt)
    local platTransform = platformObj:getTransform()
    local platTransformY = platTransform:getAbsolutePosition().x
    -- platTransform.m_position - return local if has parent. So we use getAbsolutePosition() to be sure it's absolute world position

    if (bodiesOnPlatform > 0 and platTransformY < topPosition.x) then
        platTransform.m_position.x = platTransform.m_position.x + upSpeed * dt
    end

    if (bodiesOnPlatform == 0 and platTransformY > startPosition.x) then
        platTransform.m_position.x = platTransform.m_position.x - upSpeed * dt
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior