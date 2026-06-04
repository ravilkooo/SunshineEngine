behavior = {}

local playerUUID
local playerObj

-- can be made unique for every object
-- for example: based on initial position of object or time
local angleOffet = 0.2

function behavior:start()
    
    playerUUID = UUID.new()
    playerUUID.hi = 851949874
    playerUUID.lo = 907375339
    playerObj = getGameObjectByUUID(playerUUID)
    
    local trigger = self.owner:getTrigger()
    playerUUID:toString()
    print("Ground behavior started", self.id)
    
    trigger:setLuaCallback(function(event, otherUUID)
        if event == "enter" then
            -- print("Object entered trigger")
            -- local newUUID = playerUUID:toString()
            -- print("Object entered trigger: " otherUUID:toString())
            if otherUUID:isEqual(playerUUID) then
                local bc = playerObj:getBehavior()
                local physics = playerObj:getPhysics()
                bc:MB_setBool("isGrounded", true)
                physics:setLinearVelocity(Vector3.new(velocity.x, 0, velocity.z))
                bc:MB_setFloat("velocityY", 0.0)
                print("Player on ground")
            end
        end
    end)
    print("Ground behavior started", self.id)
end

function behavior:update(dt)
    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
