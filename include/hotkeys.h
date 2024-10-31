#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>

class Hotkey {
private:
  static inline std::string current;
  bool wPressed = false;
  ImGuiKey codes[2], defaultCodes[2];
  double lastUpdate;

public:
  Hotkey(ImGuiKey key1 = ImGuiKey_None, ImGuiKey key2 = ImGuiKey_None);

  bool Pressed(bool noDelay = false);
};