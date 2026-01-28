behavior = {}

local targetPoints = {}
local basePosition
local targetPosition
local done = false

local playerUUID
local competitor1UUID
local competitor2UUID

local moveSpeed = 500
local returnSpeed = 50

local audio

function behavior:start()
    local transform = self.owner:getTransform()
    basePosition = Vector3.new(transform.m_position.x, transform.m_position.y, transform.m_position.z)
    
    playerUUID = UUID.new()
    playerUUID.hi = 94714652
    playerUUID.lo = 1299782790

    competitor1UUID = UUID.new()
    competitor1UUID.hi = 2331460130
    competitor1UUID.lo = 3670470622

    competitor2UUID = UUID.new()
    competitor2UUID.hi = 1289409358
    competitor2UUID.lo = 567470455

    --perception
    local perceptionSystem = getPerceptionSystem()
    perceptionSystem:registerTeam(1)
    perceptionSystem:addSightTargetTeamIDsInTeam(1, 2)

    local perception = self.owner:getPerception()
    perceptionSystem:addToTeam(1, perception)

    local cbId = perception:addSightCallback(function(targetId, newCondition)
        if (newCondition == true) then
            table.insert(targetPoints, targetId:toString())
        else
            local index = 1
            for idx, value in pairs(targetPoints) do
                if (value == targetId:toString()) then
                    index = idx
                end
            end
            table.remove(targetPoints, index)
        end
    end)
end

function behavior:update(dt)
    local transform = self.owner:getTransform()
    local physics = self.owner:getPhysics()
    local currentLocation = Vector3.new(transform.m_position.x, transform.m_position.y, transform.m_position.z)

    --floating
    local physics = self.owner:getPhysics()
    currentTime = os.clock()
    if (currentTime % 1 < 0.5) then
        local up = Vector3.new(0, 160, 0)
        physics:addImpulse(up)
    end
    print(done)
    if (#targetPoints > 0 and done == false) then
        --moving towards target
        local target
        if (targetPoints[1] == playerUUID:toString()) then
            target = getGameObjectByUUID(playerUUID)
        elseif (targetPoints[1] == competitor1UUID:toString()) then
            target = getGameObjectByUUID(competitor1UUID)
        elseif (targetPoints[1] == competitor2UUID:toString()) then
            target = getGameObjectByUUID(competitor2UUID)
        end

        if not target then return end

        local targetTransform = target:getTransform()
        local forward = Vector3.new(targetTransform.m_position.x - currentLocation.x, targetTransform.m_position.y - currentLocation.y, targetTransform.m_position.z - currentLocation.z)
        local forwardLength = math.sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z)
        forward = Vector3.new((forward.x / forwardLength) * moveSpeed, (forward.y / forwardLength) * moveSpeed, (forward.z / forwardLength) * moveSpeed)
        physics:addImpulse(forward)

        --checking if close enought
        local dx = currentLocation.x - targetTransform.m_position.x
        local dy = currentLocation.y - targetTransform.m_position.y
        local dz = currentLocation.z - targetTransform.m_position.z
        local distance = math.sqrt(dx * dx + dy * dy + dz * dz)
        if (distance < 6.0) then           
            local targetPhysics = target:getPhysics()
            local noiseVector = Vector3.new(24476, -24476, 24476)
            local angularVector = Vector3.new(15000.0, 15000.0, 15000.0)
            targetPhysics:setLinearVelocity(Vector3.new(0.0, 0.0, 0.0))
            targetPhysics:addImpulse(noiseVector)
            targetPhysics:addAngularImpulse(angularVector)
            physics:setLinearVelocity(Vector3.new(0.0, 0.0, 0.0))
            done = true
        end

    else 
        print("returning")
        local dx = currentLocation.x - basePosition.x
        local dy = currentLocation.y - basePosition.y
        local dz = currentLocation.z - basePosition.z
        local distance = math.sqrt(dx * dx + dy * dy + dz * dz)

        if (distance > 8.0) then
            local forward = Vector3.new(basePosition.x - currentLocation.x, basePosition.y - currentLocation.y, basePosition.z - currentLocation.z)
            local forwardLength = math.sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z)
            forward = Vector3.new((forward.x / forwardLength) * returnSpeed, (forward.y / forwardLength) * returnSpeed, (forward.z / forwardLength) * returnSpeed)
            physics:addImpulse(forward)
        else
            done = false
        end
    end

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior