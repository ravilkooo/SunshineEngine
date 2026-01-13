#include <PlayerObject/KeyFunctionPair.h>

eastl::string KeyFunctionPair::KeyToString(Keys k)
{
    switch (k)
    {
    case Keys::W: return "W";
    case Keys::A: return "A";
    case Keys::S: return "S";
    case Keys::D: return "D";
    case Keys::Space: return "Space";
    // case Keys::ShiftKey: return "Shift";
    // case Keys::ControlKey: return "Ctrl";
    case Keys::E: return "E";
    case Keys::Q: return "Q";
    case Keys::LeftButton: return "LMB";
    case Keys::RightButton: return "RMB";
    case Keys::MiddleButton: return "MMB";
    case Keys::Escape: return "Esc";
    case Keys::Enter: return "Enter";
    case Keys::Tab: return "Tab";
    case Keys::Delete: return "Delete";
    case Keys::Insert: return "Insert";
    case Keys::F1: return "F1";
    case Keys::F2: return "F2";
    case Keys::F3: return "F3";
    case Keys::F4: return "F4";
    case Keys::F5: return "F5";
    case Keys::F6: return "F6";
    case Keys::F7: return "F7";
    case Keys::F8: return "F8";
    case Keys::F9: return "F9";
    case Keys::F10: return "F10";
    case Keys::F11: return "F11";
    case Keys::F12: return "F12";
    case Keys::LeftControl: return "LCtrl";
    case Keys::RightControl: return "RCtrl";
    case Keys::LeftShift: return "LShift";
    case Keys::RightShift: return "RShift";
    case Keys::LeftAlt: return "LAlt";
    case Keys::RightAlt: return "RAlt";
    case Keys::Up: return "Up";
    case Keys::Down: return "Down";
    case Keys::Left: return "Left";
    case Keys::Right: return "Right";
    case Keys::Home: return "Home";
    case Keys::End: return "End";
    case Keys::PageUp: return "PgUp";
    case Keys::PageDown: return "PgDn";
    case Keys::D0: return "0";
    case Keys::D1: return "1";
    case Keys::D2: return "2";
    case Keys::D3: return "3";
    case Keys::D4: return "4";
    case Keys::D5: return "5";
    case Keys::D6: return "6";
    case Keys::D7: return "7";
    case Keys::D8: return "8";
    case Keys::D9: return "9";
    default: return "Unknown";
    }
}
