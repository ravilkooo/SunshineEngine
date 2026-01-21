behavior = {}

function behavior:start()
    self.speedY = 1000.0
    self.checkTechnique = false
    -- print("Behavior started", self.id)
end

local lastEmitTime = 0
local emitCooldown = 2

function behavior:update(dt)
    local physics = self.owner:getPhysics()

    local currentTime = os.clock()

    -- print("currentTime: " .. currentTime .. ", lastEmitTime: " .. lastEmitTime .. ", cooldown: " .. emitCooldown)

    local emitter = self.owner:getParticleEmitter()
    if emitter then
        local newDir = Vector3.new(0, math.sin(currentTime*5), math.cos(currentTime*5))
        emitter:setEmitDir(newDir)
        
        if currentTime - lastEmitTime > emitCooldown then
            lastEmitTime = currentTime
            local isEnabled = emitter:isEnabled()
            if isEnabled then
                emitter:disableEmission()
            else
                emitter:enableEmission()
            end
            -- print("Jump!")
        end
    end

    -- physics:addForce(Vector3.new(0,self.speedY,0))
    -- print("Behavior update", self.id)
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior