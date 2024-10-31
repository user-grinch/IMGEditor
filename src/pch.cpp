#include "pch.h"

void ConvertCharToWideChar(const char* charBuffer, wchar_t* wideCharBuffer, size_t wideCharBufferSize) {
    int requiredSize = MultiByteToWideChar(CP_UTF8, 0, charBuffer, -1, wideCharBuffer, wideCharBufferSize);
    if (requiredSize == 0) {
        return;
    }
    if (requiredSize < wideCharBufferSize) {
        wideCharBuffer[requiredSize] = L'\0'; // Null-terminate the wide char buffer
    }
}