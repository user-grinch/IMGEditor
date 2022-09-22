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

Ui::eTheme Ui::Application::GetTheme()
{
    return Ui::Renderer::GetThemeMode();
}

void Ui::Application::SetTheme(Ui::eTheme theme)
{
    Ui::Renderer::SetThemeMode(theme);
}

void Ui::Application::CallDropHandler(const char* path)
{
    if (Info.DragDropFunc)
    {
        Info.DragDropFunc(path);
    }
}

void Ui::Application::Run()
{
    bool exit = false;
    if (Info.DragDropFunc)
    {
        DragAcceptFiles(GetActiveWindow(), TRUE);
    }

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
            Info.CleanupFunc();
            break;
        }

        Renderer::DrawLayer(Info);
    }

    if (Info.DragDropFunc)
    {
        DragAcceptFiles(GetActiveWindow(), FALSE);
    }
}