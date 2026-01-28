# Enemy Behavior Script Debug Notes

## Changes Made

Added comprehensive print statements throughout `enemy_beh.lua` to track:

### 1. **State Transitions**
- Added `S_setOnEnter()` callbacks for all states (Patrol, Hunt, Combat, Retreat, Idle)
- Prints when entering each state with clear `[STATE]` prefix
- All conditional transitions now print when triggered with relevant condition values

### 2. **Pattern Utility Evaluation**
All patterns in each state now print their utility values:
- **Patrol**: WalkRoute, IdleLookAround, SuspiciousScan
- **Hunt**: MoveToNoise, SpiralSearch, PredictiveFlank
- **Combat**: ChaseTarget, StrafeShoot, GrenadeAndCover, SuppressAndAdvance, RepositionForAngle
- **Retreat**: RunToSafeSpot, Heal, PeekAndThreaten

Prints include the calculated utility value and all factors contributing to it.

### 3. **Perception Events**
- **SIGHT CALLBACK**: Logs target sighting with position, distance, pressure/uncertainty updates
- **HEARING CALLBACK**: Logs noise detection with loudness level and position
- **DAMAGE CALLBACK**: Logs damage taken and health/pressure updates
- All perception state changes clearly marked

### 4. **Debug Output Format**
```
[STATE]        - State entry events
[TRANSITION]   - FSM transitions with conditions
[PATROL]       - Patrol state pattern utilities
[HUNT]         - Hunt state pattern utilities
[COMBAT]       - Combat state pattern utilities
[RETREAT]      - Retreat state pattern utilities
[PERCEPTION]   - Perception system events
[DEBUG]        - General debug information
```

---

## Potential Issues Found

### 1. **Perception Callback Variable Reference**
**Issue**: In perception callbacks, the variable `bc` is referenced without proper context.
- The callbacks are defined inside `InitPerception()` which has `bc` in scope, but care should be taken if these callbacks execute in a different context.
- **Location**: Lines with `bc:MB_set*` inside perception callbacks
- **Fix**: Ensure `bc` remains properly accessible in the callback scope

### 2. **Target UUID Validation**
**Issue**: Null/invalid target checks may not properly handle UUID comparison
- Lines checking `tid.isEqual(nilUUID)` could fail if `tid` is nil/false
- **Location**: Transitions checking `not tid.isEqual(nilUUID)`
- **Fix**: Add explicit nil checks before calling `.isEqual()`
```lua
local hasTarget = tid and not tid.isEqual(nilUUID) or false
```

### 3. **MBv() Vector Validity Check**
**Issue**: `MBv()` returns a Vector3 but patterns check "if not vector" which may not work correctly
- A Vector3 object may exist but be zeros (0,0,0)
- **Location**: Lines like `if (not hasNoise)` in SuspiciousScan pattern
- **Fix**: Check if vector is valid by testing components:
```lua
local function isValidVector(v)
    return v and (v.x ~= 0 or v.y ~= 0 or v.z ~= 0)
end
```

### 4. **Target Object Lookup Reliability**
**Issue**: `getGameObjectByUUID()` may return nil after target is killed
- No fallback handling if target object becomes invalid mid-combat
- **Location**: UpdatePerceptionTimers() and perception callbacks
- **Fix**: Add more robust null checking and logging

### 5. **Pressure/Uncertainty Not Decaying**
**Issue**: Pressure and uncertainty values only increase, never naturally decrease
- Over time, pressure will stay high preventing return to Patrol
- **Location**: MB_getFloat() initial values at lines 78-79
- **Fix**: Add decay mechanism in behavior:update() or add time-based resets

**Suggested Fix**:
```lua
-- In UpdatePerceptionTimers()
local pressure = bc:MB_getFloat("pressureLevel", 0.0)
local decayRate = dt * 0.1  -- 10% per second
local newPressure = math.max(0.0, pressure - decayRate)
bc:MB_setFloat("pressureLevel", newPressure)
```

### 6. **Retreat Health Threshold Logic**
**Issue**: Retreat->Combat transition requires hp >= 70, but Retreat patterns prefer hp in range 20-60
- PeekAndThreaten returns 0 if hp > 60 or < 20
- This creates a gap where Retreat with hp 60-70 has no good actions
- **Location**: Lines for Retreat patterns
- **Fix**: Adjust thresholds for consistency or add additional patterns

### 7. **Missing Event Flag Resets**
**Issue**: Event flags like `event_OnSeeEnemy`, `event_WasAttacked` are never reset
- These flags are set by external code but never cleared
- May cause repeated transitions
- **Location**: SetupTransitions() conditional functions
- **Fix**: Add flag reset mechanism or auto-clear after transition:
```lua
-- After consuming an event in a transition, reset it
bc:MB_setBool("event_OnSeeEnemy", false)
```

### 8. **Distance Calculation During Loss of Sight**
**Issue**: When target is lost, distance is not updated
- distanceToTarget stays at last known value
- **Location**: UpdatePerceptionTimers() only updates if targetObj exists
- **Fix**: Set distanceToTarget to a large value when target is lost

---

## Testing Recommendations

1. **Monitor Console Output**: Watch for frequency of state transitions - look for oscillations
2. **Check Pressure/Uncertainty Values**: Ensure they change realistically with game events
3. **Verify Target Detection**: Confirm sight/hearing callbacks are firing at expected ranges
4. **Test Retreat Behavior**: Ensure damaged NPC properly retreats and heals
5. **Event Trigger Testing**: Verify OnSeeEnemy, WasAttacked, OnNoiseHeard events are being set externally
6. **Edge Cases**: Test with target dying, being out of range, obscured by obstacles

