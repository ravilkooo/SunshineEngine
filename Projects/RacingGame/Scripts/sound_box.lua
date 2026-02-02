behavior = {}

function behavior:start()
    local audio = getAudioSystem()
    local transform = self.owner:getTransform()

    --audio:play3D("drumloop", transform.m_position.x, transform.m_position.y, transform.m_position.z, 0.2, 0.0, 0.0)
    audio:play3D("drumloop", 0, 0, 0, 0.2, 2.0, 10.0)
    audio:setSourcePosition("drumloop", 0, 0, 0)
end

function behavior:update(dt)
    local audio = getAudioSystem()
    local player = getPlayerObject()
    local transform = player:getTransform()

    audio:setListenerPosition(transform.m_position.x, transform.m_position.y, transform.m_position.z)
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior