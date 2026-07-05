behavior = {}
local waitTime = 1.0
local dissapearTime = 1.0
local moveSpeed = 3.0
local moveTime = 1
local currTime
local startTime
local playerObj
local playerPos
local startPos
local playerUUID

local startLen
local startOffset
local startRotation

local finalLen
local finalOffset
local finalRotation

function behavior:start()
    startTime = os.clock()
    playerUUID = UUID.new()
    playerUUID.hi = 4011023819
    playerUUID.lo = 3110370002
    playerObj = getGameObjectByUUID(playerUUID)
    local tr = playerObj:getTransform()
    playerPos = Vector3.new(tr.position)
    print("Player pos: " .. playerPos.x .. " " .. playerPos.y .. " " .. playerPos.z)
    startPos = Vector3.new(self.owner:getTransform().position)
    print("Camera pos: " .. startPos.x .. " " .. startPos.y .. " " .. startPos.z)

    local firstCam = self.owner:getCameraComponent():getCamera()
    local playerCam = playerObj:getCameraComponent():getCamera()

    startLen = firstCam:getSpringArmLength()
    startOffset = firstCam:getSpringArmRootOffset()
    startRotation = firstCam:getSpringArmRotation()
    print("Start len: " .. startLen)
    print("Start offset: " .. startOffset.x .. ", " .. startOffset.y .. ", " .. startOffset.z)
    print("Start rotation: " .. startRotation.x .. ", " .. startRotation.y .. ", " .. startRotation.z)

    finalLen = playerCam:getSpringArmLength()
    finalOffset = playerCam:getSpringArmRootOffset()
    finalRotation = playerCam:getSpringArmRotation()
    print("Final len: " .. finalLen)
    print("Final offset: " .. finalOffset.x .. ", " .. finalOffset.y .. ", " .. finalOffset.z)
    print("Final rotation: " .. finalRotation.x .. ", " .. finalRotation.y .. ", " .. finalRotation.z)

    return
end

function behavior:update(dt)
    currTime = os.clock()
    local tr = self.owner:getTransform()
    local firstCam = self.owner:getCameraComponent():getCamera()
    if (currTime - startTime > waitTime + dissapearTime) and (currTime - startTime < waitTime + dissapearTime + moveTime) then
        local t = (currTime - startTime - waitTime - dissapearTime) / moveTime
        print(t)
        tr.position = startPos + (playerPos - startPos) * t
        firstCam:setSpringArmLength(startLen + (finalLen - startLen) * t)
        firstCam:setSpringArmRootOffset(startOffset + (finalOffset - startOffset) * t)
        firstCam:setSpringArmRotation(startRotation + (finalRotation - startRotation) * t)

    elseif (currTime - startTime > waitTime + dissapearTime + moveTime) then
        print("Setting camera to player")
        setCameraByUUID(playerUUID)
        removeGameObjectByUUID(self.owner:getUUID())
        local char = playerObj:getCharacterComponent()
        char.isPlayerControlled = true
    end

    return "success"
end

function behavior:destroy()
    return
end

return behavior
