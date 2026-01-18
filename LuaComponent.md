# Lua Scripting Documentation

Complete guide to using Lua scripts to manage game object components through the engine.

---

## Table of Contents

1. [Introduction](#introduction)
2. [TransformComponent](#transformcomponent)
3. [PhysicsComponent](#physicscomponent)
4. [CameraComponent](#cameracomponent)
5. [PerceptionComponent](#perceptioncomponent)
6. [PerceptionSystem](#perceptionsystem)
7. [Usage Examples](#usage-examples)

---

## Introduction

The Lua scripting system allows developers to manage game object components in real-time. Each component is accessible through a corresponding Lua getter function that returns a reference to the game object's component.

### Basic Syntax

```lua
-- Get a component
local transform = getTransform(gameObject)

-- Access component fields
local position = transform.m_position

-- Call component methods
perception:setSightRadius(100.0)
```

---

## TransformComponent

The transform component manages the position, rotation, and scale of a game object in world and local space.

### Getting the Component

```lua
local transform = getTransform(gameObject)
```

### Fields

All fields are represented as Vector3 structures and can be read and modified directly.

| Field | Type | Description |
|-------|------|-------------|
| `m_position` | Vector3 | Object position in world space |
| `m_rotation` | Vector3 | Object rotation in world space (in degrees or radians) |
| `m_scaleFactor` | Vector3 | Object scale in world space |
<!--
| `m_localPosition` | Vector3 | Object position relative to parent object |
| `m_localRotation` | Vector3 | Object rotation relative to parent object |
| `m_localScaleFactor` | Vector3 | Object scale relative to parent object |
-->

### Usage Examples

```lua
-- Get current position
local pos = transform.m_position

-- Change position
transform.m_position = {x = 10.0, y = 5.0, z = 20.0}

-- Modify only X coordinate
local newPos = transform.m_position
newPos.x = 15.0
transform.m_position = newPos

-- Work with local position (relative to parent)
transform.m_localPosition = {x = 1.0, y = 0.0, z = 0.0}

-- Scale the object
transform.m_scaleFactor = {x = 2.0, y = 2.0, z = 2.0}

-- Different scales per axis
transform.m_scaleFactor = {x = 1.0, y = 2.0, z = 1.5}
```

---

## PhysicsComponent

The physics component provides access to the Jolt Physics engine. It allows you to apply forces, impulses, torques, and query velocities and positions of physics bodies.

### Getting the Component

```lua
local physics = GetPhysics(gameObject)
```

**Note:** PhysicsComponent must be previously added to the game object in the editor or through code.

### Methods

#### Force and Impulse

##### `addForce(force)`

Applies a continuous force to the physics body.

```lua
physics:addForce({x = 100.0, y = 0.0, z = 0.0})
```

**Parameters:**
- `force` (Vector3): Force vector to apply

##### `addImpulse(impulse)`

Applies an instantaneous impulse to the physics body.

```lua
physics:addImpulse({x = 500.0, y = 1000.0, z = 0.0})
```

**Parameters:**
- `impulse` (Vector3): Impulse vector to apply

##### `addTorque(torque)`

Applies a continuous rotational torque to the physics body.

```lua
physics:addTorque({x = 0.0, y = 50.0, z = 0.0})
```

**Parameters:**
- `torque` (Vector3): Torque vector to apply

##### `addAngularImpulse(angularImpulse)`

Applies an instantaneous angular impulse to the physics body.

```lua
physics:addAngularImpulse({x = 10.0, y = 20.0, z = 0.0})
```

**Parameters:**
- `angularImpulse` (Vector3): Angular impulse vector to apply

---

#### Query Methods

##### `getAccumulatedForce()`

Gets the accumulated force on the physics body.

```lua
local force = physics:getAccumulatedForce()
print("Accumulated force:", force.x, force.y, force.z)
```

**Returns:** Vector3

##### `getAccumulatedTorque()`

Gets the accumulated torque on the physics body.

```lua
local torque = physics:getAccumulatedTorque()
```

**Returns:** Vector3

##### `getLinearVelocity()`

Gets the current linear velocity of the physics body.

```lua
local velocity = physics:getLinearVelocity()
local speed = math.sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z)
print("Speed:", speed)
```

**Returns:** Vector3

##### `getAngularVelocity()`

Gets the current angular velocity of the physics body.

```lua
local angularVel = physics:getAngularVelocity()
```

**Returns:** Vector3

##### `getPointVelocity(point)`

Gets the velocity at a specific point on the physics body.

```lua
local pointVel = physics:getPointVelocity({x = 0.0, y = 1.0, z = 0.0})
```

**Parameters:**
- `point` (Vector3): World space point to query

**Returns:** Vector3

##### `getPosition()`

Gets the current position of the physics body.

```lua
local pos = physics:getPosition()
print("Position:", pos.x, pos.y, pos.z)
```

**Returns:** Vector3

##### `getRotation()`

Gets the current rotation of the physics body as a quaternion.

```lua
local rot = physics:getRotation()
print("Rotation quaternion:", rot)
```

**Returns:** Quaternion

---

#### Reset Methods

##### `resetForce()`

Clears all accumulated forces on the physics body.

```lua
physics:resetForce()
```

##### `resetTorque()`

Clears all accumulated torques on the physics body.

```lua
physics:resetTorque()
```

---

#### Velocity Control

##### `setLinearVelocity(velocity)`

Directly sets the linear velocity of the physics body.

```lua
physics:setLinearVelocity({x = 10.0, y = 0.0, z = 5.0})
```

**Parameters:**
- `velocity` (Vector3): Target velocity

##### `setAngularVelocity(angularVelocity)`

Directly sets the angular velocity of the physics body.

```lua
physics:setAngularVelocity({x = 0.0, y = 90.0, z = 0.0})
```

**Parameters:**
- `angularVelocity` (Vector3): Target angular velocity

---

## CameraComponent

> **Warning**
Only PlayerObject has CameraComponent!

The camera component provides control over the camera's view, position, rotation, and various camera modes. It manages the player's perspective with support for first-person, orbital, and follow camera modes.

### Getting the Component

```lua
local cameraComponent = self.owner:getCameraComponent()
local camera = cameraComponent:getCamera()
```

---

## Camera

The Camera object handles view frustum, direction vectors, and stick-based camera control for orbiting and third-person perspectives.

### Getting the Camera

```lua
local camera = cameraComponent:getCamera()
```

### Read-Only Properties

| Property | Type | Description |
|----------|------|-------------|
| `forward` | Vector3 | Camera forward direction vector |
| `up` | Vector3 | Camera up direction vector |
| `right` | Vector3 | Camera right direction vector |
| `position` | Vector3 | Camera world position |
| `deltaTime` | float | Delta time for current frame |

### Methods

#### Position Methods

##### `setPosition(position)`

Sets the camera position in world space.

```lua
camera:setPosition({x = 0.0, y = 5.0, z = -10.0})
```

**Parameters:**
- `position` (Vector3): Target position

##### `getPosition()`

Gets the current camera position.

```lua
local pos = camera:getPosition()
print("Position:", pos.x, pos.y, pos.z)
```

**Returns:** Vector3

#### Movement Methods

##### `moveForward(moveSpeed)`

Moves the camera forward relative to its forward direction.

```lua
camera:moveForward(5.0)
```

**Parameters:**
- `moveSpeed` (float): Speed of movement

##### `moveBackward(moveSpeed)`

Moves the camera backward relative to its forward direction.

```lua
camera:moveBackward(5.0)
```

**Parameters:**
- `moveSpeed` (float): Speed of movement

##### `moveLeft(moveSpeed)`

Moves the camera left relative to its right direction.

```lua
camera:moveLeft(5.0)
```

**Parameters:**
- `moveSpeed` (float): Speed of movement

##### `moveRight(moveSpeed)`

Moves the camera right relative to its right direction.

```lua
camera:moveRight(5.0)
```

**Parameters:**
- `moveSpeed` (float): Speed of movement

##### `moveUp(moveSpeed)`

Moves the camera up in world space.

```lua
camera:moveUp(5.0)
```

**Parameters:**
- `moveSpeed` (float): Speed of movement

##### `moveDown(moveSpeed)`

Moves the camera down in world space.

```lua
camera:moveDown(5.0)
```

**Parameters:**
- `moveSpeed` (float): Speed of movement

#### Target & Up Vector Methods (for Orbital Camera)

##### `setTarget(target)`

Sets the target point the camera looks at (for orbital mode).

```lua
camera:setTarget({x = 0.0, y = 1.0, z = 0.0})
```

**Parameters:**
- `target` (Vector3): Target position to look at

##### `getTarget()`

Gets the current camera target.

```lua
local target = camera:getTarget()
```

**Returns:** Vector3

##### `setUp(up)`

Sets the camera's up direction vector.

```lua
camera:setUp({x = 0.0, y = 1.0, z = 0.0})
```

**Parameters:**
- `up` (Vector3): Up direction vector

##### `getUp()`

Gets the current camera up direction.

```lua
local up = camera:getUp()
```

**Returns:** Vector3

#### Rotation Methods

##### `rotateYaw(angleSpeed)`

Rotates the camera around the up axis (horizontal rotation).

```lua
camera:rotateYaw(45.0)  -- 45 degrees
```

**Parameters:**
- `angleSpeed` (float): Rotation angle speed in degrees

##### `rotatePitch(angleSpeed)`

Rotates the camera around the right axis (vertical rotation).

```lua
camera:rotatePitch(30.0)  -- 30 degrees
```

**Parameters:**
- `angleSpeed` (float): Rotation angle speed in degrees

##### `switchToFPSMode()`

Switches the camera to first-person mode.

```lua
camera:switchToFPSMode()
```

#### Stick Length Methods (for Orbiting Camera)

The stick represents the distance and direction from the camera's target (used in orbital and third-person camera modes).

##### `getStickLength()`

Gets the distance from the camera to its target.

```lua
local distance = camera:getStickLength()
print("Distance to target:", distance)
```

**Returns:** float

##### `setStickLength(length)`

Sets the distance from the camera to its target (zoom).

```lua
camera:setStickLength(15.0)  -- 15 units from target
```

**Parameters:**
- `length` (float): Distance in world units (clamped to 0-100)

##### `getStickDirection()`

Gets the direction vector from target to camera (same as camera direction).

```lua
local direction = camera:getStickDirection()
```

**Returns:** Vector3

#### View Dimensions Methods

##### `getViewWidth()`

Gets the width of the view.

```lua
local width = camera:getViewWidth()
```

**Returns:** float

##### `GetViewHeight()`

Gets the height of the view.

```lua
local height = camera:getViewHeight()
```

**Returns:** float

#### Near/Far Z Plane Methods

##### `setNearZ(distance)`

Sets the near clipping plane distance.

```lua
camera:setNearZ(0.1)
```

**Parameters:**
- `distance` (float): Distance from camera to near plane

##### `getNearZ()`

Gets the near clipping plane distance.

```lua
local nearZ = camera:getNearZ()
```

**Returns:** float

##### `setFarZ(distance)`

Sets the far clipping plane distance.

```lua
camera:setFarZ(1000.0)
```

**Parameters:**
- `distance` (float): Distance from camera to far plane

##### `getFarZ()`

Gets the far clipping plane distance.

```lua
local farZ = camera:getFarZ()
```

**Returns:** float

#### Reference Length Methods

##### `setReferenceLen(length)`

Sets the reference length for camera calculations.

```lua
camera:setReferenceLen(10.0)
```

**Parameters:**
- `length` (float): Reference length value

##### `getReferenceLen()`

Gets the current reference length.

```lua
local refLen = camera:getReferenceLen()
```

**Returns:** float

### Usage Examples

#### Orbital Camera Control

```lua
-- Set up orbital camera
local camera = cameraComponent:getCamera()
camera:setTarget({x = 0.0, y = 1.0, z = 0.0})  -- Look at player center
camera:setStickLength(10.0)  -- 10 units away

-- Orbit around target with mouse movement
camera:rotateYaw(45.0)   -- Rotate horizontally
camera:rotatePitch(-20.0)  -- Rotate vertically
```

#### Dynamic Camera Distance

```lua
function behavior:update(dt)
    local camera = self.owner:getCameraComponent():getCamera()
    local currentTime = os.clock()
    
    -- Smoothly change zoom distance based on time
    camera:setStickLength(10.0 + 3.0 * math.sin(currentTime))
    
    return "success"
end
```

---

## PerceptionComponent

The perception component manages the sight and hearing of a game object. It allows you to configure object detection parameters and receive callbacks when perception events occur.

### Getting the Component

```lua
-- Get the perception component of the current object
local perception = getPerceptionComponent(gameObject)
```

**Note:** PerceptionComponent must be previously added to the game object in the editor or through code.

### Methods

#### Getting Owner ID

```lua
local ownerId = perception:getOwnerId()
```

Returns the ID of the game object that owns this component.

---

### Sight

#### `setSight(radius, loseRadius, fieldOfView, throughObjects)`

Sets all sight parameters in a single call.

```lua
perception:setSight(
    100.0,      -- radius: sight range in units
    50.0,       -- loseRadius: target loss radius
    120.0,      -- fieldOfView: field of view angle in degrees
    false       -- throughObjects: can see through objects
)
```

#### `setSightRadius(radius)`

Sets the sight radius.

```lua
perception:setSightRadius(150.0)
```

**Parameters:**
- `radius` (float): Sight radius in world units

#### `setLoseRadius(radius)`

Sets the radius at which an object is considered lost from view.

```lua
perception:setLoseRadius(75.0)
```

**Parameters:**
- `radius` (float): Target loss radius in world units

#### `setFieldOfView(fov)`

Sets the field of view angle.

```lua
perception:setFieldOfView(90.0)  -- 90 degrees
```

**Parameters:**
- `fov` (float): Field of view angle in degrees (0-360)

#### `setCanSeeThroughObjects(canSee)`

Sets whether the object can see through other objects.

```lua
perception:setCanSeeThroughObjects(true)
```

**Parameters:**
- `canSee` (bool): `true` if can see through objects, `false` otherwise

#### `getSightRadius()`

Gets the current sight radius.

```lua
local radius = perception:getSightRadius()
```

**Returns:** float

#### `getLoseRadius()`

Gets the current target loss radius.

```lua
local loseRadius = perception:getLoseRadius()
```

**Returns:** float

#### `getFieldOfView()`

Gets the current field of view angle.

```lua
local fov = perception:getFieldOfView()
```

**Returns:** float

#### `getCanSeeThroughObjects()`

Checks if the object can see through objects.

```lua
local canSee = perception:getCanSeeThroughObjects()
```

**Returns:** bool

---

### Hearing

#### `setHearing(radius, threshold, sensitivity)`

Sets all hearing parameters in a single call.

```lua
perception:setHearing(
    200.0,      -- radius: hearing range
    0.5,        -- threshold: noise threshold value
    1.0         -- sensitivity: sensitivity multiplier
)
```

#### `setHearingRadius(radius)`

Sets the hearing radius.

```lua
perception:setHearingRadius(250.0)
```

**Parameters:**
- `radius` (float): Hearing radius in world units

#### `setThreshold(threshold)`

Sets the noise threshold for detection.

```lua
perception:setThreshold(0.3)
```

**Parameters:**
- `threshold` (float): Minimum noise level for detection (0-1)

#### `setSensitivity(sensitivity)`

Sets the hearing sensitivity.

```lua
perception:setSensitivity(1.5)
```

**Parameters:**
- `sensitivity` (float): Sensitivity multiplier (1.0 = standard)

#### `getHearingRadius()`

Gets the current hearing radius.

```lua
local radius = perception:getHearingRadius()
```

**Returns:** float

#### `getThreshold()`

Gets the current threshold value.

```lua
local threshold = perception:getThreshold()
```

**Returns:** float

#### `getSensitivity()`

Gets the current sensitivity.

```lua
local sensitivity = perception:getSensitivity()
```

**Returns:** float

---

### Sight Callbacks

Callbacks are invoked when an object sees or loses sight of another object.

#### `addSightCallback(function)`

Adds a callback for visual detection.

```lua
local callbackId = perception:addSightCallback(function(targetId, isDetected)
    if isDetected then
        print("Object " .. targetId .. " detected!")
    else
        print("Object " .. targetId .. " lost from sight!")
    end
end)
```

**Parameters:**
- `function`: Lua function with signature `function(targetId, isDetected)`
  - `targetId` (uint64): ID of the detected object
  - `isDetected` (bool): `true` if detected, `false` if lost

**Returns:** uint64 (callback ID for later removal)

#### `removeSightCallback(callbackId)`

Removes a specific sight callback.

```lua
perception:removeSightCallback(callbackId)
```

**Parameters:**
- `callbackId` (uint64): Callback ID obtained when adding

#### `clearSightCallbacks()`

Removes all sight callbacks.

```lua
perception:clearSightCallbacks()
```

---

### Hearing Callbacks

Callbacks are invoked when an object hears a sound.

#### `addHearingCallback(function)`

Adds a callback for sound detection.

```lua
local callbackId = perception:addHearingCallback(function(sourceId, loudness)
    print("Heard sound from object " .. sourceId .. " with loudness " .. loudness)
end)
```

**Parameters:**
- `function`: Lua function with signature `function(sourceId, loudness)`
  - `sourceId` (uint64): ID of the sound source
  - `loudness` (float): Sound loudness level

**Returns:** uint64 (callback ID)

#### `removeHearingCallback(callbackId)`

Removes a specific hearing callback.

```lua
perception:removeHearingCallback(callbackId)
```

**Parameters:**
- `callbackId` (uint64): Callback ID

#### `clearHearingCallbacks()`

Removes all hearing callbacks.

```lua
perception:clearHearingCallbacks()
```

---

### Damage Callbacks

Callbacks are invoked when an object takes damage.

#### `addDamageCallback(function)`

Adds a callback for damage reception.

```lua
local callbackId = perception:addDamageCallback(function(instigatorId, damage)
    print("Took damage " .. damage .. " from object " .. instigatorId)
end)
```

**Parameters:**
- `function`: Lua function with signature `function(instigatorId, damage)`
  - `instigatorId` (uint64): ID of the object dealing damage
  - `damage` (float): Damage amount

**Returns:** uint64 (callback ID)

#### `removeDamageCallback(callbackId)`

Removes a specific damage callback.

```lua
perception:removeDamageCallback(callbackId)
```

**Parameters:**
- `callbackId` (uint64): Callback ID

#### `clearDamageCallbacks()`

Removes all damage callbacks.

```lua
perception:clearDamageCallbacks()
```

---

### Events

#### `makeNoise(loudness)`

Produces a sound that other objects can hear.

```lua
perception:makeNoise(0.8)
```

**Parameters:**
- `loudness` (float): Sound loudness (0-1)

**Returns:** Sound event ID

#### `dealDamage(instigator, damage)`

Deals damage to this object from a specified source.

```lua
perception:dealDamage(instigatorPerception, 25.0)
```

**Parameters:**
- `instigator` (PerceptionComponent): Perception component of the damage-dealing object
- `damage` (float): Damage amount

---

## PerceptionSystem

The global perception system manages team logic and interactions between objects of different teams.

### Accessing the System

```lua
local perceptionSystem = PerceptionSystem()
```

### Methods

#### Team Management

##### `registerTeam(teamId)`

Registers a new team in the system.

```lua
perceptionSystem:registerTeam(1)
perceptionSystem:registerTeam(2)
```

**Parameters:**
- `teamId` (uint32): Unique team ID

##### `unregisterTeam(teamId)`

Removes a team from the system.

```lua
perceptionSystem:unregisterTeam(1)
```

**Parameters:**
- `teamId` (uint32): Team ID to remove

---

#### Managing Sight Target Teams

##### `addSightTargetTeamIDsInTeam(teamId, targetTeamId)`

Adds a team as a sight target for visual detection.

```lua
-- Team 1 can see members of team 2
perceptionSystem:addSightTargetTeamIDsInTeam(1, 2)
```

**Parameters:**
- `teamId` (uint32): ID of the team that sees
- `targetTeamId` (uint32): ID of the team to see

##### `removeSightTargetTeamIDsInTeam(teamId, targetTeamId)`

Removes a team from sight targets.

```lua
perceptionSystem:removeSightTargetTeamIDsInTeam(1, 2)
```

**Parameters:**
- `teamId` (uint32): Team ID
- `targetTeamId` (uint32): Target team ID

##### `clearSightTargetTeamIDsInTeam(teamId)`

Clears all sight target teams.

```lua
perceptionSystem:clearSightTargetTeamIDsInTeam(1)
```

**Parameters:**
- `teamId` (uint32): Team ID

---

#### Managing Hearing Source Teams

##### `addHearingSourceTeamIDsInTeam(teamId, sourceTeamId)`

Adds a team as a hearing source for sound detection.

```lua
-- Team 1 can hear members of team 2
perceptionSystem:addHearingSourceTeamIDsInTeam(1, 2)
```

**Parameters:**
- `teamId` (uint32): ID of the team that hears
- `sourceTeamId` (uint32): ID of the team whose sounds should be heard

##### `removeHearingSourceTeamIDsInTeam(teamId, sourceTeamId)`

Removes a team from hearing sources.

```lua
perceptionSystem:removeHearingSourceTeamIDsInTeam(1, 2)
```

**Parameters:**
- `teamId` (uint32): Team ID
- `sourceTeamId` (uint32): Source team ID

##### `clearHearingSourceTeamIDsInTeam(teamId)`

Clears all hearing source teams.

```lua
perceptionSystem:clearHearingSourceTeamIDsInTeam(1)
```

**Parameters:**
- `teamId` (uint32): Team ID

---

#### Managing Team Members

##### `addToTeam(teamId, objectId)`

Adds a game object to a team.

```lua
perceptionSystem:addToTeam(1, objectId)
```

**Parameters:**
- `teamId` (uint32): Team ID
- `objectId` (uint64): Game object ID

##### `removeFromTeam(teamId, objectId)`

Removes an object from a team.

```lua
perceptionSystem:removeFromTeam(1, objectId)
```

**Parameters:**
- `teamId` (uint32): Team ID
- `objectId` (uint64): Game object ID

##### `clearTeam(teamId)`

Removes all members from a team.

```lua
perceptionSystem:clearTeam(1)
```

**Parameters:**
- `teamId` (uint32): Team ID

---

## Usage Examples

### Example 1: Basic Enemy Perception Setup

```lua
-- Get components
local transform = getTransform(enemyObject)
local perception = getPerceptionComponent(enemyObject)

-- Set enemy position
transform.m_position = {x = 0.0, y = 0.0, z = 0.0}

-- Configure sight: sees 100 units with 120 degree field of view
perception:setSight(100.0, 50.0, 120.0, false)

-- Configure hearing: hears 200 units away
perception:setHearing(200.0, 0.5, 1.0)

-- Add callback for player detection
perception:addSightCallback(function(targetId, isDetected)
    if isDetected then
        print("Enemy sees player!")
        -- Start attacking
    else
        print("Enemy lost player from sight")
        -- Return to patrol mode
    end
end)
```

### Example 2: Team Setup

```lua
local perceptionSystem = PerceptionSystem()

-- Create two teams: players and enemies
perceptionSystem:registerTeam(1)  -- Team 1: Players
perceptionSystem:registerTeam(2)  -- Team 2: Enemies

-- Enemies see players
perceptionSystem:addSightTargetTeamIDsInTeam(2, 1)

-- Enemies hear players and each other
perceptionSystem:addHearingSourceTeamIDsInTeam(2, 1)
perceptionSystem:addHearingSourceTeamIDsInTeam(2, 2)

-- Add enemy to enemy team
perceptionSystem:addToTeam(2, enemyObjectId)
```

### Example 3: Reactive Sound Behavior

```lua
local perception = getPerceptionComponent(enemyObject)

-- Add callback for sound
perception:addHearingCallback(function(sourceId, loudness)
    print("Enemy heard something with loudness " .. loudness)
    
    if loudness > 0.7 then
        print("Loud sound! Start active search!")
        -- Begin active search
    else
        print("Quiet sound, investigating carefully...")
        -- Move quietly toward sound source
    end
end)

-- Make a sound (e.g., footsteps)
perception:makeNoise(0.6)
```

### Example 4: Dynamic Parameter Changes

```lua
local perception = getPerceptionComponent(scoutObject)

-- Normal patrol mode
perception:setSightRadius(80.0)
perception:setFieldOfView(90.0)

-- Later - combat mode
if isUnderAttack then
    perception:setSightRadius(150.0)
    perception:setFieldOfView(120.0)
    perception:setSensitivity(1.5)
end

-- Or blinded/darkened mode
if isBlinded then
    perception:setSightRadius(20.0)
    perception:setFieldOfView(30.0)
end
```

### Example 5: Synchronizing Multiple Enemies

```lua
local perceptionSystem = PerceptionSystem()
local perception1 = getPerceptionComponent(enemy1)
local perception2 = getPerceptionComponent(enemy2)

-- Add callback to first enemy
perception1:addSightCallback(function(targetId, isDetected)
    if isDetected then
        -- Tell second enemy target is found
        perception2:addSightCallback(function()
            -- Help with the attack
        end)
    end
end)
```

---

## Tips and Recommendations

### Optimization

- **Reuse component references** instead of repeatedly calling `getTransform()` and `getPerceptionComponent()`
- **Remove unused callbacks** to save memory and improve performance
- **Group operations** - set multiple parameters at once

### Debugging

- Use `print()` in callbacks to track perception events
- Check radius and field of view values when experiencing unexpected behavior
- Ensure PerceptionComponent was previously added to the object

### Teams and Interaction

- Register teams properly at level start
- Make sure target relationships between teams are set before adding objects
- Use different team IDs for different factions or enemy types

---

## Data Structures

### Vector3

Used to represent position, rotation, and scale.

```lua
local vec = {
    x = 10.0,
    y = 20.0,
    z = 30.0
}
```

---

*Documentation is current for engine version with Lua bindings via sol2*
