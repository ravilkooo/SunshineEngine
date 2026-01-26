behavior = {}

function behavior:start()
    self.baseEmissionRate = 20.0
    self.minEmissionRate = 0.0
end

function behavior:update(dt)
    local emitter = self.owner:getParticleEmitter()
    local physics = self.owner:getPhysics()
    -- print("Emitter and Physics fetched")
    if emitter and physics then
        -- Get current velocity
        local velocity = physics:getLinearVelocity()
        
        -- Calculate speed
        local speed = math.sqrt(
            velocity.x * velocity.x + 
            velocity.y * velocity.y + 
            velocity.z * velocity.z
        )
        
        -- Decrease emission rate with speed
        local emissionRate = self.baseEmissionRate - (speed * 0.5)
        emissionRate = math.max(emissionRate, self.minEmissionRate)
        
        emitter:setEmissionRate(emissionRate)
        
        -- Set direction based on velocity
        if speed > 0.1 then
            local dir = Vector3.new(
                - 2 * velocity.x / speed,
                - 2 * velocity.y / speed,
                - 2 * velocity.z / speed
            )
            emitter:setEmitPosition(dir)
        end
        -- print("Speed: %.2f, Emission Rate: %.2f", speed, emissionRate)
    end
    
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior