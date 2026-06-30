behavior = {}

local playerUUID
local playerObj

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
    ps:addToTeam(teamPlayer, perception)
end


function behavior:start()
    TestPerceptionSystem(self)
end

function behavior:update(dt)
    local char = self.owner:getCharacterComponent()

    -- char.m_moveInput = Vector2.new(inputValue.y, inputValue.x)

    return "success"
end

function behavior:destroy()
    print("Destroyed", self.id)
end

return behavior
