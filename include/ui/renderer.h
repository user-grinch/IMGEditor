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

    class Renderer
    {
    private:
        static inline IDirect3D9* pD3D = NULL;
        static inline IDirect3DDevice9* pd3dDevice = NULL;
        static inline D3DPRESENT_PARAMETERS d3dpp = {};
        static inline HWND hwnd = NULL;
        static inline WNDCLASSEX wc = {};

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

    public:
        // Draws a ui layer on window
        static void DrawLayer(Specification &Spec);

        // Inits the device
        static void Init(const Specification &Spec);

        // Closes the device & window
        static void Shutdown();
    };
}