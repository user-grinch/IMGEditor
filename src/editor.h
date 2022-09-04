#pragma once
#include "ui/application.h"
#include "imgmgr.h"

class Editor
{
private:
    static inline std::vector<IMGArchive> Archives {  IMGArchive("Untitled", true) };
    static inline IMGArchive* pSelectedArchive;
    static inline EntryInfo *pSelectedEntry = nullptr;
    static inline Ui::Application *pApp = nullptr;

    // Popups
    static void AboutPopUp();
    static void WelcomePopup();

    static void ProcessContextMenu();
    static void ProcessMenuBar();
    static void ProcessWindow();
public:

    // Initializes & runs the IMGEditor
    static void Run();
};