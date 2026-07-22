local behavior = {}

local playerUUID
local playerObj
local collected = false
local rotSpeed = 10
local destroyed = false
local destroyMoment
local emissionDuration = 0.2
local destroyDuration = 1.0

PlayerVar = "SomeVar" -- this is a global variable, it can be accessed from other scripts

-- can be made unique for every object
-- for example: based on initial position of object or time
local angleOffset = 0.2


local function startDestroyTimer(self)
    destroyed = true
    destroyMoment = os.clock()
    local emitter = self.owner:getParticleEmitter()
    emitter:enableEmission()
    local render = self.owner:getRender()
    render.visibility = false
end

function behavior:start()

    playerUUID = UUID.new()
    playerUUID.hi = 4011023819
    playerUUID.lo = 3110370002
    playerObj = getGameObjectByUUID(playerUUID)

    local emitter = self.owner:getParticleEmitter()
    emitter:disableEmission()

    local trigger = self.owner:getTrigger()

    trigger:setLuaCallback(function(event, otherUUID)
        if not collected then
            if event == "enter" then
                if otherUUID:isEqual(playerUUID) then
                    print("OI HUE!")
                    collected = true
                    startDestroyTimer(self)
                    -- removeGameObjectByUUID(self.owner:getUUID())
                end
            end
        end
    end)
end

function behavior:update(dt)
    
    if not collected then

        local currentTime = os.clock()
        self.owner:getTransform().rotation = Vector3.new(0, rotSpeed * currentTime, 0)
        
    elseif (destroyed) and (os.clock() - destroyMoment >= emissionDuration) then
        local emitter = self.owner:getParticleEmitter()
        emitter:setEmissionRate(0)
    end

    if (destroyed) and (os.clock() - destroyMoment >= destroyDuration) then
        removeGameObjectByUUID(self.owner:getUUID())
        print("== Destroy! ==")
    end

    return "success"
end

function behavior:destroy()
    -- print("Destroyed", self.id)
end

return behavior
