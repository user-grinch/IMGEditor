#include "pch.h"
#include "editor.h"
#include "widget.h"
#include "windialogs.h"
#define MENU_VERSION "1.0-alpha"

void Editor::AboutPopUp()
{
    ImGui::Columns(2, nullptr, false);
    ImGui::Text("Author: Grinch_");
    ImGui::Text("Version: " MENU_VERSION);
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
    ImGui::Text("Copyright Grinch_ 2022-2023. All rights reserved.");
}

void Editor::WelcomePopup()
{
    Widget::TextCentered("Welcome to IMG Editor v" MENU_VERSION);
    Widget::TextCentered("by Grinch_");
    ImGui::Spacing();
    ImGui::TextWrapped("This editor is still work in progress. There may be LOT of bugs. Feel free to report if you find any.");
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::Text("Supported IMG formats,");
    ImGui::Text("1. GTA 3");
    ImGui::Text("2. GTA VC");
    ImGui::Text("3. GTA SA");
    ImGui::Text("4. Fastman92's format");
    ImGui::Spacing();
    ImGui::TextWrapped("Directly open archives in IMG Editor by windows modifying file association.");
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
    ImGui::Text("Copyright Grinch_ 2022-2023. All rights reserved.");
}


bool Editor::DoesArchiveExist(const std::string &name)
{
    for (IMGArchive &arc : ArchiveList)
    {
        if (std::string(arc.FileName) == name)
        {
            return true;
        }
    }
    return false;
}

void Editor::ProcessMenuBar()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New"))
        {
            if (DoesArchiveExist("Untitled"))
            {
                // 50 is kinda overkill
                for (size_t i = 2; i < 50; ++i)
                {
                    std::string name = std::format("Untitled({})", i);
                    if (!DoesArchiveExist(name))
                    {
                        AddArchiveEntry({name, true});
                        break;
                    }
                }
            }
            else
            {
                AddArchiveEntry({"Untitled", true});
            }
        }
        if (ImGui::MenuItem("Open..."))
        {
            std::string path = WinDialogs::OpenFile();
            std::string fileName = std::filesystem::path(path).filename().stem().string();
            if (!DoesArchiveExist(fileName))
            {
                if (path != "")
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
                            ImGui::Text("Supported formats,");
                            ImGui::Text("1. IMG v1 (GTA III & VC)");
                            ImGui::Text("2. IMG v2 (GTA SA)");
                            ImGui::Text("3. Fastman92's format (GTA SA with FLA plugin)");
                        });
                    }
                }
            }
        }
        if (ImGui::MenuItem("Save") && !pSelectedArchive->ProgressBar.bInUse)
        {
            if (pSelectedArchive->Path == "")
            {
                pSelectedArchive->Path = WinDialogs::SaveFile(pSelectedArchive->FileName + ".img");
            }
            ArchiveInfo *info  = new ArchiveInfo{pSelectedArchive, pSelectedArchive->Path};
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::Save, info, NULL, NULL);
            pSelectedArchive->AddLogMessage("Archive saved");
        }
        if (ImGui::MenuItem("Save as...") && !pSelectedArchive->ProgressBar.bInUse)
        {
            std::string path = WinDialogs::SaveFile(pSelectedArchive->FileName + ".img");
            ArchiveInfo *info  = new ArchiveInfo{pSelectedArchive, path};
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::Save, info, NULL, NULL);
            pSelectedArchive->AddLogMessage("Archive saved");
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Import"))
        {
            std::string fileNames = WinDialogs::ImportFiles();
            pSelectedArchive->ImportEntries(fileNames);
        }
        if (ImGui::MenuItem("Import & replace"))
        {
            std::string fileNames = WinDialogs::ImportFiles();
            pSelectedArchive->ImportEntries(fileNames, true);
        }
        if (ImGui::MenuItem("Export all", NULL, false, !pSelectedArchive->ProgressBar.bInUse))
        {
            std::string path = WinDialogs::SaveFolder();
            ArchiveInfo *info  = new ArchiveInfo{pSelectedArchive, path};
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::ExportAll, info, NULL, NULL);
        }
        if (ImGui::MenuItem("Export selected", NULL, false, !pSelectedArchive->ProgressBar.bInUse))
        {
            std::string path = WinDialogs::SaveFolder();
            ArchiveInfo *info  = new ArchiveInfo{pSelectedArchive, path};
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::ExportSelected, info, NULL, NULL);
        }
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
                        return !strcmp(obj.FileName, pContextEntry->FileName);
                    }
                ),
                pSelectedArchive->EntryList.end()
            ); 
            pContextEntry = nullptr;
        }

        if (ImGui::MenuItem("Export"))
        {
            std::string path = WinDialogs::SaveFile(pContextEntry->FileName);
            pSelectedArchive->ExportEntry(pContextEntry, path);
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
            }
            pContextEntry = nullptr;
        }
        if (ImGui::MenuItem("Copy name"))
        {
            ImGui::SetClipboardText(pContextEntry->FileName);
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

    ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs;
    pSelectedArchive = nullptr;
    if (ImGui::BeginTabBar("Archives", tabFlags))
    {
        for (IMGArchive &archive : ArchiveList)
        {  
            if (ImGui::BeginTabItem(archive.FileName.c_str(), &archive.bOpen))
            {
                pSelectedArchive = &archive;
                ImGui::Columns(2, NULL, false);
                float columnWidth = windowWidth/1.5f;
                ImGui::SetColumnWidth(0, columnWidth);
            
                // Search bar
                ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - style.ItemSpacing.x - style.WindowPadding.x);
                static ImGuiTextFilter filter;
                std::string hint = std::format("Search  {}", archive.Path);
                Widget::Filter("##Search", filter, hint.c_str()); 
                if (ImGui::BeginTable("ListedItems", 4, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    // Freeze the header row
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Name");
                    float scl = columnWidth / 445.5f;
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100 * scl);
                    ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed, 50 * scl);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 50 * scl);
                    ImGui::TableHeadersRow();

                    static ImGuiListClipper clipper;
                    clipper.Begin(static_cast<int>(archive.EntryList.size()), ImGui::GetTextLineHeight());
                    while (clipper.Step())
                    {
                        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                        {
                            EntryInfo &entry = archive.EntryList[i];
                            if (filter.PassFilter(entry.FileName))
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();

                                // Renaming system
                                if (entry.bRename)
                                {
                                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                                    ImGui::InputText("##Rename", entry.FileName, sizeof(entry.FileName));
                                    if (ImGui::IsKeyPressed(VK_RETURN) 
                                    || (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered()))
                                    {
                                        entry.bRename = false;
                                    }
                                }
                                else
                                {
                                    bool styleApplied = false;
                                    if (entry.bSelected)
                                    {
                                        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
                                        styleApplied = true;
                                    }
                                    if (ImGui::Selectable(entry.FileName, entry.bSelected))
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
                                                if (!strcmp(e.FileName, entry.FileName))
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
                                                    e.bSelected  = false;
                                                }
                                            }
                                            e.bRename  = false;
                                        }
                                        entry.bSelected = true;
                                    }

                                    if (styleApplied)
                                    {
                                        ImGui::PopStyleColor();
                                        styleApplied = false;
                                    }
                                    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                                    {
                                        pContextEntry = pContextEntry ? nullptr : &entry;
                                    }
                                }

                                ImGui::TableNextColumn();
                                ImGui::Text(entry.Type.c_str());
                                ImGui::TableNextColumn();
                                ImGui::Text(std::format("0x{:X}", entry.Offset).c_str());
                                ImGui::TableNextColumn();
                                ImGui::Text("%d kb", entry.Size*2);
                            }
                        }
                    }
                    clipper.End();
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
                    pSelectedArchive->AddLogMessage("Canceled operation");
                }
                if (!pSelectedArchive->ProgressBar.bInUse)
                {
                    ImGui::EndDisabled();
                }

                ImGui::Spacing();
                ImVec2 sz = Widget::CalcSize(2, true, true);
                if (ImGui::Button("Import", sz))
                {
                    std::string fileNames = WinDialogs::ImportFiles();
                    pSelectedArchive->ImportEntries(fileNames);
                }
                ImGui::SameLine();

                if (ImGui::Button("Export all", sz) && !pSelectedArchive->ProgressBar.bInUse)
                {
                    std::string path = WinDialogs::SaveFolder();
                    ArchiveInfo *info  = new ArchiveInfo{pSelectedArchive, path};
                    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&IMGArchive::ExportAll, info, NULL, NULL);
                }
                if (ImGui::Button("Select all", sz))
                {
                    for (EntryInfo &e : pSelectedArchive->EntryList)
                    {   
                        e.bSelected = true;
                    }
                    pSelectedArchive->AddLogMessage("Selected all entries");
                }
                ImGui::SameLine();
                if (ImGui::Button("Select inverse", sz))
                {
                    for (EntryInfo &e : pSelectedArchive->EntryList)
                    {   
                        e.bSelected = !e.bSelected;
                    }
                    pSelectedArchive->AddLogMessage("Selection inversed");
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
                }
                ImGui::SameLine();
                if (ImGui::Button("Dump list", sz))
                {
                    
                    FILE *fp = fopen(std::format("C:/Users/User/Desktop/{}.txt", pSelectedArchive->FileName).c_str(), "w");
                    if (fp)
                    {
                        fprintf(fp, "Dumped list from %s.img\n", pSelectedArchive->FileName.c_str());
                        fprintf(fp, "Total entries %d\n\n", static_cast<int>(pSelectedArchive->EntryList.size()));
                        for (EntryInfo &e : pSelectedArchive->EntryList)
                        {   
                            fprintf(fp, "%s\n", e.FileName);
                        }
                        pSelectedArchive->AddLogMessage("Dumped to desktop");
                        fclose(fp);
                    }
                }
                ImGui::Spacing();

                switch (pSelectedArchive->ImageVersion)
                {
                case eImgVer::One:
                    ImGui::Text("IMG format: 1 (III/VC/BULLY)");
                    break;
                case eImgVer::Two:
                    ImGui::Text("IMG format: 2 (SA)");
                    break;
                default:
                    ImGui::Text("IMG format: Unknown");
                    break;
                }
                ImGui::Text("Total entries: %d", pSelectedArchive->EntryList.size());
                
                ImGui::Spacing();
                
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
                        ImGui::Text(pSelectedArchive->LogList[i].c_str());
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
}

void Editor::AddArchiveEntry(IMGArchive &&archive)
{
    for (IMGArchive &arc : ArchiveList)
    {
        if (arc.Path == archive.Path)
        {
            return;
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
    spec.Name = "IMG Editor v" MENU_VERSION;
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
    Editor::AddArchiveEntry(exists ? IMGArchive(&lpCmdLine[1]) : IMGArchive("Untitled", true));
    Editor::Run();
    return 0;
}