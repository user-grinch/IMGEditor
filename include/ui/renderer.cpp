#include "renderer.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#include "resource.h"

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

bool Ui::Renderer::IsLightTheme() {
    // based on https://stackoverflow.com/questions/51334674/how-to-detect-windows-10-light-dark-mode-in-win32-application

    // The value is expected to be a REG_DWORD, which is a signed 32-bit little-endian
    auto buffer = std::vector<char>(4);
    auto cbData = static_cast<DWORD>(buffer.size() * sizeof(char));
    auto res = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD, // expected value type
        nullptr,
        buffer.data(),
        &cbData);

    // return true by default
    if (res != ERROR_SUCCESS) {
        return true;
    }

    // convert bytes written to our buffer to an int, assuming little-endian
    auto i = int(buffer[3] << 24 |
        buffer[2] << 16 |
        buffer[1] << 8 |
        buffer[0]);

    return i == 1;
}

void Ui::Renderer::ApplyLightTheme()
{
    ImGuiStyle *style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;
    
    style->AntiAliasedLines = true;
    style->FrameBorderSize = 1.0f;
    style->WindowBorderSize = 0.00f;
    style->PopupBorderSize =1.0f;
    style->ScrollbarRounding = 0.0f;
    style->TabRounding = 0.0f;
    style->TabBorderSize = 1.0f;

    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.98f, 0.98f, 0.98f, 0.0f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.58f, 1.0f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.65f, 0.75f, 1.00f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.97f, 0.97f, 0.97f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.78f, 0.87f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.71f, 0.81f, 1.00f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.78f, 0.87f, 0.94f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.78f, 0.87f, 0.98f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.73f, 0.82f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.35f, 0.35f, 0.35f, 0.17f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.68f, 0.78f, 0.98f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.71f, 0.81f, 1.00f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = colors[ImGuiCol_Tab];
    colors[ImGuiCol_TabUnfocusedActive]     = colors[ImGuiCol_TabActive];
    colors[ImGuiCol_PlotLines]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.73f, 0.82f, 0.98f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.78f, 0.87f, 0.98f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.57f, 0.57f, 0.64f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.68f, 0.68f, 0.74f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.78f, 0.87f, 0.98f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.0f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void Ui::Renderer::ApplyDarkTheme()
{
    ImGuiStyle *style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;
    
    style->AntiAliasedLines = true;
    style->FrameBorderSize = 1.0f;
    style->WindowBorderSize = 0.00f;
    style->PopupBorderSize =1.0f;
    style->ScrollbarRounding = 0.0f;
    style->TabRounding = 0.0f;
    style->TabBorderSize = 1.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.33f, 0.3f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.06f, 0.95f);
    style->Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.05f, 0.06f, 0.95f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.94f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5f, 0.5f, 0.5f, 0.3f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.7f, 0.7f, 0.7f, 0.3f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.9f, 0.9f, 0.9f, 0.3f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_Separator] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.12f, 0.12f, 0.12f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.06f, 0.05f, 0.06f, 0.95f);
    style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.6f);
    style->Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);  
    style->Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
}

Ui::eDeviceState Ui::Renderer::CreateDevice(HWND hWnd)
{
    if (pd3dDevice)
    {
        return eDeviceState::AlreadyExists;
    }
    
    pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (pD3D == NULL)
    {
        return eDeviceState::CreationFailed;
    }

    // Create the D3DDevice
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice) < 0)
    {
        return eDeviceState::CreationFailed;
    }

    return eDeviceState::CreationSuccess;
}

void Ui::Renderer::CleanupDevice()
{
    if (pd3dDevice) 
    { 
        pd3dDevice->Release(); 
        pd3dDevice = NULL; 
    }

    if (pD3D) 
    { 
        pD3D->Release(); 
        pD3D = NULL; 
    }
}

void Ui::Renderer::ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = pd3dDevice->Reset(&d3dpp);
    if (hr == D3DERR_INVALIDCALL)
    {
        assert(0);
    }
    ImGui_ImplDX9_CreateDeviceObjects();
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI Ui::Renderer::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            d3dpp.BackBufferWidth = LOWORD(lParam);
            d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void Ui::Renderer::ProcessThemes()
{
    BOOL useDarkMode = TRUE;
    bool isLightMode = IsLightTheme();
    static bool prevIsLightMode = !isLightMode;
    static Ui::eTheme prevTheme = eTheme::SystemDefault;

    // run once on change
    if (isLightMode != prevIsLightMode || prevTheme != Theme)
    {
        if ((isLightMode && Theme == eTheme::SystemDefault) || Theme == eTheme::Light)
        {
            useDarkMode = FALSE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
            ApplyLightTheme();
        }
        else
        {
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
            ApplyDarkTheme();
        }

        ShowWindow(hwnd, false);
        UpdateWindow(hwnd);
        ShowWindow(hwnd, true);

        prevIsLightMode = isLightMode;
        prevTheme = Theme;
    }
}

void Ui::Renderer::DrawLayer(Specification &Spec)
{
    if (!hwnd)
    {
        return;
    }
    
    ProcessThemes();
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (Spec.MenuBarFunc)
    {
        if (ImGui::BeginMainMenuBar())
        {
            Spec.MenuBarFunc();
            ImGui::EndMainMenuBar();
        }
    }

    if (Spec.LayerFunc)
    {
        float start = ImGui::GetFrameHeight() - 1.0f;
        RECT rect;
        GetWindowRect(hwnd, &rect);
        float width = static_cast<float>(rect.right - rect.left - ImGui::GetStyle().WindowPadding.x*2);
        float height = static_cast<float>(rect.bottom - rect.top - start - ImGui::GetStyle().WindowPadding.y*4.5);

        ImGui::SetNextWindowPos(ImVec2(0, start), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
        if (ImGui::Begin("MainWindow", NULL, flags))
        {
            Spec.LayerFunc();
            ImGui::End();
        }

        if (Spec.PopupFunc)
        {
            static ImVec2 sz;
            float posX = (width - sz.x)/2;
            float posY = (height - sz.y)/4;
            ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_Always);
            flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
            bool state = true;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.5f);
            ImGui::SetNextWindowFocus();
            if (ImGui::Begin("##PopupWindow", &state, flags))
            {
                Spec.PopupFunc();
                sz = ImGui::GetWindowSize();
                ImGui::End();
            }
            ImGui::PopStyleVar();

            if (!state)
            {
                Spec.PopupFunc = nullptr;
            }
        }
    }
    ImGui::EndFrame();

    pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
    pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
    if (pd3dDevice->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        pd3dDevice->EndScene();
    }
    HRESULT result = pd3dDevice->Present(NULL, NULL, NULL, NULL);
    if (result == D3DERR_DEVICELOST && pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
    {
        ResetDevice();
    }
}

void Ui::Renderer::Init(const Specification &Spec)
{
    ImGui_ImplWin32_EnableDpiAwareness();
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_APPICON));
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = Spec.Name.c_str();
    wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_APPICON));

    RegisterClassEx(&wc);
    hwnd = CreateWindow(wc.lpszClassName,  Spec.Name.c_str(), WS_OVERLAPPEDWINDOW, Spec.Pos.x, Spec.Pos.y, Spec.Size.x, Spec.Size.y, 
                        NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (CreateDevice(hwnd) == eDeviceState::CreationFailed)
    {
        CleanupDevice();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    // get screen info
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info;
    info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &info);
    int h = info.rcMonitor.bottom - info.rcMonitor.top;
    io.FontDefault = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 17.0f * h/768);
    io.Fonts->Build();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(pd3dDevice);
}

void Ui::Renderer::SetThemeMode(eTheme themeMode)
{
    Theme = themeMode;
}

Ui::eTheme Ui::Renderer::GetThemeMode()
{
    return Theme;
}

void Ui::Renderer::Shutdown()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDevice();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
}