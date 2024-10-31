#pragma once
#include <string>

class WinDialogs
{
public:
    WinDialogs() = delete;
    WinDialogs(WinDialogs&&) = delete;

    static std::wstring ImportFiles();
    static std::wstring OpenFile();
    static size_t SaveArchive(std::wstring &path);
    static std::wstring ExportFile(const wchar_t* fileName);
    static std::wstring SaveFolder();
};