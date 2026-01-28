behavior = {}

-- local tracePoints = {}
local playerUUID
local playerObj

local moveSpeed = 3500
local slowSpeed = 200
local rotSpeed = 1

local started = false

function rotateToObjDirection(self, obj)
    local playerPos = obj:getTransform():getAbsolutePosition()
    
    local tf = self.owner:getTransform()
    local enemyPos = tf:getAbsolutePosition()
    local enemyYaw = self.owner:getTransform().m_rotation.y

    -- Direction on XZ plane
    local dx = playerPos.x - enemyPos.x
    local dz = playerPos.z - enemyPos.z

    -- Engine rotations are Vector3, interpret Y as yaw in radians
    local newYaw = math.atan(dx, dz)

    local tf = self.owner:getTransform()

    self.owner:getTransform().m_rotation.y = newYaw
end

function behavior:start()
    
    playerObj = getPlayerObject()
    playerUUID = playerObj:getUUID()

    local perceptionSystem = getPerceptionSystem()
    perceptionSystem:registerTeam(1)

    local perception = self.owner:getPerception()
    perceptionSystem:addToTeam(1, perception)
end

function behavior:update(dt)
    
    rotateToObjDirection(self, playerObj)

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior