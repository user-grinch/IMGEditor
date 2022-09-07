#pragma once
#include "ui/application.h"
#include "imgmgr.h"

class Editor
{
private:
    static inline std::vector<IMGArchive> ArchiveList {  IMGArchive("Untitled", true) };
    static inline IMGArchive* pSelectedArchive;         // pointer to the selected archive in editor
    static inline EntryInfo *pContextEntry = nullptr;  // pointer to the selected archive entry in editor
    static inline Ui::Application *pApp = nullptr;

    // Popups
    static void AboutPopUp();
    static void WelcomePopup();

    static void ProcessContextMenu();
    static void ProcessMenuBar();
    static void ProcessWindow();

public:

    // Returns true if archive with the given name already exists
    static bool DoesArchiveExist(const std::string &name);

    // Initializes & runs the IMGEditor
    static void Run();
};