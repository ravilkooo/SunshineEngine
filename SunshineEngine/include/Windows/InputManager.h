#pragma once

#include <Windows/Keys.h>
#include <EASTL/unordered_map.h>
#include <EASTL/unordered_set.h>

/**
 * InputManager - Robust input handling with proper edge detection
 * 
 * Tracks key states across frames to provide:
 * - IsKeyDown()     : Key is currently held down
 * - IsKeyPressed()  : Key was just pressed this frame (edge: up->down)
 * - IsKeyReleased() : Key was just released this frame (edge: down->up)
 * 
 * Usage:
 *   1. Call ProcessKeyDown(key) / ProcessKeyUp(key) from OS events
 *   2. Call Update() once per frame AFTER all event processing
 *   3. Query key states during game logic
 */
class InputManager
{
public:
    InputManager();
    ~InputManager();

    /**
     * Process raw key events from OS/InputDevice
     * Call these from your event handlers
     */
    void ProcessKeyDown(Keys key);
    void ProcessKeyUp(Keys key);

    /**
     * Update input state - MUST be called once per frame
     * Call this at the start or end of your game loop, after processing all events
     */
    void Update();

    /**
     * Query methods - use these in game logic
     */
    
    // Returns true if key is currently held down (includes press frame)
    bool IsKeyDown(Keys key) const;
    
    // Returns true only on the frame when key transitions from up to down
    bool IsKeyPressed(Keys key) const;
    
    // Returns true only on the frame when key transitions from down to up
    bool IsKeyReleased(Keys key) const;

    /**
     * Get all keys currently held down
     */
    const eastl::unordered_set<Keys>& GetKeysDown() const { return m_currentKeys; }

    /**
     * Check if any key is down
     */
    bool IsAnyKeyDown() const { return !m_currentKeys.empty(); }

    /**
     * Reset all input state (useful when switching modes, e.g., game <-> editor)
     */
    void Reset();

    /**
     * Enable/disable input processing (useful for UI focus, etc.)
     */
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }

    // Current frame: keys that are down
    eastl::unordered_set<Keys> m_currentKeys;

    // Previous frame: keys that were down
    eastl::unordered_set<Keys> m_previousKeys;

    // Keys pressed this frame (edge detection cache)
    eastl::unordered_set<Keys> m_pressedThisFrame;

    // Keys released this frame (edge detection cache)
    eastl::unordered_set<Keys> m_releasedThisFrame;

private:
    // Enable/disable flag
    bool m_enabled;

    // Helper to compute edge events
    void ComputeEdgeEvents();
};
