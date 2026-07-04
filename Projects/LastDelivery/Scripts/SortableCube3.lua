behavior = {}

function behavior:start()
    self.owner:getBehavior():MB_setInt("color", 3)
    print("Color set to 3 for object with UUID: " .. self.owner:getUUID().hi .. ", " .. self.owner:getUUID().lo)
end

function behavior:update(dt)
    return "success"
end

function behavior:destroy()
    --
end

return behavior
