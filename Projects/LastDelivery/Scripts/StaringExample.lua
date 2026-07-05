behavior = {}

local chasedPlayerUUID
local chasedPlayer
local stareAtPlayer = false

local function TestPerceptionSystem(self)
    local ps = getPerceptionSystem()

    local teamPlayer = 1
    local teamEnemy = 2
    local invalidTeam = 0xFFFFFFFF

    -- =========================
    -- REGISTER
    -- =========================

    ps:registerTeam(teamPlayer)
    ps:registerTeam(teamEnemy)
    ps:registerTeam(invalidTeam)
    
    ps:addSightTargetTeamIDsInTeam(teamEnemy, teamPlayer)
    local perception = self.owner:getPerception()
    ps:addToTeam(teamEnemy, perception)

    local sightCbId = perception:addSightCallback(
        function(targetId, newCond, inYaw)
            local newObj = getGameObjectByUUID(targetId)
            local newChar = newObj:getCharacterComponent()
            if (newChar == nil) then
                print("Target has no character component")
                return
            end

            print("I can see", targetId.hi, targetId.lo, newCond, inYaw)
            stareAtPlayer = newCond
            if (newCond) then
                chasedPlayer = newObj
                chasedPlayerUUID = targetId
                local char = self.owner:getCharacterComponent()                       
                char.yaw = char.yaw + inYaw
            end
        end)
end


function behavior:start()
    TestPerceptionSystem(self)
end

function behavior:update(dt)

    local char = self.owner:getCharacterComponent()
    if (stareAtPlayer) then
        -- print(char.yaw)
        local chasedPlayerPos = chasedPlayer:getTransform().position
        local dir = chasedPlayerPos - self.owner:getTransform().position
        local absoluteYaw = math.atan(dir.x, dir.z)
        char.yaw = absoluteYaw
        -- float ViewYaw = std::acosf(ViewerForward.Dot(DirNorm));
        -- ViewYaw = ViewYaw * (ViewerForward.Cross(DirNorm).y > 0 ? 1 : -1);

    else
        -- local char = self.owner:getCharacterComponent()
    end

    -- char.moveInput = Vector2.new(inputValue.y, inputValue.x)

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
