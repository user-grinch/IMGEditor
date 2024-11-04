#pragma once
#include <windows.h>

class Utils {
public:
    // Convert a UTF-8 char buffer to a wide char buffer
    static void ConvertUtf8ToWide(const char* utf8Str, wchar_t* wideStr, size_t wideStrSize);

    // Convert a wide char buffer to a UTF-8 char buffer
    static void ConvertWideToUtf8(const wchar_t* wideStr, char* utf8Str, size_t utf8StrSize);

    // Convert a wide char string to lowercase
    static void ToLowerCase(wchar_t* wideStr);
};