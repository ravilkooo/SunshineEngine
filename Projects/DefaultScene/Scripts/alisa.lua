behavior = {}


local playerCircles = 0
local competitor1Circles = 0
local competitor2Circles = 0

function behavior:start()

    local trigger = self.owner:getTrigger()
    local emitter = self.owner:getParticleEmitter()

    trigger:setLuaCallback(function(event, otherUUID)
        if event == "enter" then
            print("enter")
        end
    end)
end

function behavior:update(dt)

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior