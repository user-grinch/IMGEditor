#include "pch.h"
#include "editor.h"
#include "widget.h"
#include "windialogs.h"
#define MENU_VERSION "1.0"

void Editor::AboutPopUp()
{
    ImGui::Columns(2, nullptr, false);
    ImGui::Text("Author: Grinch_");
    ImGui::Text("Version: " MENU_VERSION);
    ImGui::Spacing();

    if (ImGui::Button("GitHub", Widget::CalcSize(2)))
    {
        ShellExecute(nullptr, "open", "https://github.com/user-grinch/IMGEditor/", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::NextColumn();
    ImGui::Text("ImGui: %s", ImGui::GetVersion());
    ImGui::Text("Build: %s", __DATE__ );
    ImGui::Spacing();
    if (ImGui::Button("Patreon", Widget::CalcSize(2)))
    {
        ShellExecute(nullptr, "open", "https://www.patreon.com/grinch_", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::Columns(1);
    ImGui::Spacing();
    ImGui::Text("Copyright Grinch_ 2022. All rights reserved.");
}

void Editor::WelcomePopup()
{
    Widget::TextCentered("Welcome to IMG Editor v" MENU_VERSION);
    Widget::TextCentered("by Grinch_" MENU_VERSION);
    ImGui::Spacing();
    
    ImGui::Text("Supported IMG formats,");
    ImGui::Text("1. GTA 3");
    ImGui::Text("2. GTA VC");
    ImGui::Text("3. GTA SA");
    ImGui::Text("4. Fastman92's format");

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
    ImGui::Text("Copyright Grinch_ 2022. All rights reserved.");
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
                        ArchiveList.push_back({name, true});
                        break;
                    }
                }
            }
            else
            {
                ArchiveList.push_back({"Untitled", true});
            }
        }
        if (ImGui::MenuItem("Open..."))
        {
            std::string path = WinDialogs::OpenFile();

            if (path != "" && IMGArchive::IsSupported(path))
            {
                ArchiveList.push_back(std::move(IMGArchive(std::move(path))));
            }
            else
            {
                pApp->SetPopup([]()
                {
                    ImGui::Text("IMG format not supported!");
                    ImGui::Spacing();
                    ImGui::Text("IMG Editor only supports below games,");
                    ImGui::Text("1. GTA SA");
                });
            }
        }
        ImGui::MenuItem("Save");
        ImGui::MenuItem("Save as...");
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

    ImVec2 pos = ImGui::GetMousePos();
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.5f);
    ImGui::SetNextWindowFocus();
    if (ImGui::Begin("##Context", NULL, flags))
    {
        if (ImGui::MenuItem("Delete"))
        {
            std::remove_if (
                pSelectedArchive->EntryList.begin(), 
                pSelectedArchive->EntryList.end(), 
                [](EntryInfo const& obj) {
                    return !strcmp(obj.FileName, pContextEntry->FileName);
                }
            ), 
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

        if (ImGui::MenuItem("Close"))
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
                ImGui::SetColumnWidth(0, windowWidth/1.5f);
                // Search bar
                ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - style.ItemSpacing.x - style.WindowPadding.x);

                static ImGuiTextFilter filter;
                std::string hint = std::format("Search  -  Total entries: {}", pSelectedArchive->EntryList.size());
                Widget::Filter("##Search", filter, hint.c_str());
                if (ImGui::BeginTable("ListedItems", 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    // Freeze the header row
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 50);
                    ImGui::TableHeadersRow();
                    for (EntryInfo &entry : archive.EntryList)
                    {
                        if (filter.PassFilter(entry.FileName))
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();

                            // Renaming system
                            if (entry.bRename)
                            {
                                ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                                ImGui::InputText("##Rename", entry.FileName, sizeof(entry.FileName));
                                if (ImGui::IsKeyPressed(VK_RETURN))
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
                                    for (EntryInfo &e : archive.EntryList)
                                    {
                                        // allow multiselect when ctrl is pressed
                                        if (!(ImGui::IsKeyDown(VK_LCONTROL) || ImGui::IsKeyDown(VK_RCONTROL)))
                                        {
                                            e.bSelected  = false;
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
                            ImGui::Text("%d kb", entry.Size*2);
                        }
                    }
                    ProcessContextMenu();
                    ImGui::EndTable();
                }

                ImGui::NextColumn();
                ImVec2 sz = Widget::CalcSize(2, true, true);

                ImGui::Button("Import", sz);
                ImGui::SameLine();
                if (ImGui::Button("Select all", sz))
                {
                    for (EntryInfo &e : pSelectedArchive->EntryList)
                    {   
                        e.bSelected = true;
                    }
                    pSelectedArchive->AddLogMessage("Selected all entries");
                }

                ImGui::Button("Export", sz);
                ImGui::SameLine();
                ImGui::Button("Export all", sz);

                ImGui::Button("Dump list", sz);
                ImGui::SameLine();
                ImGui::Button("Merge", sz);

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

void Editor::Run()
{
    Ui::Specification spec;
    spec.Name = "IMG Editor v" MENU_VERSION;
    spec.MenuBarFunc = ProcessMenuBar;
    spec.Size = { 600, 500 };

    spec.LayerFunc = ProcessWindow;
    spec.PopupFunc = WelcomePopup;
    pApp = new Ui::Application(spec);
    pApp->Run();
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Editor::Run();
    return 0;
}