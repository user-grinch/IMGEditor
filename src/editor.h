#pragma once
#include "ui/application.h"
#include "imgarchive.h"
#include "SimpleIni.h"

class Editor
{
private:
    static inline std::vector<IMGArchive> ArchiveList;
    static inline IMGArchive* pSelectedArchive;         // pointer to the selected archive in editor
    static inline EntryInfo *pContextEntry = nullptr;  // pointer to the selected archive entry in editor
    static inline Ui::Application *pApp = nullptr;
    static inline CSimpleIniA Config;
    static inline std::string ConfigPath = "";
    static inline ImVec2 MonitorScale = {0, 0};
    static inline ImGuiTextFilter Filter = "";


    // Action
    static void NewArchive();
    static void OpenArchive();
    static void SaveArchive();
    static void SaveArchiveAs();
    static void ImportFiles();
    static void ImportAndReplaceFiles();
    static void ExportAll();
    static void ExportSelected();
    static void SelectAll();
    static void SelectInverse();
    static void DumpList();


    // Popups
    static void AboutPopUp();
    static void UpdatePopUp();
    static void WelcomePopup();

    static void ProcessContextMenu();
    static void ProcessMenuBar();
    static void ProcessWindow();

    static void ProcessSelection(EntryInfo *pEntry, IMGArchive *pArchive);

public:

    // Adds a new archive to the list
    static void AddArchiveEntry(IMGArchive &&archive);

    // Returns true if archive with the given name already exists
    static bool DoesArchiveExist(const std::wstring &name);

    // Returns current search filter text
    static const wchar_t* GetFilterText();

    // Initializes & runs the IMGEditor
    static void Run();

    // Sets editor to update found state
    static void SetUpdateFound();

    // Does cleanup for the editor
    static void Shutdown();
};