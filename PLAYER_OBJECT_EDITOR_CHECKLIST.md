# Editor UI Implementation Checklist

## Overview
This document provides a step-by-step checklist for implementing the editor UI for PlayerObject_Info Lua configuration.

## Core Implementation ✅ COMPLETE

- [x] KeyFunctionPair struct created
  - [x] Key and function name fields
  - [x] IsValid() method
  - [x] KeyToString() conversion
  - [x] JSON serialization (ToJson/FromJson)

- [x] PlayerObject_Info fields added
  - [x] m_luaScriptPath field
  - [x] m_keyFunctionMapping vector

- [x] PlayerObject_Info methods implemented
  - [x] Lua script management (Set/Get/Has)
  - [x] Add bindings (two overloads)
  - [x] Edit bindings (three methods for different fields)
  - [x] Delete bindings (by index and clear all)
  - [x] Query bindings (get by index, get all, get count)
  - [x] Find binding by key

- [x] JSON persistence
  - [x] ToJson() serialization
  - [x] SettingsToJson() 
  - [x] SettingsFromJson() loading

## Editor UI Tasks

### Phase 1: Basic UI Panel
- [ ] Create PlayerObjectEditorPanel class in editor
- [ ] Add to ImGui editor interface
- [ ] Display selected PlayerObject_Info
- [ ] Show "Lua Script" section
  - [ ] Display current script path or "Not Set" message
  - [ ] "Select Script" button
  - [ ] File browser integration

### Phase 2: Bindings List Display
- [ ] Create ImGui table for key bindings
  - [ ] Column 1: Key name (left-aligned)
  - [ ] Column 2: Function name (left-aligned)
  - [ ] Column 3: Actions buttons (Edit, Delete)
- [ ] Display binding count
- [ ] Handle empty state (no bindings)
- [ ] Color-code entries (valid=green, invalid=red)

### Phase 3: Add Binding Dialog
- [ ] Create modal dialog "Add Key Binding"
- [ ] Key selection dropdown
  - [ ] Populate from Keys enum
  - [ ] Show key names (W, Space, E, etc.)
- [ ] Function selection
  - [ ] Text input OR dropdown
  - [ ] (Future: auto-populate from Lua script)
- [ ] Validation
  - [ ] Check key not already bound
  - [ ] Check function name not empty
  - [ ] Show error messages
- [ ] Add/Cancel buttons
- [ ] Integration with PlayerObject_Info::AddKeyFunctionPair()

### Phase 4: Edit Binding Dialog
- [ ] Create modal dialog "Edit Key Binding"
- [ ] Display current binding info
- [ ] Key selection dropdown (same as add)
- [ ] Function name input (same as add)
- [ ] Validation
  - [ ] Check for duplicate keys (excluding current pair)
  - [ ] Check function name not empty
- [ ] Update/Cancel buttons
- [ ] Integration with PlayerObject_Info::EditKeyFunctionPair()

### Phase 5: Delete Binding
- [ ] Confirmation dialog when delete button clicked
- [ ] Message: "Delete this binding?"
- [ ] Yes/No buttons
- [ ] Integration with PlayerObject_Info::RemoveKeyFunctionPair()

### Phase 6: File Browser Integration
- [ ] File browser for Lua script selection
- [ ] Filter to .lua files
- [ ] Return AssetPath
- [ ] Validate file exists
- [ ] Integration with PlayerObject_Info::SetLuaScriptPath()

## Implementation Details

### Dropdown Key Selection
```cpp
// Supported keys in dropdown
const char* keyNames[] = { 
    "W", "A", "S", "D", "Space", "E", "Q", 
    "Shift", "Ctrl", "LMB", "RMB", "Enter", "Tab", "Esc"
};

Keys keyValues[] = { 
    Keys::W, Keys::A, Keys::S, Keys::D, Keys::Space, Keys::E, Keys::Q,
    Keys::ShiftKey, Keys::ControlKey, Keys::LeftButton, Keys::RightButton,
    Keys::Enter, Keys::Tab, Keys::Escape
};

static int selectedIdx = 0;
ImGui::Combo("Key", &selectedIdx, keyNames, IM_ARRAYSIZE(keyNames));
Keys selectedKey = keyValues[selectedIdx];
```

### Validation Pattern
```cpp
// Before adding
if (playerInfo.FindPairIndexByKey(selectedKey) == -1 && 
    !selectedFunction.empty()) {
    playerInfo.AddKeyFunctionPair(selectedKey, selectedFunction);
    showSuccess = true;
} else {
    showError = true;
    errorMsg = "Key already bound or function is empty";
}
```

### Bindings Table Pattern
```cpp
if (ImGui::BeginTable("KeyBindings", 3, ImGuiTableFlags_Borders)) {
    ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 80);
    ImGui::TableSetupColumn("Function", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 120);
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
        ImGui::PushID(static_cast<int>(i));
        if (ImGui::SmallButton("Edit")) { /* show edit dialog */ }
        ImGui::SameLine();
        if (ImGui::SmallButton("Delete")) { /* show delete confirmation */ }
        ImGui::PopID();
    }
    ImGui::EndTable();
}
```

## Testing Checklist

### Functionality Tests
- [ ] Can set Lua script path
- [ ] Can add binding
  - [ ] Prevents duplicate keys
  - [ ] Validates function name not empty
- [ ] Can edit binding
  - [ ] Prevents duplicate keys (except current)
  - [ ] Updates correctly in list
- [ ] Can delete binding
  - [ ] Removes from list
  - [ ] Shows confirmation
- [ ] Can clear all bindings
- [ ] Can find binding by key

### UI Tests
- [ ] Panel displays correctly
- [ ] Script path shows when set
- [ ] Bindings list displays all pairs
- [ ] Add dialog appears and closes
- [ ] Edit dialog appears with current values
- [ ] Delete confirmation appears
- [ ] Error messages show on validation failure
- [ ] Success messages show on completion

### Data Persistence Tests
- [ ] Bindings save to JSON
- [ ] JSON format is correct
- [ ] Bindings load from JSON
- [ ] Scene save/load works
- [ ] No data loss after reload

### Edge Cases
- [ ] Empty binding list
- [ ] Duplicate key detection
- [ ] Empty function name validation
- [ ] Invalid script path
- [ ] Very long function names
- [ ] Special characters in function names

## Example Panel Initialization

```cpp
class MyEditorUI {
    PlayerObject_Info* m_currentPlayer = nullptr;
    PlayerObjectEditorPanel m_playerPanel;
    
    void DrawPlayerPanel() {
        if (m_currentPlayer) {
            m_playerPanel.SetTarget(m_currentPlayer);
            m_playerPanel.DrawUI();
        }
    }
};
```

## Future Enhancements

Once basic implementation is complete, consider:

1. **Auto-discovery of Lua Functions**
   - Parse Lua script to find function definitions
   - Auto-populate dropdown with available functions
   - Validate at selection time

2. **Binding Profiles**
   - Save multiple binding configurations
   - Switch between profiles
   - Share binding presets

3. **Input Remapping UI**
   - Allow players to customize controls at runtime
   - Save player preferences
   - Per-player binding profiles

4. **Gamepad Support**
   - Add gamepad buttons to key selection
   - Vibration intensity configuration
   - Dead zone settings

5. **Modifier Key Combinations**
   - Support Ctrl+K, Shift+K combinations
   - Multi-key press detection
   - Combo sequences

6. **Conflict Detection**
   - Warn when function is bound to multiple keys
   - Warn when Lua function doesn't exist
   - Visual highlighting of invalid bindings

## File References

**Implementation Complete:**
- ✅ `SunshineEngine/include/PlayerObject/KeyFunctionPair.h`
- ✅ `SunshineEngine/src/PlayerObject/KeyFunctionPair.cpp`
- ✅ `SunshineEngine/include/PlayerObject/PlayerObject.h`
- ✅ `SunshineEngine/src/PlayerObject/PlayerObject.cpp`

**Examples & Documentation:**
- 📄 `SunshineEngine/src/PlayerObject/PlayerObject_Editor_Examples.h` (10 examples)
- 📄 `PLAYER_OBJECT_LUA_CONFIG_GUIDE.md` (Complete guide)
- 📄 `PLAYER_OBJECT_EDITOR_SUMMARY.md` (Implementation summary)

**To Do:**
- [ ] Create ImGui editor panel (not yet implemented)
- [ ] Integrate into your existing editor UI
- [ ] Add file browser for script selection
- [ ] Wire up all dialogs and callbacks

## Quick Reference: API

```cpp
// Create player info
PlayerObject_Info playerInfo;

// Script management
playerInfo.SetLuaScriptPath(path);
const auto& path = playerInfo.GetLuaScriptPath();
if (playerInfo.HasLuaScript()) { /* ... */ }

// Add bindings
playerInfo.AddKeyFunctionPair(Keys::W, "onMoveForward");

// Edit bindings
playerInfo.EditKeyFunctionPair(index, Keys::A, "onNewFunc");

// Delete bindings
playerInfo.RemoveKeyFunctionPair(index);
playerInfo.ClearKeyFunctionPairs();

// Query bindings
const auto& allPairs = playerInfo.GetAllKeyFunctionPairs();
size_t count = playerInfo.GetKeyFunctionPairCount();
const auto* pair = playerInfo.GetKeyFunctionPair(index);
int index = playerInfo.FindPairIndexByKey(Keys::W);

// JSON persistence
json j = playerInfo.SettingsToJson();
playerInfo.SettingsFromJson(j, renderer);
```

## Next Steps

1. ✅ Core implementation is COMPLETE
2. ⏭️  Next: Create ImGui editor UI panel
3. ⏭️  Then: Integrate with existing editor
4. ⏭️  Then: Test and refine
5. ⏭️  Later: Add future enhancements

---

**Status**: Core API complete, ready for UI implementation!
