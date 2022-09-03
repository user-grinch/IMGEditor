#pragma once
#include "pch.h"

class Widget
{
public:
    Widget() = delete;
    Widget(Widget&&) = delete; 

    // Calculates button size based on window width & spacing flags
    static ImVec2 CalcSize(short count = 1, bool spacing = true, bool column = false);

    // ImGui::TextFilter with hint support
    static void Filter(const char* label, ImGuiTextFilter& filter, const char* hint);
    
};