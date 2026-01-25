behavior = {}

function behavior:start()
    self.speedY = 1000.0
    self.checkTechnique = false
    -- print("Behavior started", self.id)
end

local lastJumpTime = 4
local jumpCooldown = 2

function behavior:update(dt)
    local physics = self.owner:getPhysics()

    local currentTime = os.clock()

    -- print("currentTime: " .. currentTime .. ", lastJumpTime: " .. lastJumpTime .. ", cooldown: " .. jumpCooldown)

    if (currentTime - lastJumpTime > jumpCooldown) and (math.abs(physics:getLinearVelocity().y) <= 0.0001) then
        lastJumpTime = currentTime
        local up = Vector3.new(0, self.speedY * 50, 0)
        physics:addImpulse(up)
        -- print("Jump!")
    end

    -- physics:addForce(Vector3.new(0,self.speedY,0))
    -- print("Behavior update", self.id)
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior