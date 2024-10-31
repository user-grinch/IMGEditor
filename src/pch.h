#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define EDITOR_VERSION_NUM "0.60"
#define EDITOR_VERSION EDITOR_VERSION_NUM"-beta"
#include <filesystem>
#include <windows.h>
#include <format>
#include "imgui/imgui.h"

void ConvertCharToWideChar(const char* charBuffer, wchar_t* wideCharBuffer, size_t wideCharBufferSize);
void ConvertWideCharToChar(const wchar_t* wideCharBuffer, char* charBuffer, size_t charBufferSize);