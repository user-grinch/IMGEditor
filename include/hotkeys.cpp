#include "hotkeys.h"

Hotkey::Hotkey(ImGuiKey key1, ImGuiKey key2) {
    codes[0] = defaultCodes[0] = key1;
    
    if (key2 == ImGuiKey_None) {
        codes[1] = defaultCodes[1] = key1;
    } else {
        codes[1] = defaultCodes[1] = key2;
    }
}

bool Hotkey::Pressed(bool noDelay) {
    if (ImGui::GetTime() - lastUpdate < 2.0) return false;

    if (noDelay) {
        return ImGui::IsKeyDown(codes[0]) && ImGui::IsKeyDown(codes[1]);
    } else {
        if (ImGui::IsKeyDown(codes[0]) && ImGui::IsKeyDown(codes[1])) {
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