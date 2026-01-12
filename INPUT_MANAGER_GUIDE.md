# InputManager - Robust Input Handling System

## Overview
The `InputManager` provides reliable keyboard input handling with proper edge detection for press, release, and hold states. It solves common input bugs by tracking state across frames.

## Features
- **IsKeyDown()**: Returns true while key is held (includes first press frame)
- **IsKeyPressed()**: Returns true only on the frame when key goes from up→down (edge detection)
- **IsKeyReleased()**: Returns true only on the frame when key goes from down→up (edge detection)
- Handles very short taps (1-2 frames)
- Handles keys held for extended periods
- Supports multiple simultaneous key presses
- Safe enable/disable functionality
- Reset capability for mode switches

## Architecture

### State Tracking
```
Frame N-1:     [Previous Keys]
                    ↓
Frame N:       [Current Keys]
                    ↓
              Edge Detection
                    ↓
         [Pressed]    [Released]
```

### Integration Points
1. **PlayerController** - Game player input
2. **WorldEditor** - Editor camera controls
3. Separate instances for game vs world editor (no conflicts)

---

## Quick Start

### Basic Usage Pattern

```cpp
#include <Windows/InputManager.h>

// 1. Create InputManager instance
InputManager m_inputManager;

// 2. In your event handlers
void OnKeyDown(Keys key) {
    m_inputManager.ProcessKeyDown(key);
}

void OnKeyUp(Keys key) {
    m_inputManager.ProcessKeyUp(key);
}

// 3. In your game loop (once per frame, AFTER processing events)
void Update(float deltaTime) {
    // MUST call this first!
    m_inputManager.Update();
    
    // Now query input states...
}
```

---

## Detailed Examples

### Example 1: Player Movement (Held Keys)

```cpp
void UpdatePlayerMovement(float deltaTime)
{
    // Update must be called first each frame
    m_inputManager.Update();
    
    Vector3 moveDir = Vector3::Zero;
    
    // IsKeyDown() returns true every frame while key is held
    if (m_inputManager.IsKeyDown(Keys::W)) {
        moveDir.z += 1.0f;
    }
    if (m_inputManager.IsKeyDown(Keys::S)) {
        moveDir.z -= 1.0f;
    }
    if (m_inputManager.IsKeyDown(Keys::A)) {
        moveDir.x -= 1.0f;
    }
    if (m_inputManager.IsKeyDown(Keys::D)) {
        moveDir.x += 1.0f;
    }
    
    if (moveDir.LengthSquared() > 0) {
        moveDir.Normalize();
        player->position += moveDir * speed * deltaTime;
    }
}
```

### Example 2: Jump (Press Once)

```cpp
void UpdatePlayerActions(float deltaTime)
{
    m_inputManager.Update();
    
    // IsKeyPressed() returns true ONLY on the frame when key is pressed
    // Perfect for actions that should trigger once
    if (m_inputManager.IsKeyPressed(Keys::Space)) {
        if (player->isGrounded) {
            player->velocity.y = jumpForce;
            PlaySound("jump.wav");
        }
    }
}
```

### Example 3: Toggle Actions (Press to Toggle)

```cpp
bool isCrouching = false;

void UpdatePlayerState(float deltaTime)
{
    m_inputManager.Update();
    
    // Toggle crouch on press
    if (m_inputManager.IsKeyPressed(Keys::ControlKey)) {
        isCrouching = !isCrouching;
        player->SetCrouchState(isCrouching);
    }
    
    // Or hold to crouch
    if (m_inputManager.IsKeyDown(Keys::ControlKey)) {
        player->SetCrouchState(true);
    } else {
        player->SetCrouchState(false);
    }
}
```

### Example 4: Charging Attack (Press, Hold, Release)

```cpp
float chargeTime = 0.0f;
bool isCharging = false;

void UpdateAttack(float deltaTime)
{
    m_inputManager.Update();
    
    // Start charging when button is pressed
    if (m_inputManager.IsKeyPressed(Keys::LeftButton)) {
        isCharging = true;
        chargeTime = 0.0f;
    }
    
    // Continue charging while held
    if (isCharging && m_inputManager.IsKeyDown(Keys::LeftButton)) {
        chargeTime += deltaTime;
        chargeTime = std::min(chargeTime, maxChargeTime);
    }
    
    // Release attack when button is released
    if (m_inputManager.IsKeyReleased(Keys::LeftButton) && isCharging) {
        float damage = baseDamage + (chargeTime / maxChargeTime) * bonusDamage;
        PerformAttack(damage);
        isCharging = false;
        chargeTime = 0.0f;
    }
}
```

### Example 5: Combo System (Sequence Detection)

```cpp
struct ComboState {
    Keys lastKey = Keys::None;
    float timeSinceLastPress = 0.0f;
    int comboCount = 0;
};

ComboState comboState;

void UpdateCombos(float deltaTime)
{
    m_inputManager.Update();
    
    comboState.timeSinceLastPress += deltaTime;
    
    // Reset combo if too much time passed
    if (comboState.timeSinceLastPress > comboTimeout) {
        comboState.comboCount = 0;
        comboState.lastKey = Keys::None;
    }
    
    // Detect combo sequence: Left Click -> Left Click -> Right Click
    if (m_inputManager.IsKeyPressed(Keys::LeftButton)) {
        if (comboState.lastKey == Keys::LeftButton && comboState.comboCount == 1) {
            comboState.comboCount = 2;
        } else {
            comboState.comboCount = 1;
        }
        comboState.lastKey = Keys::LeftButton;
        comboState.timeSinceLastPress = 0.0f;
    }
    
    if (m_inputManager.IsKeyPressed(Keys::RightButton)) {
        if (comboState.comboCount == 2 && comboState.lastKey == Keys::LeftButton) {
            // Execute combo!
            ExecuteSpecialCombo();
            comboState.comboCount = 0;
        }
        comboState.lastKey = Keys::RightButton;
        comboState.timeSinceLastPress = 0.0f;
    }
}
```

### Example 6: Multiple Key Checks (Sprint)

```cpp
void UpdateSprint(float deltaTime)
{
    m_inputManager.Update();
    
    // Sprint when holding Shift + W
    bool isSprinting = m_inputManager.IsKeyDown(Keys::ShiftKey) && 
                       m_inputManager.IsKeyDown(Keys::W);
    
    float currentSpeed = isSprinting ? sprintSpeed : walkSpeed;
    
    // Consume stamina only while actually sprinting
    if (isSprinting) {
        stamina -= staminaDrainRate * deltaTime;
        if (stamina <= 0.0f) {
            stamina = 0.0f;
            // Can't sprint anymore
            currentSpeed = walkSpeed;
        }
    }
}
```

### Example 7: Mode Switching (Reset Input)

```cpp
enum class GameMode { Playing, Paused, Menu };
GameMode currentMode = GameMode::Playing;

void SwitchMode(GameMode newMode)
{
    currentMode = newMode;
    
    // Reset input state when switching modes to prevent stuck keys
    m_inputManager.Reset();
    
    switch (newMode) {
        case GameMode::Menu:
            m_inputManager.SetEnabled(false); // Disable game input in menu
            break;
        case GameMode::Playing:
            m_inputManager.SetEnabled(true);
            break;
        case GameMode::Paused:
            // Keep input enabled but frozen
            break;
    }
}

void Update(float deltaTime)
{
    if (m_inputManager.IsKeyPressed(Keys::Escape)) {
        if (currentMode == GameMode::Playing) {
            SwitchMode(GameMode::Paused);
        } else {
            SwitchMode(GameMode::Playing);
        }
    }
    
    if (currentMode == GameMode::Playing) {
        m_inputManager.Update();
        UpdateGameplay(deltaTime);
    }
}
```

### Example 8: Debug Input (Check Any Key)

```cpp
void UpdateDebug(float deltaTime)
{
    m_inputManager.Update();
    
    // Check if any key is currently pressed
    if (m_inputManager.IsAnyKeyDown()) {
        // Get all currently pressed keys
        const auto& pressedKeys = m_inputManager.GetKeysDown();
        
        for (const auto& key : pressedKeys) {
            // Log or display pressed keys
            LogDebugInfo("Key down: " + KeyToString(key));
        }
    }
    
    // Debug toggle that works even if held for a long time
    if (m_inputManager.IsKeyPressed(Keys::F3)) {
        showDebugInfo = !showDebugInfo;
    }
}
```

---

## Integration in Engine

### PlayerController Integration

```cpp
// In PlayerController.h
#include <Windows/InputManager.h>

class PlayerController {
    InputManager m_inputManager;
    // ...
};

// In PlayerController.cpp
void PlayerController::HandleKeyDown(Keys key) {
    m_inputManager.ProcessKeyDown(key);
}

void PlayerController::HandleKeyUp(Keys key) {
    m_inputManager.ProcessKeyUp(key);
}

void PlayerController::UpdatePlayer(float deltaTime) {
    // CRITICAL: Update first!
    m_inputManager.Update();
    
    // Now use IsKeyDown/IsKeyPressed/IsKeyReleased
    if (m_inputManager.IsKeyDown(Keys::W)) {
        // Move forward
    }
}
```

### WorldEditor Integration (Already Done)

```cpp
// In WorldEditor.h
#include <Windows/InputManager.h>

class WorldEditor {
    InputManager m_editorInputManager; // Separate instance for editor
    // ...
};

// In EditorApp.cpp
void EditorApp::UpdateEditor(float deltaTime) {
	if (!imguiEditorPass->IsFocusedGameViewport) {
		// Reset input state when not focused
		m_worldEditor->m_editorInputManager.Reset();
		m_worldEditor->IsRightMousePressed = false;
	}
	else {
		// Update input manager for edge detection
		m_worldEditor->m_editorInputManager.Update();
	}
	m_worldEditor->Update(deltaTime);
}
```

---

## Common Patterns

### Pattern 1: Continuous Action While Held
```cpp
// Use IsKeyDown() for actions that happen every frame
if (m_inputManager.IsKeyDown(Keys::W)) {
    // Executed every frame while W is held
    MoveForward(deltaTime);
}
```

### Pattern 2: One-Time Action on Press
```cpp
// Use IsKeyPressed() for actions that trigger once
if (m_inputManager.IsKeyPressed(Keys::E)) {
    // Executed only once when E is pressed
    Interact();
}
```

### Pattern 3: Action on Release
```cpp
// Use IsKeyReleased() for actions on button up
if (m_inputManager.IsKeyReleased(Keys::LeftButton)) {
    // Executed when mouse button is released
    ShootArrow();
}
```

### Pattern 4: Hold Duration
```cpp
float holdTime = 0.0f;

if (m_inputManager.IsKeyPressed(Keys::Space)) {
    holdTime = 0.0f;
}

if (m_inputManager.IsKeyDown(Keys::Space)) {
    holdTime += deltaTime;
}

if (m_inputManager.IsKeyReleased(Keys::Space)) {
    if (holdTime > longPressThreshold) {
        PerformLongPress();
    } else {
        PerformShortPress();
    }
}
```

---

## Common Mistakes to Avoid

### ❌ Mistake 1: Not Calling Update()
```cpp
void BadExample(float deltaTime) {
    // WRONG: No Update() call!
    if (m_inputManager.IsKeyPressed(Keys::Space)) {
        Jump();
    }
}
```
**Fix**: Always call `Update()` once per frame before querying input.

### ❌ Mistake 2: Calling Update() Multiple Times
```cpp
void BadExample(float deltaTime) {
    m_inputManager.Update(); // First update
    UpdateMovement(deltaTime);
    m_inputManager.Update(); // Second update - WRONG!
    UpdateActions(deltaTime);
}
```
**Fix**: Call `Update()` only once per frame, at the start of your update loop.

### ❌ Mistake 3: Using IsKeyPressed() for Continuous Actions
```cpp
void BadExample(float deltaTime) {
    m_inputManager.Update();
    
    // WRONG: Movement will only happen for 1 frame
    if (m_inputManager.IsKeyPressed(Keys::W)) {
        MoveForward(deltaTime);
    }
}
```
**Fix**: Use `IsKeyDown()` for continuous actions.

### ❌ Mistake 4: Processing Events After Update()
```cpp
void BadExample(float deltaTime) {
    m_inputManager.Update(); // Update first
    
    // WRONG: Processing events after update
    for (auto& event : events) {
        m_inputManager.ProcessKeyDown(event.key);
    }
}
```
**Fix**: Process all events first, then call `Update()` once.

---

## Debugging Tips

### Check Input State
```cpp
void DebugInput() {
    if (m_inputManager.IsKeyDown(Keys::F1)) {
        std::cout << "F1 is held" << std::endl;
    }
    
    if (m_inputManager.IsKeyPressed(Keys::F1)) {
        std::cout << "F1 was just pressed" << std::endl;
    }
    
    if (m_inputManager.IsKeyReleased(Keys::F1)) {
        std::cout << "F1 was just released" << std::endl;
    }
}
```

### Log All Input Events
```cpp
void LogAllInput() {
    const auto& keys = m_inputManager.GetKeysDown();
    for (const auto& key : keys) {
        std::cout << "Key " << (int)key << " is down" << std::endl;
    }
}
```

---

## API Reference

### Core Methods

```cpp
// Event Processing (call from OS event handlers)
void ProcessKeyDown(Keys key);
void ProcessKeyUp(Keys key);

// Frame Update (call once per frame)
void Update();

// Query Methods (use in game logic)
bool IsKeyDown(Keys key) const;       // True while held
bool IsKeyPressed(Keys key) const;    // True on press frame only
bool IsKeyReleased(Keys key) const;   // True on release frame only

// Utility Methods
void Reset();                          // Clear all state
void SetEnabled(bool enabled);         // Enable/disable input
bool IsEnabled() const;                // Check if enabled
bool IsAnyKeyDown() const;             // Check if any key is pressed
const eastl::unordered_set<Keys>& GetKeysDown() const; // Get all pressed keys
```

---

## Performance Notes

- **Memory**: ~200 bytes per InputManager instance
- **CPU**: O(n) where n = number of keys pressed (typically < 10)
- **Edge Detection**: Computed once per frame in `Update()`
- **Thread Safety**: Not thread-safe, use from main thread only

---


## File Locations

- **Header**: `SunshineEngine/include/Windows/InputManager.h`
- **Implementation**: `SunshineEngine/src/Windows/InputManager.cpp`
- **PlayerController**: `SunshineEngine/include/PlayerObject/PlayerController.h`
- **EditorApp**: `SunshineEditor/include/EditorApp.h`

---

## Best Practices

1. ✅ Call `Update()` once per frame
2. ✅ Process all events before `Update()`
3. ✅ Use `IsKeyDown()` for continuous actions
4. ✅ Use `IsKeyPressed()` for one-shot actions
5. ✅ Use `IsKeyReleased()` for release-triggered actions
6. ✅ Call `Reset()` when switching modes
7. ✅ Use separate instances for different input contexts
8. ✅ Check `IsEnabled()` before critical actions

---

## Troubleshooting

**Problem**: Keys feel "sticky" or don't respond  
**Solution**: Make sure `Update()` is called every frame

**Problem**: Press events fire multiple times  
**Solution**: Use `IsKeyPressed()` instead of `IsKeyDown()` for one-shot actions

**Problem**: Keys stay pressed after losing focus  
**Solution**: Call `Reset()` when window loses focus

**Problem**: Some keys don't register  
**Solution**: Ensure `ProcessKeyDown/Up` are called from event handlers

**Problem**: Input works in editor but not in game  
**Solution**: Check that game mode is routing input correctly

---
