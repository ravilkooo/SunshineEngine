behavior = {}

local playerUUID
local competitor1UUID
local competitor2UUID

local playerCircles = 0
local competitor1Circles = 0
local competitor2Circles = 0

local playerPlace = 1
local number1MeshUUID
local number2MeshUUID
local number3MeshUUID
local wordMeshUUID

function finish() 
    local audio = getAudioSystem()
    audio:play("finish")
    print(playerPlace)
    local numberMesh
    if (playerPlace == 1) then
        numberMesh = getGameObjectByUUID(number1MeshUUID)      
    elseif (playerPlace == 2) then
        numberMesh = getGameObjectByUUID(number2MeshUUID)
    elseif (playerPlace == 3) then
        numberMesh = getGameObjectByUUID(number3MeshUUID)
    end

    local numberTransform = numberMesh:getTransform()
    numberTransform.m_position.x = -28.00
    numberTransform.m_position.y = 25
    numberTransform.m_position.z = -39.50

    local wordMesh = getGameObjectByUUID(wordMeshUUID)
    local wordTransform = wordMesh:getTransform()
    wordTransform.m_position.x = -28.00
    wordTransform.m_position.y = 17
    wordTransform.m_position.z = -39.50
end

function behavior:start()

    playerUUID = UUID.new()
    playerUUID.hi = 94714652
    playerUUID.lo = 1299782790

    competitor1UUID = UUID.new()
    competitor1UUID.hi = 2331460130
    competitor1UUID.lo = 3670470622

    competitor2UUID = UUID.new()
    competitor2UUID.hi = 1289409358
    competitor2UUID.lo = 567470455

    number1MeshUUID = UUID.new()
    number1MeshUUID.hi = 3033643487
    number1MeshUUID.lo = 629990738

    number2MeshUUID = UUID.new()
    number2MeshUUID.hi = 852556085
    number2MeshUUID.lo = 3882781984

    number3MeshUUID = UUID.new()
    number3MeshUUID.hi = 3483802944
    number3MeshUUID.lo = 976720698

    wordMeshUUID = UUID.new()
    wordMeshUUID.hi = 2934410086
    wordMeshUUID.lo = 3532126048

    local trigger = self.owner:getTrigger()
    local emitter = self.owner:getParticleEmitter()

    trigger:setLuaCallback(function(event, otherUUID)
        if event == "enter" then
            if (otherUUID:toString() == "406796334091803782") then
                playerCircles = playerCircles + 1
                if (playerCircles == 3) then
                    finish()
                    emitter:setEmissionRate(40.0)
                end 
            elseif (otherUUID:toString() == "10013545013948379102") then
                competitor1Circles = competitor1Circles + 1
                if (competitor1Circles == 3) then
                    playerPlace = playerPlace + 1
                end
            elseif (otherUUID:toString() == "5537971024333826423") then
                competitor2Circles = competitor2Circles + 1
                if (competitor2Circles == 3) then
                    playerPlace = playerPlace + 1
                end
            end
        end
    end)
end

function behavior:update(dt)

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior