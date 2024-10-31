#include "pch.h"
#include "editor.h"
#include "widget.h"
#include "windialogs.h"
#include "updater.h"
#include "hotkeys.h"
#include <codecvt>

std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

Hotkey openFile {ImGuiKey_ModCtrl, ImGuiKey_O};
Hotkey newFile {ImGuiKey_ModCtrl, ImGuiKey_N};
Hotkey saveFile {ImGuiKey_ModCtrl, ImGuiKey_S};
Hotkey saveFileAs {ImGuiKey_ModShift, ImGuiKey_S};
Hotkey importFile {ImGuiKey_ModCtrl, ImGuiKey_I};
Hotkey importReplaceFile {ImGuiKey_ModShift, ImGuiKey_I};
Hotkey exportFile {ImGuiKey_ModCtrl, ImGuiKey_E};
Hotkey exportSelectedFile {ImGuiKey_ModShift, ImGuiKey_E};

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
    static wchar_t buf[256];
    ConvertCharToWideChar(Filter.InputBuf, buf, sizeof(buf));
    assert(sizeof(Filter.InputBuf) != sizeof(buf));
    return buf;
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
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Import    (Ctrl + I)", NULL, false, pSelectedArchive)) ImportFiles();
        if (ImGui::MenuItem("Import & replace   (Shift + I)", NULL, false, pSelectedArchive)) ImportAndReplaceFiles();
        if (ImGui::MenuItem("Export all    (Ctrl + E)", NULL, false, pSelectedArchive && !pSelectedArchive->ProgressBar.bInUse)) ExportAll();
        if (ImGui::MenuItem("Export selected    (Shift + E)", NULL, false, pSelectedArchive && !pSelectedArchive->ProgressBar.bInUse)) ExportSelected();
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
    ImGui::SetNextWindowFocus();
    if (ImGui::Begin("##Context", NULL, flags))
    {
        if (ImGui::MenuItem("Delete"))
        {
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
            wchar_t buf[256];
            ConvertCharToWideChar(Filter.InputBuf, buf, sizeof(buf));
            assert(sizeof(Filter.InputBuf) != sizeof(buf));
            pSelectedArchive->UpdateSelectList(buf);
            pContextEntry = nullptr;
        }

        if (ImGui::MenuItem("Export"))
        {
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
                wchar_t buf[256];
                ConvertCharToWideChar(Filter.InputBuf, buf, sizeof(buf));
                assert(sizeof(Filter.InputBuf) != sizeof(buf));
                pSelectedArchive->UpdateSelectList(buf);
            }
            pContextEntry = nullptr;
        }
        if (ImGui::MenuItem("Copy name"))
        {
            ImGui::SetClipboardText(converter.to_bytes(pContextEntry->FileName).c_str());
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

void Editor::ProcessWindow()
{
    float windowWidth = ImGui::GetWindowContentRegionWidth();
    ImGuiStyle &style = ImGui::GetStyle();

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
            if (ImGui::BeginTabItem(converter.to_bytes(archive.FileName).c_str(), &archive.bOpen))
            {
                pSelectedArchive = &archive;
                ImGui::Columns(2, NULL, false);
                float columnWidth = windowWidth/1.5f;
                ImGui::SetColumnWidth(0, columnWidth);
            
                // Search bar
                ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - style.ItemSpacing.x - style.WindowPadding.x);
                std::wstring hint = std::format(L"Search  {}", archive.Path);
                if (Widget::Filter("##Search", Filter, converter.to_bytes(hint).c_str()))
                {
                    wchar_t buf[256];
                    ConvertCharToWideChar(Filter.InputBuf, buf, sizeof(buf));
                    assert(sizeof(Filter.InputBuf) != sizeof(buf));
                    pSelectedArchive->UpdateSelectList(buf);
                }

                if (ImGui::BeginTable("ListedItems", 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    // Freeze the header row
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Name");
                    float scl = columnWidth / 445.5f;
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100 * scl);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 75 * scl);
                    ImGui::TableHeadersRow();
                   
                    float height = ImGui::GetItemRectSize().y;
                    ImGuiListClipper clipper(static_cast<int>(pSelectedArchive->SelectedList.size()), height);
                    while (clipper.Step())
                    {
                        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                        {
                            EntryInfo *pEntry = archive.SelectedList[i];
                            if (Filter.PassFilter(converter.to_bytes(pEntry->FileName).c_str()))
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                // Renaming system
                                if (pEntry->bRename)
                                {
                                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());

                                    char buf[24];
                                    memset(buf, 0, sizeof(buf));
                                    assert(sizeof(buf) != sizeof(pEntry->FileName));
                                    if (ImGui::InputText("##Rename", buf, sizeof(buf))) {
                                        ConvertCharToWideChar(buf, pEntry->FileName, sizeof(pEntry->FileName));
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
                                    if (ImGui::Selectable(converter.to_bytes(pEntry->FileName).c_str(), pEntry->bSelected))
                                    {
                                        // selection & renanme disable
                                        bool prevSelected = false;
                                        for (EntryInfo &e : archive.EntryList)
                                        {
                                            // allow shift multiselect
                                            ImVec2 min = ImGui::GetItemRectMin();
                                            if (ImGui::IsKeyDown(VK_LSHIFT) || ImGui::IsKeyDown(VK_RSHIFT))
                                            {
                                                if (prevSelected)
                                                {
                                                    e.bSelected = true;
                                                }
                                                if (!wcscmp(e.FileName, pEntry->FileName))
                                                {
                                                    prevSelected = false;
                                                }
                                                else
                                                {
                                                    prevSelected = e.bSelected;
                                                }
                                            }
                                            else
                                            {
                                                // allow multiselect when ctrl is pressed
                                                if (!(ImGui::IsKeyDown(VK_LCONTROL) || ImGui::IsKeyDown(VK_RCONTROL)))
                                                {
                                                    e.bSelected = false;
                                                }
                                            }
                                            e.bRename  = false;
                                        }
                                        pEntry->bSelected = !pEntry->bSelected;
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
                                ImGui::Text(converter.to_bytes(pEntry->Type).c_str());
                                ImGui::TableNextColumn();
                                ImGui::Text("%d kb", pEntry->Size*2);
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

                ImGui::Spacing();
                ImVec2 sz = Widget::CalcSize(2, true, true);
                if (ImGui::Button("Import", sz))
                {
                    std::wstring fileNames = WinDialogs::ImportFiles();
                    if (fileNames != L"") {
                        ArchiveInfo *info  = new ArchiveInfo{pSelectedArchive, fileNames, eImgVer::Unknown, false};
                        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::ImportEntries, info, NULL, NULL);
                    }
                }
                ImGui::SameLine();

                if (ImGui::Button("Export all", sz) && !pSelectedArchive->ProgressBar.bInUse)
                {
                    std::wstring path = WinDialogs::SaveFolder();
                    if (path != L"") {
                        ArchiveInfo *info  = new ArchiveInfo{pSelectedArchive, path};
                        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::ExportAll, info, NULL, NULL);
                    }
                }
                if (ImGui::Button("Select all", sz))
                {
                    for (EntryInfo &e : pSelectedArchive->EntryList)
                    {   
                        e.bSelected = true;
                    }
                    pSelectedArchive->AddLogMessage(L"Selected all entries");
                }
                ImGui::SameLine();
                if (ImGui::Button("Select inverse", sz))
                {
                    for (EntryInfo &e : pSelectedArchive->EntryList)
                    {   
                        e.bSelected = !e.bSelected;
                    }
                    pSelectedArchive->AddLogMessage(L"Selection inversed");
                }
                if (ImGui::Button("Delete selection", sz))
                {
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
                    wchar_t buf[256];
                    ConvertCharToWideChar(Filter.InputBuf, buf, sizeof(buf));
                    assert(sizeof(Filter.InputBuf) != sizeof(buf));
                    pSelectedArchive->UpdateSelectList(buf);
                }
                ImGui::SameLine();
                if (ImGui::Button("Dump list", sz))
                {
                    FILE *fp = _wfopen(std::format(L"C:/Users/Public/Desktop/{}.txt", pSelectedArchive->FileName).c_str(), L"w");
                    if (fp)
                    {
                        fprintf(fp, "Dumped list from %s.img\n", pSelectedArchive->FileName.c_str());
                        fprintf(fp, "Total entries %d\n\n", static_cast<int>(pSelectedArchive->EntryList.size()));
                        for (EntryInfo &e : pSelectedArchive->EntryList)
                        {   
                            fprintf(fp, "%s\n", e.FileName);
                        }
                        pSelectedArchive->AddLogMessage(L"Dumped to desktop");
                        fclose(fp);
                    }
                }
                switch (pSelectedArchive->ImageVersion)
                {
                case eImgVer::One:
                    ImGui::Text("IMG format: v1");
                    break;
                case eImgVer::Two:
                    ImGui::Text("IMG format: v2");
                    break;
                default:
                    ImGui::Text("IMG format: Unknown");
                    break;
                }
                ImGui::SameLine();
                ImGui::Text("Entries: %d", pSelectedArchive->EntryList.size());
                if (ImGui::BeginTable("Log", 1, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders))
                {
                    // Freeze the header row
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Logs");
                    ImGui::TableHeadersRow();

                    int size = static_cast<int>(pSelectedArchive->LogList.size()-1);
                    for (int i = size; i >= 0; --i)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text(converter.to_bytes(pSelectedArchive->LogList[i]).c_str());
                    }
                    ImGui::EndTable();
                }

                ImGui::Columns(1);

                ImGui::EndTabItem();
            }

            // Remove element if closed form editor
            if (!archive.bOpen)
            {
                ArchiveList.erase (
                    std::remove_if (
                        ArchiveList.begin(), 
                        ArchiveList.end(), 
                        
                        [&archive](IMGArchive const& obj) {
                            return obj.bCreateNew ? obj.FileName == archive.FileName : obj.Path == archive.Path;
                        }
                    ), 
                    ArchiveList.end()
                );
            }
        }

        ImGui::EndTabBar();
    }

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
    spec.Name = "IMG Editor v" EDITOR_VERSION;
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
    Editor::AddArchiveEntry(exists ? IMGArchive(converter.from_bytes(&lpCmdLine[1])) : IMGArchive(L"Untitled", true));
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
            ArchiveInfo* info = new ArchiveInfo{pSelectedArchive, pSelectedArchive->Path, pSelectedArchive->ImageVersion};
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
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::Save, info, NULL, NULL);
        pSelectedArchive->AddLogMessage(L"Archive saved");
    }
}

void Editor::ImportFiles()
{
    std::wstring fileNames = WinDialogs::ImportFiles();
    if (!fileNames.empty())
    {
        ArchiveInfo* info = new ArchiveInfo{pSelectedArchive, fileNames, eImgVer::Unknown, false};
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::ImportEntries, info, NULL, NULL);
    }
}

void Editor::ImportAndReplaceFiles()
{
    std::wstring fileNames = WinDialogs::ImportFiles();
    if (!fileNames.empty())
    {
        ArchiveInfo* info = new ArchiveInfo{pSelectedArchive, fileNames, eImgVer::Unknown, true};
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::ImportEntries, info, NULL, NULL);
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