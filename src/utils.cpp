#include "pch.h"
#include "utils.h"
#include <assert.h>
#include <codecvt>

void Utils::ConvertUtf8ToWide(const char* utf8Str, size_t utf8StrSize, wchar_t* wideStr, size_t wideStrSize) {
    if (utf8Str == nullptr || wideStr == nullptr || wideStrSize == 0) {
        return;
    }

    // Convert UTF-8 to wide string using std::wstring_convert
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wideString = converter.from_bytes(utf8Str, utf8Str + utf8StrSize);

    // Copy to the provided wideStr buffer with size constraints
    size_t copySize = (wideString.size() < wideStrSize - 1) ? wideString.size() : (wideStrSize - 1);
    std::wmemcpy(wideStr, wideString.c_str(), copySize);
    wideStr[copySize] = L'\0'; // Null-terminate
}

void Utils::ConvertWideToUtf8(const wchar_t* wideStr, size_t wideStrSize, char* utf8Str, size_t utf8StrSize) {
    if (wideStr == nullptr || utf8Str == nullptr || utf8StrSize == 0) {
        return;
    }

    // Convert wide string to UTF-8 using std::wstring_convert
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8String = converter.to_bytes(wideStr, wideStr + wideStrSize);

    // Copy to the provided utf8Str buffer with size constraints
    size_t copySize = (utf8String.size() < utf8StrSize - 1) ? utf8String.size() : (utf8StrSize - 1);
    std::memcpy(utf8Str, utf8String.c_str(), copySize);
    utf8Str[copySize] = '\0'; // Null-terminate
}

void Utils::ToLowerCase(wchar_t* wideStr) {
    if (wideStr == nullptr) return;

    for (size_t i = 0; wideStr[i] != L'\0'; ++i) {
        if (wideStr[i] >= L'A' && wideStr[i] <= L'Z') {
            wideStr[i] = wideStr[i] + (L'a' - L'A'); 
        }
    }
}
