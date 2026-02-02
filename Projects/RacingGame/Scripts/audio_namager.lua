behavior = {}

function behavior:start()
    --local audio = getAudioSystem()
    --audio:play("drumloop")
end

function behavior:update(dt)
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior