function move_object(go, dx, dy, dz)
    local transform = go:getTransform()
    transform.m_position.x = transform.m_position.x + dx
    transform.m_position.y = transform.m_position.y + dy
    transform.m_position.z = transform.m_position.z + dz
    return "success"
end

move_object_params = { {name="go", type="userdata"}, {name="dx", type="number"}, {name="dy", type="number"}, {name="dz", type="number"} }
