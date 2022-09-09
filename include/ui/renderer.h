#pragma once
#include <d3d9.h>
#include "specification.h"

namespace Ui
{
    enum class eDeviceState
    {
        AlreadyExists,
        CreationFailed,
        CreationSuccess,
    };

    enum class eTheme
    {
        Light,
        Dark,
        SystemDefault
    };

    class Renderer
    {
    private:
        static inline IDirect3D9* pD3D = NULL;
        static inline IDirect3DDevice9* pd3dDevice = NULL;
        static inline D3DPRESENT_PARAMETERS d3dpp = {};
        static inline HWND hwnd = NULL;
        static inline WNDCLASSEX wc = {};
        static inline eTheme Theme = eTheme::SystemDefault;

        // DirectX functions
        static eDeviceState CreateDevice(HWND hWnd);
        static void ResetDevice();
        static void CleanupDevice();
        static inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // Returns true if windows is using light theme
        static bool IsLightTheme();

        // Apply ImGui themes
        static void ApplyLightTheme();
        static void ApplyDarkTheme();
        static void ProcessThemes();

    public:
        // Draws a ui layer on window
        static void DrawLayer(Specification &Spec);

        // Inits the device
        static void Init(const Specification &Spec);

        // Returns the active thememode
        static eTheme GetThemeMode();

        // Sets the current theme 
        static void SetThemeMode(eTheme theme);

        // Closes the device & window
        static void Shutdown();
    };
}