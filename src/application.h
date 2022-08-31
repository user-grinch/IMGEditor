#pragma once
#include "specification.h"
#include "imgui/imgui.h"

namespace Ui
{
    class Application
    {
    private:
        Specification Info;

    public:
        Application(Specification &Spec);
        ~Application();
        
        // Start the application loop
        void Run();
    };
}
