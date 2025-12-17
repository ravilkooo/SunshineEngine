behavior = {}


function behavior:start()
    print("[Lua] start, self.id =", self.id)

    -- получаем синглтон PerceptionSystem через глобальную функцию
    local ps = GetPerceptionSystem()

    -- регистрируем две команды / стороны
    local ok1 = ps:registerTeam(1)
    local ok2 = ps:registerTeam(2)
    print("[Lua] registerTeam(1) =", ok1, "registerTeam(2) =", ok2)

    -- -- добавляем, кого первая команда может видеть и кто её слышит
    -- --ps:addSightTargetTeamIDsInTeam(1, { 2 })
    -- --ps:addHearingSourceTeamIDsInTeam(1, { 2 })

    -- привязываем текущий объект к команде 1
    -- local pc = self.owner:getPerception()
    -- if pc ~= nil then
    --     local added = ps:addToTeam(1, pc)
    --     print("[Lua] addToTeam(1, self.owner:getPerception()) =", added)
    -- else
    --     print("[Lua] warning: owner has no PerceptionComponent")
    -- end

    self.time = 0
end


function behavior:update(dt)
    self.time = (self.time or 0) + dt
    if self.time > 1.0 then
        local ps = GetPerceptionSystem()
        -- просто для проверки: чистим цели зрения у команды 1 раз в секунду
        local cleared = ps:clearSightTargetTeamIDsInTeam(1)
        print("[Lua] update, cleared sight targets for team 1 =", cleared, "time =", self.time)
        self.time = 0
    end
end


function behavior:destroy()
    print("[Lua] destroy, self.id =", self.id)

    local ps = GetPerceptionSystem()
    local pc = self.owner:getPerception()
    if pc ~= nil then
        local removed = ps:removeFromTeam(1, pc)
        print("[Lua] removeFromTeam(1, pc) =", removed)
    end
end


return behavior
