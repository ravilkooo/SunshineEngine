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

    local beh = self.owner:getBehavior()
    if (beh) then
        print("Behavior found")
        print("int = " .. beh:MB_getInt("int"))
        print("change = " .. beh:MB_getInt("change"))
        print("fl = " .. beh:MB_getFloat("fl"))
        if (beh:MB_getBool("fls")) then
            print("fls = 1")
        else
            print("fls = 0")
        end
        if (beh:MB_getBool("true")) then
            print("true = 1")
        else
            print("true = 0")
        end
        print("str = " .. beh:MB_getString("str"))
        local vec = beh:MB_getVector3("vec")
        print("vec = " .. vec.x .. ", " .. vec.y .. ", " .. vec.z)
        print("uuid = " .. beh:MB_getUUID("uuid").hi .. ", " .. beh:MB_getUUID("uuid").lo)

    else
        print("No behavior found")
    end
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
