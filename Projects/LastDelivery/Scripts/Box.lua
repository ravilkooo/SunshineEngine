local behavior = {}

local playerUUID
local playerObj
local destroyed = false
local playerIsNear = false
local destroyMoment
local emissionDuration = 0.2
local destroyDuration = 1.0

function behavior:start()
    
    playerUUID = UUID.new()
    playerUUID.hi = 4011023819
    playerUUID.lo = 3110370002
    playerObj = getGameObjectByUUID(playerUUID)

    local emitter = self.owner:getParticleEmitter()
    emitter:disableEmission()

    local trigger = self.owner:getTrigger()


    if (trigger) then
        trigger:setLuaCallback(function(event, otherUUID)
            if not destroyed then
                if event == "enter" then
                    if otherUUID:isEqual(playerUUID) then
                        playerIsNear = true
                        print("Player entered", otherUUID.hi, otherUUID.lo)
                    end
                elseif event == "exit" then
                    if otherUUID:isEqual(playerUUID) then
                        playerIsNear = false
                        print("Player exited", otherUUID.hi, otherUUID.lo)
                    end
                end
            end
        end)
    end

    print(PlayerVar)
end

local function startDestroyTimer(self)
    destroyed = true
    destroyMoment = os.clock()
    local emitter = self.owner:getParticleEmitter()
    emitter:enableEmission()
    local render = self.owner:getRender()
    render.visibility = false
    local physics = self.owner:getPhysics()
    physics:removeBody()
end

function behavior:update(dt)
    if (destroyed) then
        -- print("destroyed")
    else
        -- print("not destroyed")
    end

    if (not destroyed) and playerIsNear then
        if (InputSystem:isPressed("Destroy")) then        
            startDestroyTimer(self)
        end
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
