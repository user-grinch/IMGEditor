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

void Editor::ProcessMenuBar()
{
    if (ImGui::BeginMenu("File"))
    {
        ImGui::MenuItem("New");
        if (ImGui::MenuItem("Open..."))
        {
            std::string path = WinDialogs::OpenFile();
            if (path != "")
            {
                Archives.push_back(std::move(IMGArchive(std::move(path))));
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
    if (!pSelectedEntry)
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
            pSelectedEntry = nullptr;
        }

        if (ImGui::MenuItem("Export"))
        {
            std::string path = WinDialogs::SaveFile(pSelectedEntry->name);
            pSelectedArchive->ExportEntry(pSelectedEntry, path);
            pSelectedEntry = nullptr;
        }

        if (ImGui::MenuItem("Rename"))
        {
            if (pSelectedArchive)
            {
                for (EntryInfo &e : pSelectedArchive->Entries)
                {
                    e.rename  = false;
                }
                pSelectedEntry->rename = true;
            }
            pSelectedEntry = nullptr;
        }

        if (ImGui::MenuItem("Close"))
        {
            pSelectedEntry = nullptr;
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
        for (IMGArchive &archive : Archives)
        {  
            if (ImGui::BeginTabItem(archive.FileName.c_str(), &archive.Open))
            {
                pSelectedArchive = &archive;
                ImGui::Columns(2, NULL, false);
                ImGui::SetColumnWidth(0, windowWidth/1.5f);
                // Search bar
                ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - style.ItemSpacing.x - style.WindowPadding.x);

                static ImGuiTextFilter filter;
                Widget::Filter("##Search", filter, "Search");
                if (ImGui::BeginTable("ListedItems", 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    // Freeze the header row
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 50);
                    ImGui::TableHeadersRow();
                    for (EntryInfo &entry : archive.Entries)
                    {
                        if (filter.PassFilter(entry.name))
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();

                            // Renaming system
                            if (entry.rename)
                            {
                                ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                                ImGui::InputText("##Rename", entry.name, sizeof(entry.name));
                                if (ImGui::IsKeyPressed(VK_RETURN))
                                {
                                    entry.rename = false;
                                }
                            }
                            else
                            {
                                if (ImGui::Selectable(entry.name))
                                {
                                    // selection & renanme disable
                                    for (EntryInfo &e : archive.Entries)
                                    {
                                        // allow multiselect when ctrl is pressed
                                        if (!(ImGui::IsKeyDown(VK_LCONTROL) || ImGui::IsKeyDown(VK_RCONTROL)))
                                        {
                                            e.selected  = false;
                                        }
                                        e.rename  = false;
                                    }

                                    entry.selected = true;
                                }
                                if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                                {
                                    pSelectedEntry = pSelectedEntry ? nullptr : &entry;
                                }
                            }

                            ImGui::TableNextColumn();
                            ImGui::Text(entry.fileType.c_str());
                            ImGui::TableNextColumn();
                            ImGui::Text("%d kb", entry.size*2);
                        }
                    }
                    ProcessContextMenu();
                    ImGui::EndTable();
                }

                ImGui::NextColumn();
                // ImGui::Text("Total lines: %d", archive.Entries.size(): 0);
                ImVec2 sz = Widget::CalcSize(2, true, true);

                ImGui::Button("Import", sz);
                ImGui::SameLine();
                ImGui::Button("Select all", sz);

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

                    for (int i = 0; i < 30; ++i)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::MenuItem(std::format("Test {}", i).c_str());
                    }
                    ImGui::EndTable();
                }

                ImGui::Columns(1);

                ImGui::EndTabItem();
            }

            // Remove element if closed form editor
            if (!archive.Open)
            {
                Archives.erase (
                    std::remove_if (
                        Archives.begin(), 
                        Archives.end(), 
                        //here comes the C++11 lambda:
                        [&archive](IMGArchive const& obj) {
                            return obj.Path == archive.Path;
                        }
                    ), 
                    Archives.end()
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