#pragma once
#include "ui/application.h"
#include "imgmgr.h"

class Editor
{
private:
    static inline IMGArchive *pIMG = nullptr;
    static inline Ui::Application *pApp = nullptr;
    static inline DirEntry *pContextEntry = nullptr;
    static inline ImGuiTextFilter filter;

    // Popups
    static void AboutPopUp();

    static void ProcessContextMenu();
    static void ProcessMenuBar();
    static void ProcessWindow();
public:

    // Initializes & runs the IMGEditor
    static void Run();
};