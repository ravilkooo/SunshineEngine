behavior = {}

local startPosition
local startScale

local playerUUID
local playerObj
local collected = false
local rotSpeed = 10
local radius = 3
local upOffset = 0.6
local newScale
-- can be made unique for every object
-- for example: based on initial position of object or time
local angleOffet = 0.2

-- Camera effect
local baseStickLength = 15
local stcikLenMaxDelta = 10
local stcikLenFreq = 5
local camera

-- Effect Duration
local effectStartTime
local effectDuration = 3

function behavior:start()
    tr = self.owner:getTransform()
    startScale = Vector3.new(tr.m_scaleFactor.x, tr.m_scaleFactor.y, tr.m_scaleFactor.z)
    newScale = Vector3.new(0.3, 0.3, 0.3)
    startPosition = Vector3.new(tr.m_position.x, tr.m_position.y, tr.m_position.z)

    playerObj = getPlayerObject()
    camera = playerObj:getCamera()
    baseStickLength = camera:getStickLength()
    stcikLenMaxDelta = math.min(stcikLenMaxDelta, baseStickLength * 0.95)
    
    playerUUID = playerObj:getUUID()
    local trigger = self.owner:getTrigger()

    trigger:setLuaCallback(function(event, otherUUID)
        if not collected then
            if event == "enter" then
                effectStartTime = os.clock()
                if otherUUID:isEqual(playerUUID) then
                    collected = true
                    angleOffet = self.owner:getTransform().m_position.x
                    self.owner:getTransform().m_scaleFactor = newScale
                    setGlobalGravity(Vector3.new(0,-1,0))
                    local audio = getAudioSystem()
                    audio:play("drumloop")
                end
            end
        end
    end)
    -- print("Behavior started", self.id
end

function behavior:update(dt)
    
    if collected then
        local currentTime = os.clock()

        if (currentTime - effectStartTime < effectDuration) then

            local playerPos = playerObj:getTransform():getAbsolutePosition()

            local newPos = Vector3.new(
                playerPos.x + radius * math.cos(angleOffet + rotSpeed * currentTime),
                playerPos.y + upOffset,
                playerPos.z + radius * math.sin(angleOffet + rotSpeed * currentTime))

            self.owner:getTransform().m_position = newPos

            camera:setStickLength(baseStickLength + stcikLenMaxDelta * math.sin(stcikLenFreq * currentTime))
        else
            collected = false
            self.owner:getTransform().m_position = Vector3.new(
                startPosition.x, startPosition.y, startPosition.z
            )
            self.owner:getTransform().m_scaleFactor = Vector3.new(
                startScale.x, startScale.y, startScale.z
            )
            setGlobalGravity(Vector3.new(0,-9.8,0))
            local audio = getAudioSystem()
            audio:stop("drumloop")
        end
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
