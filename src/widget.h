#pragma once
#include "pch.h"

class Widget
{
public:
    Widget() = delete;
    Widget(Widget&&) = delete; 

    // ImGui::TextFilter with hint support
    static void Filter(const char* label, ImGuiTextFilter& filter, const char* hint);
};