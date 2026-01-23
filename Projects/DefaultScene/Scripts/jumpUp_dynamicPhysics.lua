behavior = {}

function behavior:start()
    self.speedY = 1000.0
    self.checkTechnique = false
    -- print("Behavior started", self.id)
end

local lastJumpTime = 0
local jumpCooldown = 0.8

function behavior:update(dt)
    local physics = self.owner:getPhysics()

    local currentTime = os.clock()

    -- print("currentTime: " .. currentTime .. ", lastJumpTime: " .. lastJumpTime .. ", cooldown: " .. jumpCooldown)

    if currentTime - lastJumpTime > jumpCooldown then
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