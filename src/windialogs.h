#pragma once
#include <string>

class WinDialogs
{
public:
    WinDialogs() = delete;
    WinDialogs(WinDialogs&&) = delete;

    static std::string ImportFiles();
    static std::string OpenFile();
    static size_t SaveArchive(std::string &path);
    static std::string ExportFile(const char* fileName);
    static std::string SaveFolder();
};