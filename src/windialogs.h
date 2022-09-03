#pragma once
#include <string>

class WinDialogs
{
public:
    WinDialogs() = delete;
    WinDialogs(WinDialogs&&) = delete;

    static std::string OpenFile();
};