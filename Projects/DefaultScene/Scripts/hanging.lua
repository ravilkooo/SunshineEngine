behavior = {}

local speedX
local currRotZ
local maxRotZ
local parentUUID
local parentObj
local physics
local startPosition
local sensitivity = 0.03

function behavior:start()
    parentUUID = UUID.new()
    parentUUID.hi = 253145895
    parentUUID.lo = 2320618671

    startPosition = self.owner:getTransform().m_position
    
    parentObj = getGameObjectByUUID(parentUUID)

    physics = parentObj:getPhysics()

    speedX = 0
    currRotZ = 0
    maxRotZ = 30 * 3.14159265 / 180
    -- print("Behavior started", self.id)
end

function behavior:update(dt)
    speedX = physics:getLinearVelocity().x * sensitivity * math.cos(currRotZ)
    currRotZ = math.max(math.min(maxRotZ, speedX), -maxRotZ)
    self.owner:getTransform().m_rotation.z = -currRotZ
    


    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
