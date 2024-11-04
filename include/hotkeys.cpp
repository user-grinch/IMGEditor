#include "hotkeys.h"
#include <Windows.h>

Hotkey::Hotkey(ImGuiKey key1, ImGuiKey key2) {
    codes[0] = defaultCodes[0] = key1;
    
    if (key2 == ImGuiKey_None) {
        codes[1] = defaultCodes[1] = key1;
    } else {
        codes[1] = defaultCodes[1] = key2;
    }
}

bool Hotkey::Pressed(bool noDelay) {
    if (ImGui::GetTime() - lastUpdate < 0.5) return false;

    bool key0Pressed = GetAsyncKeyState(codes[0]) & 0x8000;
    bool key1Pressed = GetAsyncKeyState(codes[1]) & 0x8000;

    if (noDelay) {
        return key0Pressed && key1Pressed;
    } else {
        if (key0Pressed && key1Pressed) {
            wPressed = true;
        } else {
            if (wPressed) {
                wPressed = false;
                return current == "";
            }
        }
    }
    return false;
}