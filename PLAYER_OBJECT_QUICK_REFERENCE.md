# PlayerObject_Info Lua Configuration - Quick Reference

## What Was Implemented ✅

```
PlayerObject_Info
├── Lua Script Path (AssetPath)
│   ├── SetLuaScriptPath(path)
│   ├── GetLuaScriptPath()
│   └── HasLuaScript()
│
└── Key-Function Bindings (vector<KeyFunctionPair>)
    ├── Add
    │   ├── AddKeyFunctionPair(key, functionName)
    │   └── AddKeyFunctionPair(KeyFunctionPair)
    │
    ├── Edit
    │   ├── EditKeyFunctionPair(idx, key, func)
    │   ├── EditKeyFunctionPairKey(idx, key)
    │   └── EditKeyFunctionPairFunction(idx, func)
    │
    ├── Delete
    │   ├── RemoveKeyFunctionPair(idx)
    │   └── ClearKeyFunctionPairs()
    │
    └── Query
        ├── GetKeyFunctionPair(idx)
        ├── GetAllKeyFunctionPairs()
        ├── GetKeyFunctionPairCount()
        └── FindPairIndexByKey(key)
```

## Architecture Overview

```
Editor UI
   ↓
PlayerObject_Info (Data Model)
   ├── m_luaScriptPath (AssetPath)
   └── m_keyFunctionMapping (vector<KeyFunctionPair>)
   ↓
JSON Serialization
   ├── ToJson()
   ├── SettingsToJson()
   └── SettingsFromJson()
   ↓
Scene File (JSON)
   ↓
Game Runtime
   ↓
PlayerObject → m_luaActionMapping
   ↓
InputManager + Lua Callbacks
```

## Data Flow

### Setting Up in Editor

```
1. User selects player object
        ↓
2. Editor loads PlayerObject_Info from scene
        ↓
3. User selects Lua script file
        ↓
4. playerInfo.SetLuaScriptPath(path)
        ↓
5. UI shows "Script Set ✓"
        ↓
6. User clicks "+ Add Binding"
        ↓
7. Dialog: Select Key dropdown + Function dropdown
        ↓
8. User selects "W" and "onMoveForward"
        ↓
9. playerInfo.AddKeyFunctionPair(Keys::W, "onMoveForward")
        ↓
10. Binding appears in table
        ↓
11. User saves scene
        ↓
12. JSON saves script + all bindings
```

### At Runtime

```
Game Loads Scene
        ↓
Scene contains PlayerObject_Info config
        ↓
PlayerObject created from config
        ↓
m_luaActionMapping initialized with:
    ├── Script path (from m_luaScriptPath)
    └── Bindings (from m_keyFunctionMapping)
        ↓
Game loop processes input
        ↓
InputManager.Update()
        ↓
If key pressed → m_luaActionMapping.ExecuteKeyAction(key)
        ↓
Lua callback function executed
```

## JSON Structure

```json
{
  "playerObject": {
    "m_UUID": 1234567890,
    "m_name": "PlayerObject",
    "m_group": 0,
    "components": { /* ... */ },
    
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

## Editor UI Layout

```
╔═══════════════════════════════════════════════════╗
║ PLAYER OBJECT CONFIGURATION                       ║
╠═══════════════════════════════════════════════════╣
║                                                   ║
║ Lua Script:  ✓ Set                               ║
║            [Select Script]                       ║
║            Path: Projects/Templates/...lua       ║
║                                                   ║
╠═══════════════════════════════════════════════════╣
║ Key Bindings (3 total)                           ║
╠════════════════╦═══════════════╦════════════════╣
║ Key            ║ Function      ║ Actions        ║
╠════════════════╬═══════════════╬════════════════╣
║ W              ║ onMoveForward ║ Edit  Delete   ║
║ Space          ║ onJump        ║ Edit  Delete   ║
║ E              ║ onInteract    ║ Edit  Delete   ║
╠════════════════╩═══════════════╩════════════════╣
║                                                   ║
║ [+ Add Binding]                                  ║
║                                                   ║
╚═══════════════════════════════════════════════════╝
```

## Common Operations

### Add Binding
```
User Input                  Code
──────────────────────────────────────────
Select Key: W       →   selectedKey = Keys::W
Select Function:    →   selectedFunc = "onMoveForward"
    onMoveForward

Click "Add"         →   if (playerInfo.FindPairIndexByKey(Keys::W) == -1) {
                            playerInfo.AddKeyFunctionPair(
                                Keys::W, "onMoveForward");
                        }

Table updates       ←   Display all pairs in UI
```

### Edit Binding
```
User Input                  Code
──────────────────────────────────────────
Click "Edit" on    →   const auto* pair = 
  W → onMove           playerInfo.GetKeyFunctionPair(idx);
  
Dialog shows       ←   Current: W, onMoveForward
  current values

Change to:         →   newKey = Keys::A
  A → onNewFunc        newFunc = "onNewFunc"

Click "Update"     →   playerInfo.EditKeyFunctionPair(
                        idx, Keys::A, "onNewFunc");

Table updates      ←   Display updated binding
```

### Delete Binding
```
User Input                  Code
──────────────────────────────────────────
Click "Delete"     →   Show confirmation dialog

Confirm            →   playerInfo.RemoveKeyFunctionPair(idx);

Table updates      ←   Binding removed from display
```

## Files Structure

```
SunshineEngine/
├── include/
│   ├── PlayerObject/
│   │   ├── PlayerObject.h ................. Header with API
│   │   ├── KeyFunctionPair.h ............. Struct definition
│   │   └── /* other files */
│   └── /* other headers */
│
└── src/
    ├── PlayerObject/
    │   ├── PlayerObject.cpp .............. Implementation + JSON
    │   ├── KeyFunctionPair.cpp ........... KeyToString() impl
    │   ├── PlayerObject_Editor_Examples.h  10 examples + ImGui
    │   └── /* other files */
    └── /* other sources */

Documentation/
├── PLAYER_OBJECT_LUA_CONFIG_GUIDE.md ... Full guide
├── PLAYER_OBJECT_EDITOR_SUMMARY.md .... Implementation summary
└── PLAYER_OBJECT_EDITOR_CHECKLIST.md .. UI checklist
```

## API Cheat Sheet

```cpp
// ========== SCRIPT MANAGEMENT ==========
void SetLuaScriptPath(const AssetPath& scriptPath);
const AssetPath& GetLuaScriptPath() const;
bool HasLuaScript() const;

// ========== ADD BINDINGS ==========
void AddKeyFunctionPair(Keys key, const eastl::string& functionName);
void AddKeyFunctionPair(const KeyFunctionPair& pair);

// ========== EDIT BINDINGS ==========
bool EditKeyFunctionPair(size_t index, Keys newKey, 
                        const eastl::string& newFunctionName);
bool EditKeyFunctionPairKey(size_t index, Keys newKey);
bool EditKeyFunctionPairFunction(size_t index, 
                                 const eastl::string& newFunctionName);

// ========== DELETE BINDINGS ==========
bool RemoveKeyFunctionPair(size_t index);
void ClearKeyFunctionPairs();

// ========== QUERY BINDINGS ==========
const KeyFunctionPair* GetKeyFunctionPair(size_t index) const;
const eastl::vector<KeyFunctionPair>& GetAllKeyFunctionPairs() const;
size_t GetKeyFunctionPairCount() const;
int FindPairIndexByKey(Keys key) const;
```

## Status: ✅ COMPLETE & READY

### Implemented
- ✅ KeyFunctionPair struct with utilities
- ✅ All management methods in PlayerObject_Info
- ✅ JSON serialization (save/load)
- ✅ Comprehensive documentation (3 guides)
- ✅ 10 detailed code examples
- ✅ ImGui panel pattern example

### Next: Editor UI Implementation
- ⏳ Create ImGui panel in your editor
- ⏳ Wire up dialogs and callbacks
- ⏳ Integrate with existing editor UI
- ⏳ Test and refine

See `PLAYER_OBJECT_EDITOR_CHECKLIST.md` for UI implementation checklist.

---

**TL;DR**: Add/edit/delete Lua key bindings in editor, save to JSON, load at runtime!
