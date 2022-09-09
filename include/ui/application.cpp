#include "application.h"
#include "renderer.h"

Ui::Application::Application(Specification &Spec)
{
    Info = Spec;
    Renderer::Init(Spec);
}

Ui::Application::~Application()
{
    Renderer::Shutdown();
}

void Ui::Application::SetPopup(std::function<void()> Func)
{
    Info.PopupFunc = Func;
}

Ui::CVector2 Ui::Application::GetWindowSize()
{
    return Info.Size;
}

void Ui::Application::Run()
{
    bool exit = false;
    while (!exit)
    {
        // Handle windows messages
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                exit = true;
            }
        }
        if (exit)
        {
            break;
        }

        Renderer::DrawLayer(Info);
    }
}