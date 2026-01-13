# PlayerObject_Info Lua Configuration Guide

## Overview

The `PlayerObject_Info` class now supports editor-based configuration of Lua script input bindings. This allows level designers and gameplay programmers to:
- Select a Lua script containing player action callbacks
- Visually bind keyboard keys to Lua function names
- Edit and delete bindings without recompiling code
- Save/load configurations with scenes

## Features

### 1. **Lua Script Management**
- Set a script path pointing to your Lua file
- Verify if a script is loaded
- Get the current script path

### 2. **Key-Function Binding**
- Add new key → function mappings
- Edit existing bindings (change key or function)
- Delete bindings
- Query all bindings
- Find bindings by key
- Clear all bindings

### 3. **Data Persistence**
- Serialization to JSON (for save/load)
- Automatic scene persistence
- Loads bindings when scenes are loaded

## API Reference

### Lua Script Methods

```cpp
// Set the Lua script
void SetLuaScriptPath(const AssetPath& scriptPath);

// Get the current script path
const AssetPath& GetLuaScriptPath() const;

// Check if a script is set
bool HasLuaScript() const;
```

### Key-Function Pair Management

```cpp
// Add a new binding
void AddKeyFunctionPair(Keys key, const eastl::string& functionName);
void AddKeyFunctionPair(const KeyFunctionPair& pair);

// Remove a binding by index
bool RemoveKeyFunctionPair(size_t index);

// Edit a binding (both key and function)
bool EditKeyFunctionPair(size_t index, Keys newKey, 
                        const eastl::string& newFunctionName);

// Edit only the key
bool EditKeyFunctionPairKey(size_t index, Keys newKey);

// Edit only the function name
bool EditKeyFunctionPairFunction(size_t index, 
                                 const eastl::string& newFunctionName);

// Query a specific binding
const KeyFunctionPair* GetKeyFunctionPair(size_t index) const;

// Get all bindings
const eastl::vector<KeyFunctionPair>& GetAllKeyFunctionPairs() const;

// Get binding count
size_t GetKeyFunctionPairCount() const;

// Clear all bindings
void ClearKeyFunctionPairs();

// Find binding index by key (-1 if not found)
int FindPairIndexByKey(Keys key) const;
```

---

## Data Structure

### KeyFunctionPair Struct

```cpp
struct KeyFunctionPair {
    Keys key;                       // Keyboard key (from Keys enum)
    eastl::string functionName;     // Name of Lua function to call
    
    // Check if valid
    bool IsValid() const;
    
    // Convert key to display string
    static eastl::string KeyToString(Keys k);
    
    // JSON serialization
    json ToJson() const;
    static KeyFunctionPair FromJson(const json& j);
};
```

### Example Bindings

```
Key    | Function Name
-------|------------------
W      | onMoveForward
S      | onMoveBackward
A      | onStrafeLeft
D      | onStrafeRight
Space  | onJump
E      | onInteract
Q      | onSpecialAbility
Shift  | onSprint
```

---

## Editor UI Integration

### Basic Pattern

```cpp
PlayerObject_Info playerInfo;

// 1. Set Lua script
playerInfo.SetLuaScriptPath(scriptPath);

// 2. Add bindings
playerInfo.AddKeyFunctionPair(Keys::W, "onMoveForward");
playerInfo.AddKeyFunctionPair(Keys::Space, "onJump");

// 3. Query bindings
for (const auto& pair : playerInfo.GetAllKeyFunctionPairs()) {
    DisplayInUI(pair);
}
```

### ImGui Implementation Example

```cpp
// Display all bindings in a table
if (ImGui::BeginTable("KeyBindings", 3)) {
    ImGui::TableSetupColumn("Key");
    ImGui::TableSetupColumn("Function");
    ImGui::TableSetupColumn("Actions");
    ImGui::TableHeadersRow();

    const auto& pairs = playerInfo.GetAllKeyFunctionPairs();
    for (size_t i = 0; i < pairs.size(); ++i) {
        const auto& pair = pairs[i];
        
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", KeyFunctionPair::KeyToString(pair.key).c_str());
        
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", pair.functionName.c_str());
        
        ImGui::TableSetColumnIndex(2);
        if (ImGui::SmallButton("Edit")) {
            // Open edit dialog for pair i
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Delete")) {
            playerInfo.RemoveKeyFunctionPair(i);
        }
    }
    ImGui::EndTable();
}

// Add new binding button
if (ImGui::Button("+ Add Binding")) {
    // Open add dialog
}
```

---

## Workflow: Adding a Binding in Editor

### Step 1: Select Lua Script
- User clicks "Select Script" button
- File browser opens filtered to `.lua` files
- User selects `player_controller.lua`
- Path is set: `playerInfo.SetLuaScriptPath(selectedPath)`

### Step 2: Add New Binding Dialog
- User clicks "+ Add Binding"
- Dialog displays two dropdowns:
  - **Key**: Dropdown of Keys enum (W, A, S, D, Space, E, Q, Shift, Ctrl, etc.)
  - **Function**: Dropdown of functions from selected Lua script

### Step 3: Validation
- Check that key isn't already bound: `FindPairIndexByKey(selectedKey)`
- Check that function name isn't empty
- If valid: `playerInfo.AddKeyFunctionPair(selectedKey, selectedFunction)`

### Step 4: Display
- Show updated list of bindings in table
- Allow in-line edit and delete for each binding

---

## Workflow: Editing a Binding

### Step 1: Select Pair
- User clicks "Edit" button next to a binding
- Get the pair: `const auto* pair = playerInfo.GetKeyFunctionPair(index)`

### Step 2: Edit Dialog
- Display current key and function
- User can change either or both
- Dropdowns for selection (same as add)

### Step 3: Update
- Validate new key (check for duplicates, excluding current pair)
- Call: `playerInfo.EditKeyFunctionPair(index, newKey, newFunction)`

### Step 4: Confirmation
- Show success/error message
- Refresh display

---

## Workflow: Deleting a Binding

### Step 1: Confirm Deletion
- User clicks "Delete" next to a binding
- Show confirmation dialog: "Delete this binding?"

### Step 2: Remove
- Call: `playerInfo.RemoveKeyFunctionPair(index)`

### Step 3: Refresh
- Update UI to remove deleted binding

---

## JSON Serialization Format

### Saved JSON

```json
{
  "playerObject": {
    "m_UUID": 12345678,
    "luaScript": "Projects/Templates/player_controller.lua",
    "keyFunctionMappings": [
      {
        "key": 87,
        "functionName": "onMoveForward"
      },
      {
        "key": 32,
        "functionName": "onJump"
      },
      {
        "key": 69,
        "functionName": "onInteract"
      }
    ]
  }
}
```

### Loading from JSON

```cpp
PlayerObject_Info playerInfo;
json sceneJson = /* load from file */;

// Automatically loads script and bindings
playerInfo.SettingsFromJson(sceneJson["playerObject"], renderer);

// All bindings are now available
assert(playerInfo.GetKeyFunctionPairCount() == 3);
```

---

## Integration with Runtime

### Syncing to Game

When entering game mode from editor:

```cpp
// In EditorApp::RunGame()
Game* game = new Game();

// Get configured bindings from editor
const auto& editorPlayerInfo = m_worldEditor->m_scene->m_playerObject;
const auto& bindings = editorPlayerInfo->GetAllKeyFunctionPairs();

// Apply to game player
PlayerObject* gamePlayer = game->m_playerObject;
gamePlayer->m_luaActionMapping.SetLuaScriptPath(
    editorPlayerInfo->GetLuaScriptPath());

for (const auto& binding : bindings) {
    gamePlayer->m_luaActionMapping.BindKey(binding.key, binding.functionName);
}
```

---

## Best Practices

1. **Validate Script Path**
   - Check if Lua file exists before setting
   - Validate script has required functions

2. **Prevent Duplicate Keys**
   - Always check `FindPairIndexByKey()` before adding
   - Show error if key already bound

3. **Handle Function Errors**
   - Validate function names exist in Lua script
   - Provide autocomplete from script functions

4. **Save Frequently**
   - Auto-save scene after each change
   - Show dirty indicator in UI

5. **Clear on Script Change**
   - When user changes script, consider clearing old bindings
   - Ask user: "Clear existing bindings?"

6. **Provide Defaults**
   - Auto-bind common keys on first setup
   - Offer "Reset to Default Bindings" button

---

## Troubleshooting

### Bindings Not Saving
- Check that `ToJson()` is being called when saving
- Verify JSON is being written to file
- Check file permissions

### Bindings Not Loading
- Verify JSON format is correct
- Check that `SettingsFromJson()` is called on load
- Check console for deserialization errors

### Duplicate Key Errors
- Always call `FindPairIndexByKey()` before adding
- Implement in UI to prevent selection of existing keys

### Function Not Found at Runtime
- Verify Lua script path is correct
- Check function names match exactly (case-sensitive)
- Validate script was parsed without errors

---

## File Locations

- **Header**: `SunshineEngine/include/PlayerObject/PlayerObject.h`
- **Implementation**: `SunshineEngine/src/PlayerObject/PlayerObject.cpp`
- **KeyFunctionPair**: `SunshineEngine/include/PlayerObject/KeyFunctionPair.h`
- **KeyFunctionPair Impl**: `SunshineEngine/src/PlayerObject/KeyFunctionPair.cpp`
- **Examples**: `SunshineEngine/src/PlayerObject/PlayerObject_Editor_Examples.h`

---

## Example Use Cases

### Level Designer Workflow
1. Open scene in editor
2. Select player object
3. Click "Select Script" → Choose `player_controller.lua`
4. Click "+ Add Binding"
5. Select key "W" from dropdown
6. Select function "onMoveForward" from dropdown
7. Click "Add"
8. Repeat for other keys (Space → onJump, E → onInteract, etc.)
9. Save scene
10. Test in game mode

### Gameplay Programmer Workflow
1. Create Lua script with callback functions
2. Create PlayerObject_Info in code
3. Set script path: `SetLuaScriptPath()`
4. Add default bindings: `AddKeyFunctionPair()`
5. Save configuration to JSON
6. Share with level designers

### Modding Workflow
1. Players create custom Lua script
2. Select script in editor
3. Customize input bindings
4. Save scene
5. Others load and play with custom controls

---

## Next Steps

1. **Implement Editor UI** - Create ImGui panel for binding management
2. **Add Lua Function Discovery** - Automatically populate function list from script
3. **Add Binding Profiles** - Save multiple binding configurations
4. **Add Conflict Detection** - Warn when same key is bound to multiple functions
5. **Add Key Remapping at Runtime** - Allow players to customize controls

