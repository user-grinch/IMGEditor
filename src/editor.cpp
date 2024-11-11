#include "pch.h"
#include "editor.h"
#include "widget.h"
#include "windialogs.h"
#include "updater.h"
#include "hotkeys.h"
#include "utils.h"

Hotkey closeTab {VK_SHIFT, 0x58};          // VK_X
Hotkey selectAll {VK_CONTROL, 0x41};       // VK_A
Hotkey selectInverse {VK_SHIFT, 0x41};     // VK_A
Hotkey openFile {VK_CONTROL, 0x4F};        // VK_O
Hotkey newFile {VK_CONTROL, 0x4E};         // VK_N
Hotkey saveFile {VK_CONTROL, 0x53};        // VK_S
Hotkey saveFileAs {VK_SHIFT, 0x53};        // VK_S
Hotkey importFile {VK_CONTROL, 0x49};      // VK_I
Hotkey importReplaceFile {VK_SHIFT, 0x49}; // VK_I
Hotkey exportFile {VK_CONTROL, 0x45};       // VK_E
Hotkey exportSelectedFile {VK_SHIFT, 0x45}; // VK_E

void Editor::AboutPopUp()
{
    ImGui::Columns(2, nullptr, false);
    ImGui::Text("Author: Grinch_");
    ImGui::Text("Version: " EDITOR_VERSION);
    ImGui::Spacing();
    ImGui::NextColumn();
    ImGui::Text("ImGui: %s", ImGui::GetVersion());
    ImGui::Text("Build: %s", __DATE__);
    ImGui::Columns(1);
    
    ImGui::Dummy(ImVec2(0, 10));
    Widget::TextCentered("Credits");
    ImGui::Columns(2, NULL, false);

    ImGui::Text("Freetype");
    ImGui::Text("ImGui");
    ImGui::NextColumn();
    ImGui::Text("SimpleINI");
    ImGui::Columns(1);
    ImGui::Dummy(ImVec2(0, 10));

    if (ImGui::Button("GitHub", Widget::CalcSize(2)))
    {
        ShellExecute(nullptr, "open", "https://github.com/user-grinch/IMGEditor/", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::SameLine();
    if (ImGui::Button("Patreon", Widget::CalcSize(2)))
    {
        ShellExecute(nullptr, "open", "https://www.patreon.com/grinch_", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::Spacing();
    ImGui::Text("Copyright Grinch_ 2022-2025. All rights reserved.");
}

void Editor::UpdatePopUp()
{
    Widget::TextCentered("A new update is available");
    Widget::TextCentered("Current version: " EDITOR_VERSION);
    Widget::TextCentered(std::format("Latest version: {}", Updater::GetUpdateVersion()));
    ImGui::Spacing();
    Widget::TextCentered("Update to continue using the Editor");
    ImGui::Spacing();
    ImGui::TextWrapped("Newer versions may contain more features & bug fixes. Updates will be optional once the editor is stable.");
    ImGui::Dummy(ImVec2(0, 15));
    if (ImGui::Button("Download update", Widget::CalcSize()))
    {
        ShellExecute(nullptr, "open", "https://github.com/user-grinch/IMGEditor/releases/", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::Spacing();
    ImGui::Text("Copyright Grinch_ 2022-2025. All rights reserved.");
}

void Editor::WelcomePopup()
{
    Widget::TextCentered("Welcome to IMG Editor v" EDITOR_VERSION);
    Widget::TextCentered("by Grinch_");
    ImGui::Spacing();
    ImGui::TextWrapped("This editor is still work in progress. There may be LOT of bugs. Feel free to report if you find any.");
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::Text("Supported IMG formats,");
    ImGui::Text("1. GTA 3");
    ImGui::Text("2. GTA VC");
    ImGui::Text("3. GTA SA");
    ImGui::Text("4. Bully Scholarship Edition");
    ImGui::Spacing();
    ImGui::TextWrapped("Directly open archives in IMG Editor by setting windows file association.");
    ImGui::Spacing();
    if (ImGui::Button("Help with file association",  Widget::CalcSize(1)))
    {
        ShellExecute(nullptr, "open", "https://www.google.com/search?q=windows+set+file+associations", nullptr, nullptr, SW_SHOWNORMAL);
    }
     ImGui::Spacing();
    if (ImGui::Button("GitHub", Widget::CalcSize(2)))
    {
        ShellExecute(nullptr, "open", "https://github.com/user-grinch/IMGEditor/", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::SameLine();
    if (ImGui::Button("Patreon", Widget::CalcSize(2)))
    {
        ShellExecute(nullptr, "open", "https://www.patreon.com/grinch_", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::Spacing();
    ImGui::Text("Copyright Grinch_ 2022-2025. All rights reserved.");
}

const wchar_t* Editor::GetFilterText()
{
    return FilterText;
}

bool Editor::DoesArchiveExist(const std::wstring &name)
{
    for (IMGArchive &arc : ArchiveList)
    {
        if (std::wstring(arc.FileName) == name)
        {
            return true;
        }
    }
    return false;
}

void Editor::SetUpdateFound()
{
    if (pApp)
    {
        pApp->SetPopup(UpdatePopUp);
    }
}

void Editor::ProcessMenuBar()
{
     if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New    (Ctrl + N)")) NewArchive();
        if (ImGui::MenuItem("Open... (Ctrl + O)")) OpenArchive();
        if (ImGui::MenuItem("Save   (Ctrl + S)", NULL, false, pSelectedArchive && !pSelectedArchive->Path.empty())) SaveArchive();
        if (ImGui::MenuItem("Save as... (Shift + S)", NULL, false, pSelectedArchive)) SaveArchiveAs();
        if (ImGui::MenuItem("Close (Shift + X)", NULL, false, pSelectedArchive)) CloseArchive(pSelectedArchive);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Import    (Ctrl + I)", NULL, false, pSelectedArchive)) ImportFiles();
        if (ImGui::MenuItem("Import & replace   (Shift + I)", NULL, false, pSelectedArchive)) ImportAndReplaceFiles();
        if (ImGui::MenuItem("Dump list", NULL, false, pSelectedArchive)) DumpList();
        if (ImGui::MenuItem("Export all    (Ctrl + E)", NULL, false, pSelectedArchive && !pSelectedArchive->ProgressBar.bInUse)) ExportAll();
        if (ImGui::MenuItem("Export selected    (Shift + E)", NULL, false, pSelectedArchive && !pSelectedArchive->ProgressBar.bInUse)) ExportSelected();
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Selection"))
    {
        if (ImGui::MenuItem("Select all    (Ctrl + A)", NULL, false, pSelectedArchive)) SelectAll();
        if (ImGui::MenuItem("Select Inverse   (Shift + A)", NULL, false, pSelectedArchive)) SelectInverse();
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Option"))
    {
        if (ImGui::BeginMenu("Theme", true))
        {
            Ui::eTheme theme = pApp->GetTheme();
            bool state = (theme == Ui::eTheme::Dark);
            if (ImGui::MenuItem("Dark theme", NULL, &state))
            {
                pApp->SetTheme(Ui::eTheme::Dark);
            }
            state = (theme == Ui::eTheme::Light);
            if (ImGui::MenuItem("Light theme", NULL, &state))
            {
                pApp->SetTheme(Ui::eTheme::Light);
            }
            state = (theme == Ui::eTheme::SystemDefault);
            if (ImGui::MenuItem("System default", NULL, &state))
            {
                pApp->SetTheme(Ui::eTheme::SystemDefault);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("Check for update"))
        {
            ShellExecute(nullptr, "open", "https://github.com/user-grinch/IMGEditor/releases", nullptr, nullptr, SW_SHOWNORMAL);
        }
        if (ImGui::MenuItem("Welcome"))
        {
            if (pApp)
            {
                pApp->SetPopup(WelcomePopup);
            }
        }
        if (ImGui::MenuItem("About"))
        {
            if (pApp)
            {
                pApp->SetPopup(AboutPopUp);
            }
        }
        ImGui::EndMenu();
    }
}

void Editor::ProcessContextMenu()
{
    if (!pContextEntry)
    {
        return;
    }

    // Restrict showing context menu outside window
    // ImGui::SetNextWindowPosConstraints()
    static float height = 0.0f;

    ImVec2 pos = ImGui::GetMousePos();
    float windowMaxY = pApp->GetWindowSize().y / 1.1f;
    if (pos.y + height > windowMaxY)
    {
        pos.y = windowMaxY - height;
    }
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.5f);
    if (ImGui::Begin("##Context", NULL, flags))
    {
        if (ImGui::MenuItem("Copy name"))
        {
            char buf[24];
            Utils::ConvertWideToUtf8(pContextEntry->FileName, buf, sizeof(buf));
            ImGui::SetClipboardText(buf);
            pContextEntry = nullptr;
        }
        if (ImGui::MenuItem("Delete"))
        {
            pContextEntry->bSelected = true;
            pSelectedArchive->EntryList.erase (
                std::remove_if (
                    pSelectedArchive->EntryList.begin(), 
                    pSelectedArchive->EntryList.end(), 
                    [](EntryInfo const& obj) {
                        return obj.bSelected;
                    }
                ),
                pSelectedArchive->EntryList.end()
            ); 
            pSelectedArchive->bUpdateSearch = true;
            pContextEntry = nullptr;
        }
        if (ImGui::MenuItem("Export"))
        {
            pContextEntry->bSelected = true;
            std::wstring path = WinDialogs::SaveFolder();
            if (path != L"") {
                ArchiveInfo *info  = new ArchiveInfo{pSelectedArchive, path};
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::ExportSelected, info, NULL, NULL);
            }
            pContextEntry = nullptr;
        }
        if (ImGui::MenuItem("Rename"))
        {
            if (pSelectedArchive)
            {
                for (EntryInfo &e : pSelectedArchive->EntryList)
                {
                    e.bRename  = false;
                }
                pContextEntry->bRename = true;
                pSelectedArchive->bUpdateSearch = true;
            }
            pContextEntry = nullptr;
        }
        height = ImGui::GetWindowHeight();
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsWindowHovered())
        {
            pContextEntry = nullptr;
        }
        ImGui::End();
    }
    
    ImGui::PopStyleVar();
}

void FixInputBug();

void Editor::ProcessWindow()
{
    float windowWidth = ImGui::GetWindowContentRegionWidth();
    ImGuiStyle &style = ImGui::GetStyle();
    bool blockHotkeys = false;

    if (Updater::IsUpdateAvailable())
    {
        if(pApp)
        {
            pApp->SetPopup(UpdatePopUp);
        }
    }

    ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs;
    pSelectedArchive = nullptr;
    if (ImGui::BeginTabBar("Archives", tabFlags))
    {
        for (IMGArchive &archive : ArchiveList)
        {  
            char buf[24];
            Utils::ConvertWideToUtf8(archive.FileName.c_str(), buf, sizeof(buf));
            if (ImGui::BeginTabItem(buf, &archive.bOpen))
            {
                pSelectedArchive = &archive;
                ImGui::Columns(2, NULL, false);
                float columnWidth = windowWidth/1.5f;
                ImGui::SetColumnWidth(0, columnWidth);
            
                // Search bar
                ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - style.ItemSpacing.x - style.WindowPadding.x);

                static char buf[256] = "";
                if (ImGui::InputTextWithHint("##Filter", "Search", buf, sizeof(buf)))
                {
                    Utils::ConvertUtf8ToWide(buf, FilterText, sizeof(buf));
                    Utils::ToLowerCase(FilterText);
                    pSelectedArchive->bUpdateSearch = true;
                }

                blockHotkeys = ImGui::IsItemActive();

                if (ImGui::BeginTable("ListedItems", 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    // Freeze the header row
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Name");
                    float scl = columnWidth / 445.5f;
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100 * scl);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 75 * scl);
                    ImGui::TableHeadersRow();

                    if (pSelectedArchive->bUpdateSearch) {
                        pSelectedArchive->UpdateSelectList(FilterText);
                        pSelectedArchive->bUpdateSearch = false;
                    }
                    
                    float height = ImGui::GetItemRectSize().y;
                    ImGuiListClipper clipper(static_cast<int>(pSelectedArchive->SelectedList.size()), height);
                    while (clipper.Step())
                    {
                        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                        {
                            EntryInfo *pEntry = archive.SelectedList[i];
                            
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                // Renaming system
                                if (pEntry->bRename)
                                {
                                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());

                                    char buf[24] = "";
                                    if (ImGui::InputText("##Rename", buf, sizeof(buf))) {
                                        Utils::ConvertUtf8ToWide(buf, pEntry->FileName, sizeof(pEntry->FileName));
                                    }
                                    if (ImGui::IsKeyPressed(VK_RETURN) 
                                    || (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered()))
                                    {
                                        pEntry->bRename = false;
                                    }
                                }
                                else
                                {
                                    bool styleApplied = false;
                                    if (pEntry->bSelected)
                                    {
                                        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
                                        styleApplied = true;
                                    }
                                    char buf[24] = "";
                                    Utils::ConvertWideToUtf8(pEntry->FileName, buf, sizeof(buf));
                                    if (ImGui::Selectable(buf, pEntry->bSelected))
                                    {
                                        ProcessSelection(pEntry, &archive);
                                    }

                                    if (styleApplied)
                                    {
                                        ImGui::PopStyleColor();
                                        styleApplied = false;
                                    }
                                    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                                    {
                                        pContextEntry = pContextEntry ? nullptr : pEntry;
                                    }
                                }

                                ImGui::TableNextColumn();
                                char buf[128] = "";
                                std::wstring str = pEntry->Type + L"##" + pEntry->FileName;
                                Utils::ConvertWideToUtf8(str.c_str(), buf, sizeof(buf));
                                if (ImGui::Selectable(buf, archive.EntryList[i].bSelected)) 
                                {
                                    ProcessSelection(pEntry, &archive);
                                }
                                if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                                {
                                    pContextEntry = pContextEntry ? nullptr : pEntry;
                                }
                                ImGui::TableNextColumn();
                                str = std::format(L"{} kb ## {}", pEntry->Sector*2, pEntry->FileName);
                                Utils::ConvertWideToUtf8(str.c_str(), buf, sizeof(buf));
                                if (ImGui::Selectable(buf, archive.EntryList[i].bSelected))
                                {
                                    ProcessSelection(pEntry, &archive);
                                }
                                if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                                {
                                    pContextEntry = pContextEntry ? nullptr : pEntry;
                                }
                        }
                    }
                    ProcessContextMenu();
                    ImGui::EndTable();
                }

                ImGui::NextColumn();

                ImVec2 barSz = Widget::CalcSize(1, true, true);
                ImGui::ProgressBar(pSelectedArchive->ProgressBar.Percentage, barSz);
                
                if (!pSelectedArchive->ProgressBar.bInUse)
                {
                    ImGui::BeginDisabled();
                }
                if (ImGui::Button("Cancel", barSz))
                {
                    pSelectedArchive->ProgressBar.bCancel = true;
                    pSelectedArchive->ProgressBar.Percentage = 0.0f;
                    pSelectedArchive->AddLogMessage(L"Canceled operation");
                }
                if (!pSelectedArchive->ProgressBar.bInUse)
                {
                    ImGui::EndDisabled();
                }

                ImGui::NewLine();
                ImGui::Text("IMG Format: %s", pSelectedArchive->GetFormatText().c_str());
                ImGui::Text("Total Entries: %d", pSelectedArchive->EntryList.size());
                ImGui::Spacing();
                
                if (ImGui::BeginTable("Log", 1, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders))
                {
                    // Freeze the header row
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Logs");
                    ImGui::TableHeadersRow();

                    int size = static_cast<int>(pSelectedArchive->LogList.size()-1);
                    char buf[256];
                    for (int i = size; i >= 0; --i)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        Utils::ConvertWideToUtf8(pSelectedArchive->LogList[i].c_str(), buf, sizeof(buf));
                        ImGui::Text(buf);
                    }
                    ImGui::EndTable();
                }

                ImGui::Columns(1);

                ImGui::EndTabItem();
            }

            // Remove element if closed form editor
            if (!archive.bOpen)
            {
                CloseArchive(&archive);
            }
        }

        ImGui::EndTabBar();
    }

    if (!blockHotkeys) {
        if (openFile.Pressed()) {
            OpenArchive();
        } else if (newFile.Pressed()) {
            NewArchive();
        } else if (saveFile.Pressed()) {
            SaveArchive();
        } else if (saveFileAs.Pressed()) {
            SaveArchiveAs();
        } else if (importFile.Pressed()) {
            ImportFiles();
        } else if (importReplaceFile.Pressed()) {
            ImportAndReplaceFiles();
        } else if (exportFile.Pressed()) {
            ExportAll();
        } else if (exportSelectedFile.Pressed()) {
            ExportSelected();
        } else if (selectAll.Pressed()) {
            SelectAll();
        } else if (selectInverse.Pressed()) {
            SelectInverse();
        } else if (closeTab.Pressed()) { 
            CloseArchive(pSelectedArchive);
        }
    }
}

void Editor::AddArchiveEntry(IMGArchive &&archive)
{
    if (!archive.bCreateNew)
    {
         for (IMGArchive &arc : ArchiveList)
        {
            if (arc.Path == archive.Path)
            {
                return;
            }
        }
    }

    ArchiveList.push_back(std::move(archive));
}


void Editor::Run()
{
    // std::string::data() doesn't work here?
    char buf[MAX_PATH];
    GetModuleFileName(NULL, buf, MAX_PATH);
    ConfigPath = buf;
    ConfigPath.replace(ConfigPath.end()-3, ConfigPath.end(), "ini");
    Config.LoadFile(ConfigPath.c_str());

    bool firstLaunch = Config.GetBoolValue("MAIN", "FirstLaunch", true);
    Ui::eTheme theme = static_cast<Ui::eTheme>(Config.GetLongValue("MAIN", "Theme", 
                    static_cast<long>(Ui::eTheme::SystemDefault)));

    Ui::Specification spec;
    spec.Name = "Grinch_'s IMG Editor v" EDITOR_VERSION;
    spec.MenuBarFunc = ProcessMenuBar;

    HMONITOR monitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info;
    info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &info);
    MonitorScale.x = (info.rcMonitor.right - info.rcMonitor.left) / 1366.0f;
    MonitorScale.y = (info.rcMonitor.bottom - info.rcMonitor.top) / 768.0f;
    spec.Size.x = static_cast<int>(700 * MonitorScale.x);
    spec.Size.y = static_cast<int>(500 * MonitorScale.y);

    spec.LayerFunc = ProcessWindow;
    spec.PopupFunc = firstLaunch ? WelcomePopup : nullptr;
    spec.CleanupFunc = Shutdown;
    pApp = new Ui::Application(spec);
    pApp->SetTheme(theme);
    pApp->Run();
}

void Editor::Shutdown()
{
    FILE *fp = fopen(ConfigPath.c_str(), "wb");
    if (fp)
    {
        Config.SetBoolValue("MAIN", "FirstLaunch", false);
        Config.SetLongValue("MAIN", "Theme", static_cast<long>(pApp->GetTheme()));
        Config.SaveFile(fp);
        fclose(fp);
    }
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // remove quotes
    for (size_t i = 0; i < MAX_PATH; ++i)
    {
        if (lpCmdLine[i] =='\0')
        {
            break;
        }
        if (lpCmdLine[i] =='"')
        {
            lpCmdLine[i] = '\0';
        }
    }
    
    bool exists = std::filesystem::exists(&lpCmdLine[1]); 
    wchar_t buf[256];   
    Utils::ConvertUtf8ToWide(&lpCmdLine[1], buf, sizeof(buf));
    Editor::AddArchiveEntry(exists ? IMGArchive(buf) : IMGArchive(L"Untitled", true));
    Updater::CheckUpdate();
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&Updater::Process, NULL, NULL, NULL);
    Editor::Run();
    return 0;
}

void Editor::NewArchive()
{
    if (DoesArchiveExist(L"Untitled"))
    {
        for (size_t i = 2; i < 20; ++i)
        {
            std::wstring name = std::format(L"Untitled({})", i);
            if (!DoesArchiveExist(name))
            {
                AddArchiveEntry({name, true});
                break;
            }
        }
    }
    else
    {
        AddArchiveEntry({L"Untitled", true});
    }
}

void Editor::OpenArchive()
{
    std::wstring path = WinDialogs::OpenFile();
    std::wstring fileName = std::filesystem::path(path).filename().stem().wstring();
    if (!DoesArchiveExist(fileName) && !path.empty())
    {
        if (IMGArchive::GetVersion(path) != eImgVer::Unknown)
        {
            AddArchiveEntry(std::move(IMGArchive(std::move(path))));
        }
        else
        {
            pApp->SetPopup([]()
            {
                ImGui::Text("IMG format not supported!");
                ImGui::Spacing();
                ImGui::Text("Supported formats:");
                ImGui::Text("1. GTA 3");
                ImGui::Text("2. GTA VC");
                ImGui::Text("3. GTA SA");
                ImGui::Text("4. Bully Scholarship Edition");
            });
        }
    }
}

void Editor::SaveArchive()
{
    if (pSelectedArchive && !pSelectedArchive->ProgressBar.bInUse)
    {
        std::filesystem::path fsPath(pSelectedArchive->Path);
        if (!pSelectedArchive->Path.empty() && fsPath.is_absolute() && std::filesystem::exists(fsPath)) {
            ArchiveInfo* info = new ArchiveInfo{pSelectedArchive, pSelectedArchive->Path, pSelectedArchive->GetVersion()};
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::Save, info, NULL, NULL);
            pSelectedArchive->AddLogMessage(L"Archive saved");
        } else {
            SaveArchiveAs();
        }
    }
}

void Editor::SaveArchiveAs()
{
    if (pSelectedArchive && !pSelectedArchive->ProgressBar.bInUse)
    {   
        std::wstring path = pSelectedArchive->FileName + L".img";
        eImgVer ver = static_cast<eImgVer>(WinDialogs::SaveArchive(path) - 1);
        ArchiveInfo* info = new ArchiveInfo{pSelectedArchive, path, ver, false};
        pSelectedArchive->SetVersion(ver);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::Save, info, NULL, NULL);
        pSelectedArchive->AddLogMessage(L"Archive saved");
    }
}

void Editor::ImportFiles()
{
    std::wstring fileNames = WinDialogs::ImportFiles();
    if (!fileNames.empty())
    {
        ArchiveInfo info = {pSelectedArchive, fileNames, eImgVer::Unknown, false};
        IMGArchive::ImportEntries(&info);
    }
}

void Editor::ImportAndReplaceFiles()
{
    std::wstring fileNames = WinDialogs::ImportFiles();
    if (!fileNames.empty())
    {
        ArchiveInfo info = {pSelectedArchive, fileNames, eImgVer::Unknown, true};
        IMGArchive::ImportEntries(&info);
    }
}

void Editor::ExportAll()
{
    std::wstring path = WinDialogs::SaveFolder();
    if (!path.empty())
    {
        ArchiveInfo* info = new ArchiveInfo{pSelectedArchive, path};
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::ExportAll, info, NULL, NULL);
    }
}

void Editor::ExportSelected()
{
    std::wstring path = WinDialogs::SaveFolder();
    if (!path.empty())
    {
        ArchiveInfo* info = new ArchiveInfo{pSelectedArchive, path};
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::ExportSelected, info, NULL, NULL);
    }
}

void Editor::SelectAll()
{
    for (EntryInfo &e : pSelectedArchive->EntryList)
    {   
        e.bSelected = true;
    }
}

void Editor::SelectInverse()
{
    for (EntryInfo &e : pSelectedArchive->EntryList)
    {   
        e.bSelected = !e.bSelected;
    }
}

void Editor::DumpList()
{
    FILE *fp = _wfopen(std::format(L"C:/Users/Public/Desktop/{}.txt", pSelectedArchive->FileName).c_str(), L"w");
    if (fp)
    {
        fwprintf(fp, L"Dumped list from %s.img\n", pSelectedArchive->FileName.c_str());
        fwprintf(fp, L"Total entries %d\n\n", static_cast<int>(pSelectedArchive->EntryList.size()));
        for (EntryInfo &e : pSelectedArchive->EntryList)
        {   
            fwprintf(fp, L"%s\n", e.FileName);
        }
        pSelectedArchive->AddLogMessage(L"Dumped to desktop");
        fclose(fp);
    }
}

void Editor::CloseArchive(IMGArchive *pArchive)
{
    ArchiveList.erase (
        std::remove_if (
            ArchiveList.begin(), 
            ArchiveList.end(), 
            [pArchive](IMGArchive const& obj) {
                return obj.bCreateNew ? obj.FileName == pArchive->FileName : obj.Path == pArchive->Path;
            }
        ), 
        ArchiveList.end()
    );
}


void Editor::ProcessSelection(EntryInfo *pEntry, IMGArchive *pArchive) 
{
    bool isShiftDown = GetAsyncKeyState(VK_SHIFT) & 0x8000;
    bool isCtrlDown = GetAsyncKeyState(VK_CONTROL) & 0x8000;
    int lastSelectedIndex = -1;
    int clickedIndex = -1;

    for (int i = 0; i < pArchive->EntryList.size(); i++)
    {
        if (pArchive->EntryList[i].bSelected)
        {
            lastSelectedIndex = i;  
        }
        if (&pArchive->EntryList[i] == pEntry)
        {
            clickedIndex = i;  
        }
    }

    if (isShiftDown && lastSelectedIndex != -1 && clickedIndex != -1)
    {
        int start = min(lastSelectedIndex, clickedIndex);
        int end = max(lastSelectedIndex, clickedIndex);

        for (int i = 0; i < pArchive->EntryList.size(); i++)
        {
            pArchive->EntryList[i].bRename = false;  

            if (i >= start && i <= end)
            {
                pArchive->EntryList[i].bSelected = true;
            }
            else if (!isCtrlDown)
            {
                pArchive->EntryList[i].bSelected = false;
            }
        }
    }
    else if (!isCtrlDown)
    {
        for (auto &e : pArchive->EntryList)
        {
            e.bSelected = false;
        }
    }

    if (!isShiftDown) 
    {
        pEntry->bSelected = !pEntry->bSelected;
    }
}