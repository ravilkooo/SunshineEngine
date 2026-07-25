local behavior = {}

function enableCharacterParticles()
    local emitter = behavior.self.owner:getParticleEmitter()
    emitter:enableEmission()
end

function disableCharacterParticles()
    local emitter = behavior.self.owner:getParticleEmitter()
    emitter:disableEmission()
end

function behavior:start()
    local emitter = self.owner:getParticleEmitter()
    emitter:disableEmission()
    behavior.self =  self
end

function behavior:update(dt)

    return "success"
end

function behavior:destroy()
    --
end

return behavior
