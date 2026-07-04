behavior = {}

function behavior:start()
    self.owner:getBehavior():MB_setInt("color", 2)
    print("Color set to 2 for object with UUID: " .. self.owner:getUUID().hi .. ", " .. self.owner:getUUID().lo)
end

function behavior:update(dt)
    return "success"
end

function behavior:destroy()
    --
end

return behavior
