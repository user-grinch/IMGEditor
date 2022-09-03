#pragma once
#include "ui/application.h"
#include "imgmgr.h"

class Editor
{
private:
    static inline std::vector<IMGArchive> Archives {  IMGArchive("Untitled.img", true) };
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