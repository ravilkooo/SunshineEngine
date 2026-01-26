behavior = {}

local speedX
local angleAmplitude
local maxAngleAmplitude
local parentUUID
local parentObj
local physics
local startPosition
local sensitivity = 0.03
local frequency = 4

function behavior:start()
    parentUUID = UUID.new()
    -- paste ParentUUID here
    parentUUID.hi = 253145895
    parentUUID.lo = 2320618671

    startPosition = self.owner:getTransform().m_position
    
    parentObj = getGameObjectByUUID(parentUUID)

    physics = parentObj:getPhysics()

    speedX = 0
    angleAmplitude = 0
    maxAngleAmplitude = 45 * 3.14159265 / 180
    -- print("Behavior started", self.id)
end

function behavior:update(dt)
    
    local currentTime = os.clock()
    
    speedX = physics:getLinearVelocity().x * sensitivity * math.cos(angleAmplitude)
    local angleFraction = math.sin(currentTime * frequency)

    angleAmplitude = math.min(maxAngleAmplitude, math.abs(speedX))
    self.owner:getTransform().m_rotation.z = -angleAmplitude * angleFraction - 0.5 * angleAmplitude
    


    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
