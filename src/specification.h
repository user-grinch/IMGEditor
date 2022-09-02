#pragma once
#include <string>
#include <functional>

namespace Ui
{
    struct CVector2
    {
        int x;
        int y;
    };

    struct Specification 
    {
        std::string Name = "Untitled Window";
        CVector2 Pos = { 200, 200 };
        CVector2 Size = { 600, 400 };
        std::function<void()> LayerFunc = nullptr, MenuBarFunc = nullptr, PopupFunc = nullptr;
    };
}

