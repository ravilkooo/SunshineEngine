#pragma once

#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>
#include <EASTL/string.h>
#include <Windows/Keys.h>

struct KeyInfo
{
    Keys Key;
    const char* Name;
};

static const eastl::vector<KeyInfo> g_AllKeys =
{
    // Mouse buttons
    { Keys::LeftButton, "LMB" },
    { Keys::RightButton, "RMB" },
    { Keys::MiddleButton, "MMB" },
    { Keys::MouseButtonX1, "Mouse X1" },
    { Keys::MouseButtonX2, "Mouse X2" },
    { Keys::MouseButtonX3, "Mouse X3" },
    { Keys::WheelUp, "Wheel Up" },
    { Keys::WheelDown, "Wheel Down" },

    // Navigation & control keys
    { Keys::Back, "Backspace" },
    { Keys::Tab, "Tab" },
    { Keys::Enter, "Enter" },
    { Keys::LeftControl, "Left Ctrl" },
    { Keys::Pause, "Pause" },
    { Keys::CapsLock, "Caps Lock" },
    { Keys::Kana, "Kana" },
    { Keys::Kanji, "Kanji" },
    { Keys::Escape, "Escape" },
    { Keys::ImeConvert, "IME Convert" },
    { Keys::ImeNoConvert, "IME No Convert" },
    { Keys::Space, "Space" },
    { Keys::PageUp, "Page Up" },
    { Keys::PageDown, "Page Down" },
    { Keys::End, "End" },
    { Keys::Home, "Home" },
    { Keys::Left, "Left" },
    { Keys::Up, "Up" },
    { Keys::Right, "Right" },
    { Keys::Down, "Down" },
    { Keys::Select, "Select" },
    { Keys::Print, "Print" },
    { Keys::Execute, "Execute" },
    { Keys::PrintScreen, "Print Screen" },
    { Keys::Insert, "Insert" },
    { Keys::Delete, "Delete" },
    { Keys::Help, "Help" },

    // Numbers (top row)
    { Keys::D0, "0" },
    { Keys::D1, "1" },
    { Keys::D2, "2" },
    { Keys::D3, "3" },
    { Keys::D4, "4" },
    { Keys::D5, "5" },
    { Keys::D6, "6" },
    { Keys::D7, "7" },
    { Keys::D8, "8" },
    { Keys::D9, "9" },

    // Letters
    { Keys::A, "A" },
    { Keys::B, "B" },
    { Keys::C, "C" },
    { Keys::D, "D" },
    { Keys::E, "E" },
    { Keys::F, "F" },
    { Keys::G, "G" },
    { Keys::H, "H" },
    { Keys::I, "I" },
    { Keys::J, "J" },
    { Keys::K, "K" },
    { Keys::L, "L" },
    { Keys::M, "M" },
    { Keys::N, "N" },
    { Keys::O, "O" },
    { Keys::P, "P" },
    { Keys::Q, "Q" },
    { Keys::R, "R" },
    { Keys::S, "S" },
    { Keys::T, "T" },
    { Keys::U, "U" },
    { Keys::V, "V" },
    { Keys::W, "W" },
    { Keys::X, "X" },
    { Keys::Y, "Y" },
    { Keys::Z, "Z" },

    // Windows keys
    { Keys::LeftWindows, "Left Win" },
    { Keys::RightWindows, "Right Win" },
    { Keys::Apps, "Apps" },
    { Keys::Sleep, "Sleep" },

    // Numpad
    { Keys::NumPad0, "Num 0" },
    { Keys::NumPad1, "Num 1" },
    { Keys::NumPad2, "Num 2" },
    { Keys::NumPad3, "Num 3" },
    { Keys::NumPad4, "Num 4" },
    { Keys::NumPad5, "Num 5" },
    { Keys::NumPad6, "Num 6" },
    { Keys::NumPad7, "Num 7" },
    { Keys::NumPad8, "Num 8" },
    { Keys::NumPad9, "Num 9" },
    { Keys::Multiply, "Num *" },
    { Keys::Add, "Num +" },
    { Keys::Separator, "Num Separator" },
    { Keys::Subtract, "Num -" },
    { Keys::Decimal, "Num ." },
    { Keys::Divide, "Num /" },

    // Function keys
    { Keys::F1, "F1" },
    { Keys::F2, "F2" },
    { Keys::F3, "F3" },
    { Keys::F4, "F4" },
    { Keys::F5, "F5" },
    { Keys::F6, "F6" },
    { Keys::F7, "F7" },
    { Keys::F8, "F8" },
    { Keys::F9, "F9" },
    { Keys::F10, "F10" },
    { Keys::F11, "F11" },
    { Keys::F12, "F12" },
    { Keys::F13, "F13" },
    { Keys::F14, "F14" },
    { Keys::F15, "F15" },
    { Keys::F16, "F16" },
    { Keys::F17, "F17" },
    { Keys::F18, "F18" },
    { Keys::F19, "F19" },
    { Keys::F20, "F20" },
    { Keys::F21, "F21" },
    { Keys::F22, "F22" },
    { Keys::F23, "F23" },
    { Keys::F24, "F24" },

    // Lock keys
    { Keys::NumLock, "Num Lock" },
    { Keys::Scroll, "Scroll Lock" },

    // Shift and modifier keys
    { Keys::LeftShift, "Left Shift" },
    { Keys::RightShift, "Right Shift" },
    { Keys::RightControl, "Right Ctrl" },
    { Keys::LeftAlt, "Left Alt" },
    { Keys::RightAlt, "Right Alt" },

    // Browser keys
    { Keys::BrowserBack, "Browser Back" },
    { Keys::BrowserForward, "Browser Forward" },
    { Keys::BrowserRefresh, "Browser Refresh" },
    { Keys::BrowserStop, "Browser Stop" },
    { Keys::BrowserSearch, "Browser Search" },
    { Keys::BrowserFavorites, "Browser Favorites" },
    { Keys::BrowserHome, "Browser Home" },

    // Media keys
    { Keys::VolumeMute, "Volume Mute" },
    { Keys::VolumeDown, "Volume Down" },
    { Keys::VolumeUp, "Volume Up" },
    { Keys::MediaNextTrack, "Next Track" },
    { Keys::MediaPreviousTrack, "Prev Track" },
    { Keys::MediaStop, "Media Stop" },
    { Keys::MediaPlayPause, "Play/Pause" },
    { Keys::LaunchMail, "Launch Mail" },
    { Keys::SelectMedia, "Select Media" },
    { Keys::LaunchApplication1, "Launch App 1" },
    { Keys::LaunchApplication2, "Launch App 2" },

    // OEM keys
    { Keys::OemSemicolon, ";" },
    { Keys::OemPlus, "+" },
    { Keys::OemComma, "," },
    { Keys::OemMinus, "-" },
    { Keys::OemPeriod, "." },
    { Keys::OemQuestion, "/" },
    { Keys::OemTilde, "~" },
    { Keys::ChatPadGreen, "ChatPad Green" },
    { Keys::ChatPadOrange, "ChatPad Orange" },
    { Keys::OemOpenBrackets, "[" },
    { Keys::OemPipe, "|" },
    { Keys::OemCloseBrackets, "]" },
    { Keys::OemQuotes, "'" },
    { Keys::Oem8, "OEM 8" },
    { Keys::OemBackslash, "\\" },
    { Keys::ProcessKey, "Process" },
    { Keys::OemCopy, "OEM Copy" },
    { Keys::OemAuto, "OEM Auto" },
    { Keys::OemEnlW, "OEM ENLW" },
    { Keys::Attn, "Attn" },
    { Keys::Crsel, "CrSel" },
    { Keys::Exsel, "ExSel" },
    { Keys::EraseEof, "Erase EOF" },
    { Keys::Play, "Play" },
    { Keys::Zoom, "Zoom" },
    { Keys::Pa1, "PA1" },
    { Keys::OemClear, "OEM Clear" },
};

static std::string KeyToName(Keys k)
{
    // Use static lookup map for O(1) access
    static const eastl::unordered_map<Keys, const char*> keyToStringMap = []
        {
            eastl::unordered_map<Keys, const char*> map;
            for (const auto& keyInfo : g_AllKeys)
            {
                map[keyInfo.Key] = keyInfo.Name;
            }
            return map;
        }();

    auto it = keyToStringMap.find(k);
    if (it != keyToStringMap.end())
    {
        return it->second;
    }

    // Fallback for unknown keys
    return std::string("Unknown(") + std::to_string(static_cast<int>(k)) + ")";
}